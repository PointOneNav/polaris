/**************************************************************************/ /**
 * @brief Polaris client C++ wrapper class.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <functional>
#include <string>

#include <point_one/polaris/polaris.h>

namespace point_one {
namespace polaris {

extern const std::string DEFAULT_ENDPOINT_URL;
static constexpr int DEFAULT_ENDPOINT_PORT =
#ifdef POLARIS_USE_TLS
  POLARIS_ENDPOINT_TLS_PORT;
#else
  POLARIS_ENDPOINT_PORT;
#endif

/**
 * @brief C++ wrapper class for the Polaris Client C library.
 */
class PolarisInterface {
 public:
  /**
   * @brief Construct a new client instance.
   */
  PolarisInterface();

  /**
   * @brief Disconnect and destroy this instance.
   */
  ~PolarisInterface();

  /**
   * @brief Authenticate with Polaris.
   *
   * Authentication uses the provided API key to generate an authentication
   * token that can be used to receive corrections.
   *
   * @warning
   * `unique_id` must be unique across _all_ Polaris connections for the
   * specified API key. If two instances connect at the same time using the same
   * key and ID, they will conflict with each other and will not work correctly.
   *
   * See also @ref Polaris_Authenticate().
   *
   * @param api_key The Polaris API key to be used.
   * @param unique_id A unique ID used to represent this individual instance.
   *
   * @return @ref POLARIS_SUCCESS on success.
   * @return @ref POLARIS_NOT_ENOUGH_SPACE if there is not enough storage to
   *         store the authentication request or response.
   * @return @ref POLARIS_FORBIDDEN if the API key was rejected.
   * @return @ref POLARIS_AUTH_ERROR if the authentication failed.
   * @return @ref POLARIS_SOCKET_ERROR if a connection could not be established
   *         with the Polaris authentication server.
   * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
   */
  int Authenticate(const std::string& api_key, const std::string& unique_id);

  /**
   * @brief Use an existing authentication token to connect to Polaris.
   *
   * See also @ref Polaris_SetAuthToken().
   *
   * @param context The Polaris context to be used.
   * @param auth_token The desired authentication token.
   *
   * @return @ref POLARIS_SUCCESS on success.
   * @return @ref POLARIS_NOT_ENOUGH_SPACE if the token does not fit in the
   *         allocated storage.
   */
  int SetAuthToken(const std::string& auth_token);

  /**
   * @brief Connect to the corrections service using the stored authentication
   *        token.
   *
   * @note
   * This function can not currently check for rejection of the authentication
   * token by the corrections service. Instead, authentication failure is
   * detected on the first call to @ref Work().
   *
   * See also @ref Polaris_Connect().
   *
   * @return @ref POLARIS_SUCCESS on success.
   * @return @ref POLARIS_SOCKET_ERROR if a connection could not be established
   *         with the Polaris corrections server.
   * @return @ref POLARIS_AUTH_ERROR if an authentication token was not
   *         provided.
   */
  int Connect();

  /**
   * @brief Connect to the specified corrections service URL.
   *
   * @copydetails Connect()
   *
   * See also @ref Polaris_ConnectTo().
   *
   * @param endpoint_url The desired endpoint URL.
   * @param endpoint_port The desired endpoint port.
   */
  int ConnectTo(const std::string& endpoint_url, int endpoint_port);

  /**
   * @brief Disconnect from the corrections stream.
   *
   * See also @ref Polaris_Disconnect().
   */
  void Disconnect();

  /**
   * @brief Specify a function to be called when RTCM corrections data is
   *        received.
   *
   * See also @ref Polaris_SetRTCMCallback().
   *
   * @param callback The function to be called.
   */
  void SetRTCMCallback(
      std::function<void(const uint8_t* buffer, size_t size_bytes)> callback);

  /**
   * @brief Send a position update to the corrections service.
   *
   * See also @ref Polaris_SendECEFPosition().
   *
   * @param x_m The receiver ECEF X position (in meters).
   * @param y_m The receiver ECEF Y position (in meters).
   * @param z_m The receiver ECEF Z position (in meters).
   *
   * @return @ref POLARIS_SUCCESS on success.
   * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
   * @return @ref POLARIS_SOCKET_ERROR if socket is not currently open.
   */
  int SendECEFPosition(double x_m, double y_m, double z_m);

  /**
   * @brief Send a position update to the corrections service.
   *
   * See also @ref Polaris_SendLLAPosition().
   *
   * @param latitude_deg The receiver WGS-84 latitude (in degrees).
   * @param longitude_deg The receiver WGS-84 longitude (in degrees).
   * @param altitude_m The receiver WGS-84 altitude (in meters).
   *
   * @return @ref POLARIS_SUCCESS on success.
   * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
   * @return @ref POLARIS_SOCKET_ERROR if socket is not currently open.
   */
  int SendLLAPosition(double latitude_deg, double longitude_deg,
                     double altitude_m);

  /**
   * @brief Request corrections for a specific base station.
   *
   * If desired, override the corrections stream assigned based on specified
   * receiver position and instead send corrections from the requested beacon.
   *
   * See also @ref Polaris_RequestBeacon().
   *
   * @param beacon_id The desired beacon ID.
   *
   * @return @ref POLARIS_SUCCESS on success.
   * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
   * @return @ref POLARIS_SOCKET_ERROR if socket is not currently open.
   */
  int RequestBeacon(const std::string& beacon_id);

  /**
   * @brief Receive and dispatch the next block of incoming data.
   *
   * This function blocks until some data is received, or until @ref
   * POLARIS_RECV_TIMEOUT_MS elapses. If @ref Disconnect() is called, this
   * function will return immediately.
   *
   * @note
   * There is no guarantee that a data block contains a complete RTCM message,
   * or starts on an RTCM message boundary.
   *
   * @post
   * The received data will be stored in the data buffer on return (@ref
   *  GetRecvBuffer()). If a callback function is registered (@ref
   *  SetRTCMCallback()), it will be called he the received data before this
   *  function returns.
   *
   * See also @ref Polaris_Work().
   *
   * @return The number of received bytes, or 0 if the timeout elapsed or @ref
   *         Disconnect() was called without receiving data.
   * @return @ref POLARIS_CONNECTION_CLOSED if the connection was closed
   *         remotely.
   * @return @ref POLARIS_FORBIDDEN if the connection is closed before any data
   *         is received, indicating an authentication failure (invalid or
   *         expired access token).
   * @return @ref POLARIS_SOCKET_ERROR if the socket is not currently open.
   */
  int Work();

  /**
   * @brief Receive and dispatch incoming data.
   *
   * This function blocks indefinitely and sends all incoming corrections data
   * to the registered callback function. The function will return automatically
   * when @ref Disconnect() is called.
   *
   * If the specified timeout has elapsed since the last time data was received,
   * the connection will be considered lost and the function will return.
   *
   * See also @ref Polaris_Run().
   *
   * @param connection_timeout_ms The maximum elapsed time (in ms) between
   *        reads, after which the connection is considered lost.
   *
   * @return @ref POLARIS_SUCCESS if the connection was closed by a call to @ref
   *         Disconnect().
   * @return @ref POLARIS_CONNECTION_CLOSED if the connection was closed
   *         remotely.
   * @return @ref POLARIS_TIMED_OUT if no data was received for the specified
   *         timeout.
   * @return @ref POLARIS_AUTH_ERROR if the connection is closed before any data
   *         is received, indicating an authentication failure.
   * @return @ref POLARIS_SOCKET_ERROR if the socket is not currently open.
   */
  int Run(int connection_timeout_ms);

  /**
   * @brief Get a reference to the buffer where incoming data is stored when
   *        @ref Work() is called.
   *
   * @return A reference to the data buffer.
   */
  const uint8_t* GetRecvBuffer() const;

 protected:
  PolarisContext_t context_;

  std::function<void(const uint8_t* buffer, size_t size_bytes)> callback_;

  static void HandleRTCMData(void* ptr, PolarisContext_t* context,
                             const uint8_t* buffer, size_t size_bytes);
};

} // namespace polaris
} // namespace point_one
