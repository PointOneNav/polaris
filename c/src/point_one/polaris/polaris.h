/**************************************************************************/ /**
 * @brief Point One Navigation Polaris client support.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <stdint.h>

#include "point_one/polaris/socket.h"

#define POLARIS_API_URL "api.pointonenav.com"

#define POLARIS_ENDPOINT_URL "polaris.pointonenav.com"
#define POLARIS_ENDPOINT_PORT 8088
#define POLARIS_ENDPOINT_TLS_PORT 8090

#define POLARIS_MAX_UNIQUE_ID_SIZE 36

#ifndef POLARIS_MAX_TOKEN_SIZE
# define POLARIS_MAX_TOKEN_SIZE 512
#endif

/**
 * @brief The size of the data receive buffer (in bytes).
 *
 * @note
 * The receive buffer must be large enough to store the entire HTTP
 * authentication response.
 *
 * The default buffer size can store up to one complete, maximum sized RTCM
 * message (6 bytes header/CRC + 1023 bytes payload). We don't align to RTCM
 * framing, however, so there's no guarantee that the buffer starts at the
 * beginning of an RTCM message or contains either a complete message or only
 * one message.
 */
#ifndef POLARIS_RECV_BUFFER_SIZE
# define POLARIS_RECV_BUFFER_SIZE 1029
#endif

/**
 * @brief The size of the data send buffer (in bytes).
 *
 * This buffer is used to send position updates and other control messages to
 * Polaris.
 *
 * @note
 * The send buffer is _not_ used to send the authentication token on connect
 * since it is typically too small. Instead, the receive buffer is used for the
 * entire authentication process. This is considered safe since data will not
 * be received until the client is authenticated.
 */
#ifndef POLARIS_SEND_BUFFER_SIZE
# define POLARIS_SEND_BUFFER_SIZE 64
#endif

/**
 * @brief The maximum amount of time (in ms) to wait for incoming data for a
 *        single receive call (@ref Polaris_Work()).
 */
#ifndef POLARIS_RECV_TIMEOUT_MS
# define POLARIS_RECV_TIMEOUT_MS 5000
#endif

/**
 * @brief The maximum amount of time (in ms) to wait when sending a message to
 *        Polaris.
 */
#ifndef POLARIS_SEND_TIMEOUT_MS
# define POLARIS_SEND_TIMEOUT_MS 1000
#endif

/**
 * @name Polaris Return Codes
 * @{
 */
#define POLARIS_SUCCESS 0
#define POLARIS_ERROR -1
#define POLARIS_NOT_ENOUGH_SPACE -2
#define POLARIS_SOCKET_ERROR -3
#define POLARIS_SEND_ERROR -4
#define POLARIS_AUTH_ERROR -5
#define POLARIS_FORBIDDEN -6
#define POLARIS_CONNECTION_CLOSED -7
#define POLARIS_TIMED_OUT -8
/** @} */

/**
 * @name Polaris Logging Verbosity Levels
 * @{
 */
#define POLARIS_LOG_LEVEL_INFO 0
#define POLARIS_LOG_LEVEL_DEBUG 1
#define POLARIS_LOG_LEVEL_TRACE 2
/** @} */

struct PolarisContext_s;
typedef struct PolarisContext_s PolarisContext_t;

typedef void (*PolarisCallback_t)(void* info, PolarisContext_t* context,
                                  const uint8_t* buffer, size_t size_bytes);

struct PolarisContext_s {
  P1_Socket_t socket;

  char auth_token[POLARIS_MAX_TOKEN_SIZE + 1];
  uint8_t authenticated;
  uint8_t disconnected;
  size_t total_bytes_received;
  uint8_t data_request_sent;

  // Note: Enforcing 4-byte alignment of the buffers for platforms that require
  // aligned 2- or 4-byte access.
  uint8_t recv_buffer[POLARIS_RECV_BUFFER_SIZE] __attribute__((aligned (4)));
  uint8_t send_buffer[POLARIS_SEND_BUFFER_SIZE] __attribute__((aligned (4)));

  PolarisCallback_t rtcm_callback;
  void* rtcm_callback_info;

  // Note: We're using void* to avoid needing the inclusion of SSL libs in the
  // header file.
  void* ssl_ctx;
  void* ssl;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize a Polaris context.
 *
 * @param context The Polaris context to be used.
 *
 * @return @ref POLARIS_SUCCESS on success.
 */
int Polaris_Init(PolarisContext_t* context);

/**
 * @brief Free memory and data structures used by a Polaris context.
 *
 * @note
 * This function releases any existing TLS or socket connections. You must call
 * this function before calling @ref Polaris_Init() a second time on an existing
 * context.
 *
 * @param context The Polaris context to be freed.
 */
void Polaris_Free(PolarisContext_t* context);

/**
 * @brief Set the Polaris library print verbosity level.
 *
 * @param log_level The desired verbosity level.
 */
void Polaris_SetLogLevel(int log_level);

/**
 * @brief Authenticate with Polaris.
 *
 * Authentication uses the provided API key to generate an authentication token
 * that can be used to receive corrections.
 *
 * @note
 * To enable secure connections using TLS, the library must be compiled with
 * `POLARIS_USE_TLS` defined.
 *
 * @post
 * On success, `context.auth_token` will be populated with the generated token.
 *
 * @section polaris_unique_id Connection Unique IDs
 * Polaris uses ID strings to uniquely identify client connections made using a
 * particular API key. Unique IDs allow diagnostic information to be associated
 * with a specific incoming connection when analyzing connectivity or other
 * issues.
 *
 * Unique IDs must be a maximum of 36 characters, and may include only letters,
 * numbers, hyphens, and underscores (`^[\w\d-]+$`).
 *
 * @warning
 * When specified, unique IDs must be unique across _all_ Polaris connections
 * for a given API key. If two Polaris clients connect at the same time using
 * the same API key and ID, they will conflict with each other and will not work
 * correctly.
 *
 * Unique IDs are optional, but strongly encouraged for development purposes. If
 * omitted, (empty string or `NULL`), it will not be possible to associate
 * diagnostic information with an individual client.
 *
 * @param context The Polaris context to be used.
 * @param api_key The Polaris API key to be used.
 * @param unique_id An optional unique ID used to represent this individual
 *        instance, or `NULL` or empty string if unspecified. See @ref
 *        polaris_unique_id for details and requirements.
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_ERROR if the inputs are invalid.
 * @return @ref POLARIS_NOT_ENOUGH_SPACE if there is not enough storage to store
 *         the authentication request or response.
 * @return @ref POLARIS_FORBIDDEN if the API key was rejected.
 * @return @ref POLARIS_AUTH_ERROR if the authentication failed.
 * @return @ref POLARIS_SOCKET_ERROR if a connection could not be established
 *         with the Polaris authentication server.
 * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
 */
int Polaris_Authenticate(PolarisContext_t* context, const char* api_key,
                         const char* unique_id);

/**
 * @brief Authenticate with the specified Polaris API server.
 *
 * @copydetails Polaris_Authenticate()
 *
 * @param api_url The URL of the desired API authentication server.
 */
int Polaris_AuthenticateTo(PolarisContext_t* context, const char* api_key,
                           const char* unique_id, const char* api_url);

/**
 * @brief Use an existing authentication token to connect to Polaris.
 *
 * @param context The Polaris context to be used.
 * @param auth_token The desired authentication token.
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_ERROR if the auth token is not valid.
 * @return @ref POLARIS_NOT_ENOUGH_SPACE if the token does not fit in the
 *         allocated storage.
 */
int Polaris_SetAuthToken(PolarisContext_t* context, const char* auth_token);

/**
 * @brief Connect to the corrections service using the stored authentication
 *        token.
 *
 * @note
 * This function can not currently check for rejection of the authentication
 * token by the corrections service. Instead, authentication failure is detected
 * on the first call to @ref Polaris_Work().
 *
 * @param context The Polaris context to be used.
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_SOCKET_ERROR if a connection could not be established
 *         with the Polaris corrections server.
 * @return @ref POLARIS_AUTH_ERROR if an authentication token was not provided.
 * @return @ref POLARIS_SEND_ERROR if an error occurred while sending the
 *         authentication token.
 */
int Polaris_Connect(PolarisContext_t* context);

/**
 * @brief Connect to the specified corrections service URL.
 *
 * @copydetails Polaris_Connect()
 *
 * @param endpoint_url The desired endpoint URL.
 * @param endpoint_port The desired endpoint port.
 */
int Polaris_ConnectTo(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port);

/**
 * @brief Connect to the corrections service without providing an authentication
 *        token.
 *
 * This function is intended to be used for custom edge connections where a
 * secure connection to Polaris is already established by other means.
 *
 * @param context The Polaris context to be used.
 * @param endpoint_url The desired endpoint URL.
 * @param endpoint_port The desired endpoint port.
 * @param unique_id An optional unique ID used to represent this individual
 *        instance, or `NULL` or empty string if unspecified. See @ref
 *        polaris_unique_id for details and requirements.
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_ERROR if the unique ID is not valid.
 * @return @ref POLARIS_SOCKET_ERROR if a connection could not be established
 *         with the Polaris corrections server.
 * @return @ref POLARIS_SEND_ERROR if an error occurred while sending the
 *         unique ID.
 */
int Polaris_ConnectWithoutAuth(PolarisContext_t* context,
                               const char* endpoint_url, int endpoint_port,
                               const char* unique_id);

/**
 * @brief Disconnect from the corrections stream.
 *
 * @note
 * This function may be called asynchronously from @ref Polaris_Work() or @ref
 * Polaris_Run(), and therefore does _not_ free the existing socket or TLS
 * connection. You must call @ref Polaris_Free() to do so if neither @ref
 * Polaris_Work() or @ref Polaris_Run() is executing when this function is
 * called.
 *
 * @param context The Polaris context to be disconnected.
 */
void Polaris_Disconnect(PolarisContext_t* context);

/**
 * @brief Specify a function to be called when RTCM corrections data is
 *        received.
 *
 * @param context The Polaris context to be used.
 * @param callback The function to be called.
 * @param callback_info An arbitrary pointer that will be passed to the callback
 *        function when it is called.
 */
void Polaris_SetRTCMCallback(PolarisContext_t* context,
                             PolarisCallback_t callback,
                             void* callback_info);

/**
 * @brief Send a position update to the corrections service.
 *
 * @note
 * You must send a position at least once to associate with a corrections
 * stream before Polaris will return any corrections data.
 *
 * @param context The Polaris context to be used.
 * @param x_m The receiver ECEF X position (in meters).
 * @param y_m The receiver ECEF Y position (in meters).
 * @param z_m The receiver ECEF Z position (in meters).
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
 * @return @ref POLARIS_SOCKET_ERROR if socket is not currently open.
 */
int Polaris_SendECEFPosition(PolarisContext_t* context, double x_m, double y_m,
                             double z_m);

/**
 * @brief Send a position update to the corrections service.
 *
 * @note
 * You must send a position at least once to associate with a corrections
 * stream before Polaris will return any corrections data.
 *
 * @param context The Polaris context to be used.
 * @param latitude_deg The receiver WGS-84 latitude (in degrees).
 * @param longitude_deg The receiver WGS-84 longitude (in degrees).
 * @param altitude_m The receiver WGS-84 altitude (in meters).
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
 * @return @ref POLARIS_SOCKET_ERROR if socket is not currently open.
 */
int Polaris_SendLLAPosition(PolarisContext_t* context, double latitude_deg,
                            double longitude_deg, double altitude_m);

/**
 * @brief Request corrections for a specific base station.
 *
 * If desired, override the corrections stream assigned based on specified
 * receiver position and instead send corrections from the requested beacon.
 *
 * @param context The Polaris context to be used.
 * @param beacon_id The desired beacon ID.
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_SEND_ERROR if the request could not be sent.
 * @return @ref POLARIS_SOCKET_ERROR if socket is not currently open.
 */
int Polaris_RequestBeacon(PolarisContext_t* context, const char* beacon_id);

/**
 * @brief Receive and dispatch the next block of incoming data.
 *
 * This function blocks until some data is received, or until @ref
 * POLARIS_RECV_TIMEOUT_MS elapses. If @ref Polaris_Disconnect() is called, this
 * function will return immediately.
 *
 * If an error occurs and this function returns <0 (with the exception of @ref
 * POLARIS_TIMED_OUT -- see below), the socket will be closed before the
 * function returns.
 *
 * @warning
 * Important: A read timeout is a normal occurrence and is not considered an
 * error condition. Read timeouts can happen occasionally due to intermittent
 * internet connections (e.g., client vehicle losing cell coverage briefly).
 * Most GNSS receivers can tolerate small gaps in corrections data. The socket
 * will _not_ be closed on a read timeout (@ref POLARIS_TIMED_OUT). The calling
 * code must call `Polaris_Disconnect()` to close the socket before attempting
 * to reconnect or reauthenticate.
 *
 * @note
 * There is no guarantee that a data block contains a complete RTCM message, or
 * starts on an RTCM message boundary.
 *
 * @post
 * The received data will be stored in `context->buffer` on return. If a
 * callback function is registered (@ref Polaris_SetRTCMCallback()), it will be
 * called the the received data before this function returns.
 *
 * @param context The Polaris context to be used.
 *
 * @return The number of received bytes.
 * @return @ref POLARIS_CONNECTION_CLOSED if the connection was closed remotely
 *         or by calling @ref Polaris_Disconnect().
 * @return @ref POLARIS_TIMED_OUT if the socket receive timeout elapsed.
 * @return @ref POLARIS_FORBIDDEN if the connection is closed after a position
 *         or beacon request is sent but before any data is received, indicating
 *         a possible authentication failure (invalid or expired access token).
 * @return @ref POLARIS_SOCKET_ERROR if the socket is not currently open.
 */
int Polaris_Work(PolarisContext_t* context);

/**
 * @brief Receive and dispatch incoming data.
 *
 * This function blocks indefinitely and sends all incoming corrections data to
 * the registered callback function. The function will return automatically when
 * @ref Polaris_Disconnect() is called.
 *
 * If the specified timeout has elapsed since the last time data was received,
 * the connection will be considered lost and the function will return.
 *
 * @post
 * Unlike @ref Polaris_Work(), a value of @ref POLARIS_TIMED_OUT here indicates
 * the connection has been lost, and the socket will be closed on return.
 *
 * @param context The Polaris context to be used.
 * @param connection_timeout_ms The maximum elapsed time (in ms) between reads,
 *        after which the connection is considered lost.
 *
 * @return @ref POLARIS_SUCCESS if the connection was closed by a call to @ref
 *         Polaris_Disconnect(). Note that this differs from @ref
 *         Polaris_Work(), which returns @ref POLARIS_CONNECTION_CLOSED for
 *         both remote and local connection termination.
 * @return @ref POLARIS_CONNECTION_CLOSED if the connection was closed remotely.
 * @return @ref POLARIS_TIMED_OUT if no data was received for the specified
 *         timeout.
 * @return @ref POLARIS_FORBIDDEN if the connection is closed after a position
 *         or beacon request is sent but before any data is received, indicating
 *         a possible authentication failure (invalid or expired access token).
 * @return @ref POLARIS_SOCKET_ERROR if the socket is not currently open.
 */
int Polaris_Run(PolarisContext_t* context, int connection_timeout_ms);

#ifdef __cplusplus
} // extern "C"
#endif
