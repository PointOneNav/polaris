/**************************************************************************/ /**
 * @brief Polaris C++ client.
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

#include "point_one/polaris/polaris_interface.h"

namespace point_one {
namespace polaris {

/**
 * @brief Polaris C++ client class.
 *
 * This class is responsible for interacting with the Polaris service and
 * dispatching incoming corrections data. All connection logic including
 * authentication and reconnection on errors is handled internally.
 *
 * Example usage:
 * ```cpp
 *  // Data handling function.
 *  void OnData(const uint8_t* data, size_t length) { ... }
 *
 *  // Connect to Polaris.
 *  PolarisClient client("my-api-key", "my-unique-id");
 *  client.SetRTCMCallback(OnData);
 *  client.RunAsync();
 *
 *  // Periodically send position updates as the receiver moves.
 *  //
 *  // Note that you must send an initial position before you will receive any
 *  // corrections.
 *  client.SendECEFPosition(...);
 *
 *  // Disconnect when finished.
 *  client.Disconnect();
 * ```
 */
class PolarisClient {
 public:
  /**
   * @brief Create a new instance.
   *
   * @note
   * This class will not connect to the Polaris corrections service until @ref
   * Run() is called.
   *
   * @param max_reconnect_attempts The maximum number times to attempt a
   *        reconnection before reauthenticating.
   */
  explicit PolarisClient(int max_reconnect_attempts = 2);

  /**
   * @brief Create a new instance.
   *
   * @note
   * This class will not connect to the Polaris corrections service until @ref
   * Run() is called.
   *
   * @param api_key The Polaris API key to be used.
   * @param unique_id An optional unique ID used to represent this individual
   *        instance, or or empty string if unspecified. See @ref
   *        polaris_cpp_unique_id for details and requirements.
   * @param max_reconnect_attempts The maximum number times to attempt a
   *        reconnection before reauthenticating.
   */
  PolarisClient(const std::string& api_key, const std::string& unique_id,
                int max_reconnect_attempts = 2);

  /**
   * @brief Disconnect from the Polaris service and destroy this instance.
   */
  virtual ~PolarisClient();

  /**
   * @brief Specify the API key and unique ID to use when authenticating with
   *        the Polaris corrections service.
   *
   * Ignored if an existing authentication token is provided to @ref
   * SetAuthToken().
   *
   * See also @ref PolarisInterface::Authenticate().
   *
   * @param api_key The Polaris API key to be used.
   * @param unique_id An optional unique ID used to represent this individual
   *        instance, or or empty string if unspecified. See @ref
   *        polaris_cpp_unique_id for details and requirements.
   */
  void SetAPIKey(const std::string& api_key, const std::string& unique_id);

  /**
   * @brief Specify a known authentication token to use when connecting to the
   *        Polaris corrections service.
   *
   * See also @ref PolarisInterface::SetAuthToken().
   *
   * @param auth_token The desired authentication token.
   */
  void SetAuthToken(const std::string& auth_token);

  /**
   * @brief Specify the unique ID to use when connecting to the Polaris
   *        corrections service without authentication.
   *
   * This function is intended to be used for custom edge connections where a
   * secure connection to Polaris is already established by other means. See
   * @ref PolarisInterface::ConnectWithoutAuth() for details.
   *
   * @param unique_id An optional unique ID used to represent this individual
   *        instance, or or empty string if unspecified. See @ref
   *        polaris_cpp_unique_id for details and requirements.
   */
  void SetNoAuthID(const std::string& unique_id);

  /**
   * @brief Specify an alternate URL to use when connecting to the Polaris
   *        corrections endpoint.
   *
   * @param endpoint_url The desired endpoint URL.
   * @param endpoint_port The desired endpoint port.
   */
  void SetPolarisEndpoint(const std::string& endpoint_url = "",
                          int endpoint_port = 0);

  /**
   * @brief Set the maximum number of times to attempt a reconnection before
   *        reauthenticating.
   *
   * During normal operation, if the connection to Polaris is lost, @ref Run()
   * will reconnect automatically using the existing authentication token. If
   * the connection attempt fails too many times, the authentication token will
   * be cleared and @ref Run() will reauthenticate with the corrections service.
   *
   * @param max_reconnect_attempts The maximum number of retries.
   */
  void SetMaxReconnects(int max_reconnect_attempts);

  /**
   * @brief Specify a function to be called when incoming RTCM data is received.
   *
   * @param callback A callback function taking a pointer to the data buffer and
   *        the data size (in bytes).
   */
  void SetRTCMCallback(
      std::function<void(const uint8_t* buffer, size_t size_bytes)> callback);

  /**
   * @brief Send a position update to the corrections service.
   *
   * @note
   * You must send a position at least once to associate with a corrections
   * stream before Polaris will return any corrections data.
   *
   * See also @ref Polaris_SendECEFPosition().
   *
   * @param x_m The receiver ECEF X position (in meters).
   * @param y_m The receiver ECEF Y position (in meters).
   * @param z_m The receiver ECEF Z position (in meters).
   */
  void SendECEFPosition(double x_m, double y_m, double z_m);

  /**
   * @brief Send a position update to the corrections service.
   *
   * @note
   * You must send a position at least once to associate with a corrections
   * stream before Polaris will return any corrections data.
   *
   * See also @ref PolarisInterface::SendLLAPosition().
   *
   * @param latitude_deg The receiver WGS-84 latitude (in degrees).
   * @param longitude_deg The receiver WGS-84 longitude (in degrees).
   * @param altitude_m The receiver WGS-84 altitude (in meters).
   */
  void SendLLAPosition(double latitude_deg, double longitude_deg,
                       double altitude_m);

  /**
   * @brief Request corrections for a specific base station.
   *
   * If desired, override the corrections stream assigned based on specified
   * receiver position and instead send corrections from the requested beacon.
   *
   * See also @ref PolarisInterface::RequestBeacon().
   *
   * @param beacon_id The desired beacon ID.
   */
  void RequestBeacon(const std::string& beacon_id);

  /**
   * @brief Connect to Polaris and receive data.
   *
   * This function will block indefinitely, receiving incoming data and calling
   * the callback function provided to @ref SetRTCMCallback(), until @ref
   * Disconnect() is called. If the incoming data stream stops due to a bad
   * connection or other reasons, this function will reconnect automatically.
   *
   * If no data is received after a maximum number of reconnection attempts
   * specified by @ref SetMaxReconnects(), the authentication token will be
   * cleared automatically and this function will reauthenticate using the
   * provided API key. If an explicit authentication token was specified to @ref
   * SetAuthToken() and no API key is present, this function will continue to
   * attempt reconnection and will not reauthenticate.
   *
   * @note
   * This function will also return if authentication fails because of a
   * rejected Polaris API key.
   *
   * @param timeout_sec The maximum amount of time to wait for incoming
   *        corrections data before attempting to reconnect.
   */
  void Run(double timeout_sec = 30.0);

  /**
   * @brief Connect to Polaris and receive data in a separate thread.
   *
   * This function starts a separate thread to run the @ref Run() function, and
   * then returns immediately. See @ref Run() for more details.
   *
   * @param timeout_sec The maximum amount of time to wait for incoming
   *        corrections data before attempting to reconnect.
   */
  void RunAsync(double timeout_sec = 30.0);

  /**
   * @brief Disconnect from the Polaris service and return from @ref Run().
   *
   * If running asynchronously (@ref RunAsync()), this function will block until
   * the underlying run thread has been joined.
   */
  void Disconnect();

 private:
  enum class RequestType { NONE, ECEF, LLA, BEACON };

  std::recursive_mutex mutex_;
  PolarisInterface polaris_;
  std::atomic<bool> running_;
  std::unique_ptr<std::thread> run_thread_;

  std::function<void(const uint8_t* buffer, size_t size_bytes)> callback_;

  std::string endpoint_url_;
  int endpoint_port_ = 0;

  bool auth_valid_ = false;
  bool no_auth_ = false;
  bool connected_ = false;
  int max_reconnect_attempts_ = -1;
  int connect_count_ = 0;

  std::string api_key_;
  std::string unique_id_;

  RequestType current_request_type_ = RequestType::NONE;
  double ecef_position_m_[3] = {NAN, NAN, NAN};
  double lla_position_deg_[3] = {NAN, NAN, NAN};
  std::string beacon_id_;

  /**
   * @brief Increment the reconnect attempt count and clear the current
   *        authentication if max reconnects is exceeded.
   */
  void IncrementRetryCount();

  /**
   * @brief Resend the current position/beacon request on reconnect.
   *
   * @return @ref POLARIS_SUCCESS on success or <0 on error.
   */
  int ResendRequest();
};

} // namespace polaris
} // namespace point_one
