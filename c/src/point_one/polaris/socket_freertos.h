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

#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

typedef Socket_t P1_Socket_t;
#define P1_INVALID_SOCKET FREERTOS_INVALID_SOCKET

typedef struct freertos_sockaddr P1_SocketAddrV4_t;
typedef struct freertos_sockaddr P1_SocketAddr_t;

typedef BaseType_t P1_RecvSize_t;

#define AF_INET FREERTOS_AF_INET

#define SOCK_STREAM FREERTOS_SOCK_STREAM

#define IPPROTO_TCP FREERTOS_IPPROTO_TCP

// Not currently used/ignored by FreeRTOS. Setting to the same value as POSIX.
#define SOL_SOCKET 1

#define SO_RCVTIMEO FREERTOS_SO_RCVTIMEO
#define SO_SNDTIMEO FREERTOS_SO_SNDTIMEO

#define SHUT_RDWR FREERTOS_SHUT_RDWR

// Aliases mapping FreeRTOS function names to Berkeley names. The APIs are the
// same as the Berkeley definitions for all of these functions.
#define socket FreeRTOS_socket
#define close FreeRTOS_closesocket
#define shutdown FreeRTOS_shutdown
#define connect FreeRTOS_connect
#define send FreeRTOS_send
#define recv FreeRTOS_recv
#define setsockopt FreeRTOS_setsockopt
#define htons FreeRTOS_htons

// Note: FreeRTOS does not define htole*() macros.
#if ipconfigBYTE_ORDER == pdFREERTOS_LITTLE_ENDIAN
# define htole16(x) (x)
# define le16toh(x) (x)
# define htole32(x) (x)
# define le32toh(x) (x)
#else
// Taken from FreeRTOS_IP.h.
# define htole16(x) ((uint16_t)(((x) << 8U) | ((x) >> 8U)))
# define le16toh(x) htole16(x)
# define htole32(x)                                                         \
  ((uint32_t)(                                                              \
      ((((uint32_t)(x))) << 24) | ((((uint32_t)(x)) & 0x0000ff00UL) << 8) | \
      ((((uint32_t)(x)) & 0x00ff0000UL) >> 8) | ((((uint32_t)(x))) >> 24)))
# define le32toh(x) htole32(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline int P1_SetAddress(const char* hostname, int port,
                                P1_SocketAddrV4_t* result) {
  uint32_t ip = FreeRTOS_gethostbyname(hostname);
  if (ip == 0) {
    return -1;
  }
  else {
    result->sin_family = AF_INET;
    result->sin_port = FreeRTOS_htons(port);
    result->sin_addr = ip;
    return 0;
  }
}

#ifdef __cplusplus
} // extern "C"
#endif
