/**************************************************************************/ /**
 * @brief Polaris client C++ wrapper base class.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <atomic>
#include <cmath> // For NAN
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include <point_one/polaris/polaris.h>

#include "point_one/polaris/polarispp_interface.h"

namespace point_one {
namespace polaris {

class PolarisClient {
 public:
  explicit PolarisClient(int max_reconnect_attempts = 2);

  PolarisClient(const std::string& api_key, const std::string& unique_id,
                int max_reconnect_attempts = 2);

  virtual ~PolarisClient();

  void SetAPIKey(const std::string& api_key, const std::string& unique_id);

  void SetAuthToken(const std::string& auth_token);

  void SetPolarisEndpoint(const std::string& endpoint_url, int endpoint_port);

  void SetMaxReconnects(int max_reconnect_attempts);

  void SetRTCMCallback(
      std::function<void(const uint8_t* buffer, size_t size_bytes)> callback);

  void SendECEFPosition(double x_m, double y_m, double z_m);

  void SendLLAPosition(double latitude_deg, double longitude_deg,
                       double altitude_m);

  void RequestBeacon(const std::string& beacon_id);

  void Run(double timeout_sec = 15.0);

  void RunAsync(double timeout_sec = 15.0);

  void Disconnect();

 private:
  enum class RequestType { NONE, ECEF, LLA, BEACON };

  std::recursive_mutex mutex_;
  PolarisInterface polaris_;
  std::atomic<bool> running_;
  std::unique_ptr<std::thread> run_thread_;

  std::function<void(const uint8_t* buffer, size_t size_bytes)> callback_;

  std::string endpoint_url_ = POLARIS_ENDPOINT_URL;
  int endpoint_port_ = POLARIS_ENDPOINT_PORT;

  bool auth_valid_ = false;
  bool connected_ = false;
  int max_reconnect_attempts_ = -1;
  int connect_count_ = 0;

  std::string api_key_;
  std::string unique_id_;

  RequestType current_request_type_ = RequestType::NONE;
  double ecef_position_m_[3] = {NAN, NAN, NAN};
  double lla_position_deg_[3] = {NAN, NAN, NAN};
  std::string beacon_id_;

  void IncrementRetryCount();

  int ResendRequest();
};

} // namespace polaris
} // namespace point_one
