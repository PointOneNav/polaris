/**************************************************************************/ /**
 * @brief Polaris C++ client.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/polaris_client.h"

#include <iomanip>

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
  // Note that the C library print level will not change if the VLOG level is
  // changed dynamically via SetVLOGLevel() at runtime.
  if (VLOG_IS_ON(1)) {
    Polaris_SetLogLevel(POLARIS_LOG_LEVEL_DEBUG);
  } else if (VLOG_IS_ON(2)) {
    Polaris_SetLogLevel(POLARIS_LOG_LEVEL_TRACE);
  }

  SetPolarisAuthenticationServer();
  SetPolarisEndpoint();

  polaris_.SetRTCMCallback([&](const uint8_t* buffer, size_t size_bytes) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);
    VLOG(2) << "Received " << size_bytes << " bytes.";
    bytes_received_ += size_bytes;

    // When we successfully reconnect and get data from the network, reset the
    // retry count. That way if we have N-1 connection issues earlier in the
    // day and then just 1 later, we don't end up reauthenticating immediately
    // thinking we failed N times.
    //
    // We wait until we get data since "no data received" is also treated as a
    // potential authentication issue. The network may send a single RTCM 1029
    // text message in response to an invalid request, including a bad
    // authentication. Since we are not parsing the incoming RTCM stream, we
    // wait until we have received more data than the max 1029 message size
    // before declaring the connection successful.
    if (bytes_received_ > 270) {
      connect_count_ = 0;
    }

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
  no_auth_ = false;
}

/******************************************************************************/
void PolarisClient::SetAuthToken(const std::string& auth_token) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  api_key_ = "";
  unique_id_ = "";
  no_auth_ = false;
  if (polaris_.SetAuthToken(auth_token) == POLARIS_SUCCESS) {
    auth_valid_ = true;
  } else {
    LOG(ERROR) << "Unable to set authentication token.";
  }
}

/******************************************************************************/
void PolarisClient::SetNoAuthID(const std::string& unique_id) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  api_key_ = "";
  unique_id_ = unique_id;
  no_auth_ = true;
}

/******************************************************************************/
void PolarisClient::SetPolarisAuthenticationServer(const std::string& api_url) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  if (api_url.empty()) {
    api_url_ = POLARIS_API_URL;
  }
  else {
    api_url_ = api_url;
  }
}

/******************************************************************************/
void PolarisClient::SetPolarisEndpoint(const std::string& endpoint_url,
                                       int endpoint_port) {
  std::unique_lock<std::recursive_mutex> lock(mutex_);
  if (endpoint_url.empty()) {
    endpoint_url_ = POLARIS_ENDPOINT_URL;
  } else {
    endpoint_url_ = endpoint_url;
  }

  if (endpoint_port == 0) {
    endpoint_port_ =
#ifdef POLARIS_USE_TLS
        POLARIS_ENDPOINT_TLS_PORT;
#else
        POLARIS_ENDPOINT_PORT;
#endif
  } else {
    endpoint_port_ = endpoint_port;
  }
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
  VLOG(1) << "Setting current ECEF position: [" << std::fixed
          << std::setprecision(2) << x_m << ", " << y_m << ", " << z_m << "]";
  {
    std::unique_lock<std::mutex> position_lock(position_mutex_);
    current_request_type_ = RequestType::ECEF;
    ecef_position_m_[0] = x_m;
    ecef_position_m_[1] = y_m;
    ecef_position_m_[2] = z_m;
  }

  // If mutex_ is locked, it's probably because PolarisClient::Run() is
  // reconnecting.  It will send the position once connected.
  std::unique_lock<std::recursive_mutex> lock(mutex_, std::defer_lock);
  if (lock.try_lock()) {
    if (connected_) {
      polaris_.SendECEFPosition(x_m, y_m, z_m);
    }
  }
}

/******************************************************************************/
void PolarisClient::SendLLAPosition(double latitude_deg, double longitude_deg,
                                    double altitude_m) {
  VLOG(1) << "Setting current LLA position: [" << std::fixed
          << std::setprecision(6) << latitude_deg << ", " << longitude_deg
          << ", " << std::setprecision(2) << altitude_m << "]";
  {
    std::unique_lock<std::mutex> position_lock(position_mutex_);
    current_request_type_ = RequestType::LLA;
    lla_position_deg_[0] = latitude_deg;
    lla_position_deg_[1] = longitude_deg;
    lla_position_deg_[2] = altitude_m;
  }

  // If mutex_ is locked, it's probably because PolarisClient::Run() is
  // reconnecting.  It will send the position once connected.
  std::unique_lock<std::recursive_mutex> lock(mutex_, std::defer_lock);
  if (lock.try_lock()) {
    if (connected_) {
      polaris_.SendLLAPosition(latitude_deg, longitude_deg, altitude_m);
    }
  }
}

/******************************************************************************/
void PolarisClient::RequestBeacon(const std::string& beacon_id) {
  VLOG(1) << "Requesting beacon '" << beacon_id << "'.";
  {
    std::unique_lock<std::mutex> position_lock(position_mutex_);
    current_request_type_ = RequestType::BEACON;
    beacon_id_ = beacon_id;
  }

  // If mutex_ is locked, it's probably because PolarisClient::Run() is
  // reconnecting.  It will send the position once connected.
  std::unique_lock<std::recursive_mutex> lock(mutex_, std::defer_lock);
  if (lock.try_lock()) {
    if (connected_) {
      polaris_.RequestBeacon(beacon_id.c_str());
    }
  }
}

/******************************************************************************/
void PolarisClient::Run(double timeout_sec) {
  const int timeout_ms = std::lround(timeout_sec * 1e3);
  int ret = POLARIS_SUCCESS;
  running_ = true;
  bool previous_connect_failed = false;
  while (running_) {
    // Pause briefly after a failed reconnect attempt.
    if (previous_connect_failed) {
      sleep(2);
      if (!running_) {
        break;
      }
    }
    previous_connect_failed = true;

    std::unique_lock<std::recursive_mutex> lock(mutex_);

    bytes_received_ = 0;

    // Retrieve an access token using the specified API key.
    if (!auth_valid_ && !no_auth_) {
      VLOG(1) << "Authenticating with Polaris service. [unique_id="
              << (unique_id_.empty() ? "<not specified>" : unique_id_)
              << ", api_url=" << api_url_ << "]";
      ret = polaris_.AuthenticateTo(api_key_, unique_id_, api_url_);
      if (ret == POLARIS_FORBIDDEN) {
        LOG(ERROR) << "Authentication rejected. Is your API key valid?";
        running_ = false;
        break;
      } else if (ret == POLARIS_ERROR) {
        LOG(ERROR) << "Invalid API key/unique ID specified.";
        running_ = false;
        break;
      } else if (ret != POLARIS_SUCCESS) {
        LOG(WARNING) << "Authentication failed. Retrying. [error=" << ret
                     << "]";
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
    VLOG(1) << "Authenticated. Connecting to Polaris... [" << endpoint_url_
            << ":" << endpoint_port_ << "]";

    if (no_auth_) {
      ret = polaris_.ConnectWithoutAuth(endpoint_url_, endpoint_port_,
                                        unique_id_);
    } else {
      ret = polaris_.ConnectTo(endpoint_url_, endpoint_port_);
    }

    if (ret != POLARIS_SUCCESS) {
      LOG(ERROR) << "Error connecting to Polaris corrections stream. Retrying.";
      if (ret != POLARIS_SOCKET_ERROR) {
        IncrementRetryCount();
      }
      continue;
    }

    VLOG(1) << "Connected to Polaris...";
    connected_ = true;

    // If there's an outstanding position update/beacon request resend it on
    // reconnect. Requests are cleared on a user-requested disconnect, so this
    // is a no-op on the first connection attempt.
    if (ResendRequest() != POLARIS_SUCCESS) {
      VLOG(1)
          << "Error resending position update/beacon request. Reconnecting.";
      connected_ = false;
      polaris_.Disconnect();
      if (ret != POLARIS_SOCKET_ERROR) {
        IncrementRetryCount();
      }
      continue;
    }
    previous_connect_failed = false;

    // Now release the mutex and start processing data.
    lock.unlock();
    int ret = polaris_.Run(timeout_ms);
    lock.lock();

    connected_ = false;

    if (ret == POLARIS_SUCCESS) {
      // Connection closed by a call to PolarisInterface::Disconnect().
      VLOG(1) << "Connection closed by user.";
      continue;
    } else if (ret == POLARIS_CONNECTION_CLOSED) {
      LOG(WARNING) << "Connection terminated remotely. Reconnecting.";
    } else if (ret == POLARIS_TIMED_OUT) {
      LOG(WARNING) << "Connection timed out. Reconnecting.";
    } else if (ret == POLARIS_FORBIDDEN) {
      LOG(WARNING) << "Connection timed out. Reconnecting.";
    } else if (ret == POLARIS_SOCKET_ERROR) {
      LOG(WARNING) << "Socket closed unexpectedly. Reconnecting.";
    } else {
      LOG(ERROR) << "Unexpected error. Reconnecting. [error=" << ret << "]";
    }

    // Connection closed due to an error. Reconnect.
    if (ret != POLARIS_SOCKET_ERROR) {
      IncrementRetryCount();
    }
  }

  // Finished running - clear any pending send requests for next time.
  current_request_type_ = RequestType::NONE;
  connect_count_ = 0;
  if (ret != POLARIS_SUCCESS) {
    LOG(WARNING) << "PolarisRun() exiting on fatal error. [error=" << ret
                 << "]";
  }
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
  VLOG(1) << "Disconnecting from Polaris...";
  running_ = false;
  connected_ = false;
  polaris_.Disconnect();
  lock.unlock();

  if (run_thread_) {
    VLOG(1) << "Joining run thread.";
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
  std::unique_lock<std::mutex> position_lock(position_mutex_);
  if (current_request_type_ == RequestType::ECEF) {
    VLOG(1) << "Resending ECEF position update. [" << std::fixed
            << std::setprecision(2) << ecef_position_m_[0] << ", "
            << ecef_position_m_[1] << ", " << ecef_position_m_[2] << "]";
    return polaris_.SendECEFPosition(ecef_position_m_[0], ecef_position_m_[1],
                                     ecef_position_m_[2]);
  } else if (current_request_type_ == RequestType::LLA) {
    VLOG(1) << "Resending LLA position update. [" << std::fixed
            << std::setprecision(7) << lla_position_deg_[0] << ", "
            << lla_position_deg_[1] << ", " << std::setprecision(2)
            << lla_position_deg_[2] << "]";
    return polaris_.SendLLAPosition(lla_position_deg_[0], lla_position_deg_[1],
                                    lla_position_deg_[2]);
  } else if (current_request_type_ == RequestType::BEACON) {
    VLOG(1) << "Resending beacon request. [id=" << beacon_id_ << "]";
    return polaris_.RequestBeacon(beacon_id_.c_str());
  } else {
    return POLARIS_SUCCESS;
  }
}
