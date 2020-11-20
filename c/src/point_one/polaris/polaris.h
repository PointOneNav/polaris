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

// Default buffer size can store up to one complete, maximum sized RTCM message
// (6 bytes header/CRC + 1023 bytes payload). We don't align to RTCM framing,
// however, so there's no guarantee that the buffer starts at the beginning of
// an RTCM message or contains either a complete message or only one message.
#ifndef POLARIS_BUFFER_SIZE
# define POLARIS_BUFFER_SIZE 1029
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
  uint8_t buffer[POLARIS_BUFFER_SIZE];

  PolarisCallback_t rtcm_callback;
} PolarisContext_t;

int Polaris_Init(PolarisContext_t* context);

int Polaris_Authenticate(PolarisContext_t* context, const char* api_key,
                         const char* unique_id);

int Polaris_SetAuthToken(PolarisContext_t* context, const char* auth_token);

int Polaris_Connect(PolarisContext_t* context);

int Polaris_ConnectTo(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port);

void Polaris_Disconnect(PolarisContext_t* context);

void Polaris_SetRTCMCallback(PolarisContext_t* context,
                             PolarisCallback_t callback);

int Polaris_SendECEFPosition(PolarisContext_t* context, double x_m, double y_m,
                             double z_m);

int Polaris_SendLLAPosition(PolarisContext_t* context, double latitude_deg,
                            double longitude_deg, double altitude_m);

int Polaris_RequestBeacon(PolarisContext_t* context, const char* beacon_id);

void Polaris_Run(PolarisContext_t* context);
