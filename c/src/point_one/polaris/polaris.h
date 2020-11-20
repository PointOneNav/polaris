/**************************************************************************/ /**
 * @brief Point One Navigation Polaris client support.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <stdint.h>

#include "point_one/polaris/socket.h"

#define POLARIS_ENDPOINT_URL "polaris.pointonenav.com"
#define POLARIS_ENDPOINT_PORT 8088

#ifndef POLARIS_MAX_TOKEN_SIZE
# define POLARIS_MAX_TOKEN_SIZE 512
#endif

/**
 * @brief The size of the data receive buffer (in bytes).
 *
 * @note
 * The receive buffer must be large enough to store the entire HTTP
 * authentication reponse.
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

#define POLARIS_SUCCESS 0
#define POLARIS_ERROR -1
#define POLARIS_NOT_ENOUGH_SPACE -2
#define POLARIS_SOCKET_ERROR -3
#define POLARIS_SEND_ERROR -4
#define POLARIS_AUTH_ERROR -5
#define POLARIS_FORBIDDEN -6

typedef void (*PolarisCallback_t)(const uint8_t* buffer, size_t size_bytes);

typedef struct {
  P1_Socket_t socket;

  char auth_token[POLARIS_MAX_TOKEN_SIZE + 1];
  uint8_t recv_buffer[POLARIS_RECV_BUFFER_SIZE];
  uint8_t send_buffer[POLARIS_SEND_BUFFER_SIZE];

  PolarisCallback_t rtcm_callback;
} PolarisContext_t;

/**
 * @brief Initialize a Polaris context.
 *
 * @param context The Polaris context to be used.
 *
 * @return @ref POLARIS_SUCCESS on success.
 */
int Polaris_Init(PolarisContext_t* context);

/**
 * @brief Authenticate with Polaris.
 *
 * Authentication uses the provided API key to generate an authentication token
 * that can be used to receive corrections.
 *
 * @post
 * On success, `context.auth_token` will be populated with the generated token.
 *
 * @param context The Polaris context to be used.
 *
 * @return @ref POLARIS_SUCCESS on success.
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
 * @brief Use an existing authentication token to connect to Polaris.
 *
 * @param context The Polaris context to be used.
 * @param auth_token The desired authentication token.
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_NOT_ENOUGH_SPACE if the token does not fit in the
 *         allocated storage.
 */
int Polaris_SetAuthToken(PolarisContext_t* context, const char* auth_token);

/**
 * @brief Connect to the corrections service using the stored authentication
 *        token.
 *
 * @param context The Polaris context to be used.
 *
 * @return @ref POLARIS_SUCCESS on success.
 * @return @ref POLARIS_SOCKET_ERROR if a connection could not be established
 *         with the Polaris corrections server.
 * @return @ref POLARIS_AUTH_ERROR if an authentication token was not provided.
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
 * @brief Disconnect from the corrections stream.
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
 */
void Polaris_SetRTCMCallback(PolarisContext_t* context,
                             PolarisCallback_t callback);

/**
 * @brief Send a position update to the corrections service.
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
 * @brief Receive and dispatch incoming data.
 *
 * This function blocks indefinitely and sends all incoming corrections data to
 * the registered callback function. The function will return automatically when
 * @ref Polaris_Disconnect() is called.
 *
 * @param context The Polaris context to be used.
 */
void Polaris_Run(PolarisContext_t* context);
