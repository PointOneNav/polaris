/**************************************************************************/ /**
 * @brief Polaris client C++ wrapper base class.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/polarispp.h"

#if defined(POLARIS_NO_GLOG)
#include <iostream>

// Reference:
// https://stackoverflow.com/questions/49332013/adding-a-new-line-after-stdostream-output-without-explicitly-calling-it
class StreamDelegate {
 public:
  ~StreamDelegate() { std::cerr << std::endl; }

  template <typename T>
  StreamDelegate& operator<<(T&& val) {
    std::cerr << std::forward<T>(val);
    return *this;
  }
};

class Stream {
 public:
  template <typename T>
  StreamDelegate operator<<(T&& val) {
    std::cerr << std::forward<T>(val);
    return StreamDelegate();
  }
};

static Stream cerr_stream;

#define LOG(severity) cerr_stream

#if defined(POLARIS_DEBUG)
#define VLOG(severity) cerr_stream
#else
static std::ostream null_stream(0);
#define VLOG(severity) null_stream
#endif // defined(POLARIS_DEBUG)
#else
#include <glog/logging.h>
#endif // defined(POLARIS_NO_GLOG)

using namespace point_one::polaris;

/******************************************************************************/
PolarisClient::PolarisClient(int max_reconnect_attempts)
    : PolarisClient("", "", max_reconnect_attempts) {}

/******************************************************************************/
PolarisClient::PolarisClient(const std::string& api_key,
                             const std::string& unique_id,
                             int max_reconnect_attempts)
    : max_reconnect_attempts_(max_reconnect_attempts),
      api_key_(api_key),
      unique_id_(unique_id) {
  polaris_.SetRTCMCallback([&](const uint8_t* buffer, size_t size_bytes) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    if (callback_) {
      callback_(buffer, size_bytes);
    }
  });
}

/******************************************************************************/
PolarisClient::~PolarisClient() {
  Disconnect();
}

/******************************************************************************/
void PolarisClient::SetAPIKey(const std::string& api_key,
                              const std::string& unique_id) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  api_key_ = api_key;
  unique_id_ = unique_id;
}

/******************************************************************************/
void PolarisClient::SetAuthToken(const std::string& auth_token) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  api_key_ = "";
  unique_id_ = "";
  auth_valid_ = true;
  polaris_.SetAuthToken(auth_token);
}

/******************************************************************************/
void PolarisClient::SetPolarisEndpoint(const std::string& endpoint_url,
                                       int endpoint_port) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  endpoint_url_ = endpoint_url;
  endpoint_port_ = endpoint_port;
}

/******************************************************************************/
void PolarisClient::SetMaxReconnects(int max_reconnect_attempts) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  max_reconnect_attempts_ = max_reconnect_attempts;
}

/******************************************************************************/
void PolarisClient::SetRTCMCallback(
    std::function<void(const uint8_t* buffer, size_t size_bytes)> callback) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  callback_ = callback;
}

/******************************************************************************/
void PolarisClient::SendECEFPosition(double x_m, double y_m, double z_m) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  current_request_type_ = RequestType::ECEF;
  ecef_position_m_[0] = x_m;
  ecef_position_m_[1] = y_m;
  ecef_position_m_[2] = z_m;
  if (connected_) {
    polaris_.SendECEFPosition(x_m, y_m, z_m);
  }
}

/******************************************************************************/
void PolarisClient::SendLLAPosition(double latitude_deg, double longitude_deg,
                                 double altitude_m) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  current_request_type_ = RequestType::LLA;
  lla_position_deg_[0] = latitude_deg;
  lla_position_deg_[1] = longitude_deg;
  lla_position_deg_[2] = altitude_m;
  if (connected_) {
    polaris_.SendLLAPosition(latitude_deg, longitude_deg, altitude_m);
  }
}

/******************************************************************************/
void PolarisClient::RequestBeacon(const std::string& beacon_id) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  current_request_type_ = RequestType::BEACON;
  beacon_id_ = beacon_id;
  if (connected_) {
    polaris_.RequestBeacon(beacon_id.c_str());
  }
}

/******************************************************************************/
void PolarisClient::Run(double timeout_sec) {
  const int timeout_ms = std::lround(timeout_sec * 1e3);
  while (running_) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    // Retrieve an access token using the specified API key.
    if (!auth_valid_) {
      int ret = polaris_.Authenticate(api_key_, unique_id_);
      if (ret == POLARIS_FORBIDDEN) {
        LOG(ERROR) << "Authentication rejected. Is your API key valid?";
        running_ = false;
        break;
      } else if (ret != POLARIS_SUCCESS) {
        LOG(WARNING) << "Authentication failed. Retrying.";
        continue;
      } else {
        auth_valid_ = true;
      }
    }

    // We now have a valid access token. Connect to the corrections service.
    //
    // In the calls below, if the connection times out or the access token is
    // rejected, try to connect again. If it fails too many times, the access
    // token may be expired - try reauthenticating.
    VLOG(1) << "Authenticated. Connecting to Polaris...";

    if (polaris_.ConnectTo(endpoint_url_, endpoint_port_) != POLARIS_SUCCESS) {
      LOG(ERROR) << "Error connecting to Polaris corrections stream. Retrying.";
      IncrementRetryCount();
      continue;
    }

    VLOG(1) << "Connected to Polaris...";

    // If there's an outstanding position update/beacon request resend it on
    // reconnect. Requests are cleared on a user-requested disconnect, so this
    // is a no-op on the first connection attempt.
    if (ResendRequest() != POLARIS_SUCCESS) {
      polaris_.Disconnect();
      IncrementRetryCount();
      continue;
    }

    // Now release the mutex and start processing data.
    lock.unlock();
    int ret = polaris_.Run(timeout_ms);
    lock.lock();

    if (ret == POLARIS_SUCCESS) {
      // Connection closed by a call to PolarisInterface::Disconnect().
      continue;
    } else if (ret == POLARIS_CONNECTION_CLOSED) {
      LOG(WARNING) << "Connection terminated remotely. Reconnecting.";
    } else if (ret == POLARIS_TIMED_OUT) {
      LOG(WARNING) << "Connection timed out. Reconnecting.";
    } else {
      LOG(ERROR) << "Unexpected error (" << ret << "). Reconnecting.";
    }

    // Connection closed due to an error. Reconnect.
    IncrementRetryCount();
  }

  // Finished running - clear any pending send requests for next time.
  current_request_type_ = RequestType::NONE;
  connect_count_ = 0;
}

/******************************************************************************/
void PolarisClient::RunAsync(double timeout_sec) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  run_thread_.reset(
      new std::thread(std::bind(&PolarisClient::Run, this, timeout_sec)));
}

/******************************************************************************/
void PolarisClient::Disconnect() {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  running_ = false;
  polaris_.Disconnect();
  if (run_thread_) {
    run_thread_->join();
    run_thread_.reset(nullptr);
  }
}

/******************************************************************************/
void PolarisClient::IncrementRetryCount() {
  // If we've hit the max reconnect limit, clear the auth token and try to
  // re-authenticate.
  //
  // If the user manually provided an auth token instead of an API key, however,
  // we can't perform authentication so we'll just keep retrying.
  if (!api_key_.empty() && max_reconnect_attempts_ > 0 &&
      ++connect_count_ > max_reconnect_attempts_) {
    LOG(WARNING) << "Max reconnects exceeded. Clearing access token and "
                    "retrying authentication.";
    auth_valid_ = false;
    connect_count_ = 0;
  }
}

/******************************************************************************/
int PolarisClient::ResendRequest() {
  if (current_request_type_ == RequestType::ECEF) {
    return polaris_.SendECEFPosition(ecef_position_m_[0], ecef_position_m_[1],
                                     ecef_position_m_[2]);
  } else if (current_request_type_ == RequestType::LLA) {
    return polaris_.SendLLAPosition(lla_position_deg_[0], lla_position_deg_[1],
                                    lla_position_deg_[2]);
  } else if (current_request_type_ == RequestType::BEACON) {
    return polaris_.RequestBeacon(beacon_id_.c_str());
  } else {
    return POLARIS_SUCCESS;
  }
}
