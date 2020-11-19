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

#define POLARIS_AUTH_TOKEN_MAX_LENGTH 32

// Default buffer size can store up to one complete, maximum sized RTCM message
// (6 bytes header/CRC + 1023 bytes payload). We don't align to RTCM framing,
// however, so there's no guarantee that the buffer starts at the beginning of
// an RTCM message or contains either a complete message or only one message.
#define POLARIS_DEFAULT_BUFFER_SIZE 1029

#define POLARIS_SUCCESS 0
#define POLARIS_ERROR -1
#define POLARIS_NOT_ENOUGH_SPACE -2
#define POLARIS_SOCKET_ERROR -3
#define POLARIS_SEND_ERROR -4
#define POLARIS_AUTH_ERROR -5
#define POLARIS_FORBIDDEN -5

typedef void (*PolarisCallback_t)(const uint8_t* buffer, size_t size_bytes);

typedef struct {
  P1_Socket_t socket;

  char auth_token[POLARIS_AUTH_TOKEN_MAX_LENGTH + 1];

  uint8_t* buffer;
  size_t buffer_size;
  uint8_t buffer_managed;

  PolarisCallback_t rtcm_callback;
} PolarisContext_t;

int Polaris_Open(PolarisContext_t* context);

int Polaris_OpenWithBuffer(PolarisContext_t* context, uint8_t* buffer,
                           size_t buffer_size);

void Polaris_Close(PolarisContext_t* context);

int Polaris_Authenticate(PolarisContext_t* context, const char* api_key,
                         const char* unique_id);

int Polaris_AuthenticateWith(PolarisContext_t* context, const char* api_key,
                             const char* unique_id, const char* endpoint_url,
                             int endpoint_port);

void Polaris_SetRTCMCallback(PolarisContext_t* context,
                             PolarisCallback_t callback);

void Polaris_Run(PolarisContext_t* context);
