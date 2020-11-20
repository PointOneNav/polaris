/**************************************************************************/ /**
 * @brief FreeRTOS+TCP socket support wrapper.
 *
 * The FreeRTOS TCP library's socket API is very similar to the POSIX Berkeley
 * sockets API. We map FreeRTOS calls to the POSIX equivalents wherever
 * possible.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <FreeRTOS_sockets.h>

typedef Socket_t P1_Socket_t;
#define P1_INVALID_SOCKET FREERTOS_INVALID_SOCKET

typedef struct freertos_sockaddr P1_SocketAddrV4_t;
typedef struct freertos_sockaddr P1_SocketAddr_t;

typedef BaseType_t P1_RecvSize_t;

#define AF_INET FREERTOS_AF_INET

#define SOCK_STREAM FREERTOS_SOCK_STREAM

#define IPPROTO_TCP FREERTOS_IPPROTO_UDP

#define SO_RCVTIMEO FREERTOS_SO_RCVTIMEO
#define SO_SNDTIMEO FREERTOS_SO_SNDTIMEO

typedef TickType_t P1_TimeValue_t;

static inline void P1_SetTime(int time_ms, P1_TimeValue_t* result) {
  *result = pdMS_TO_TICKS(time_ms);
}

static int P1_SetAddress(const char* hostname, int port,
                         P1_SocketAddrV4_t* result) {
  uint32_t ip = FreeRTOS_gethostbyname(hostname);
  if (ip == NULL) {
    return -1;
  }
  else {
    result->sin_family = AF_INET;
    result->sin_port = FreeRTOS_htons(port);
    result->sin_addr.addr = ip;
    return 0;
  }
}

// Aliases mapping FreeRTOS function names to Berkeley names. The APIs are the
// same as the Berkeley definitions for all of these functions.
#define socket FreeRTOS_socket
#define close FreeRTOS_closesocket
#define connect FreeRTOS_connect
#define send FreeRTOS_send
#define recv FreeRTOS_recv
#define setsockopt FreeRTOS_setsockopt
#define htons FreeRTOS_htons