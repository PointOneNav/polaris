/**************************************************************************/ /**
 * @brief POSIX socket support wrapper.
 *
 * We align our socket functionality as closely as possible with the POSIX
 * Berkeley sockets API, with minor changes and definitions made to enable other
 * OSes with similar APIs.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <netinet/in.h> // For IPPROTO_* macros and hton*()
#include <netdb.h> // For gethostbyname() and hostent
#include <string.h> // For memcpy()
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h> // For close()

typedef int P1_Socket_t;
#define P1_INVALID_SOCKET -1

typedef struct sockaddr_in P1_SocketAddrV4_t;
typedef struct sockaddr P1_SocketAddr_t;

typedef ssize_t P1_RecvSize_t;

typedef struct timeval P1_TimeValue_t;

static inline void P1_SetTime(int time_ms, P1_TimeValue_t* result) {
  result->tv_sec = time_ms / 1000;
  result->tv_usec = (time_ms % 1000) * 1000;
}

static int P1_SetAddress(const char* hostname, int port,
                         P1_SocketAddrV4_t* result) {
  struct hostent* host_info = gethostbyname(hostname);
  if (host_info == NULL) {
    return -1;
  }
  else {
    result->sin_family = AF_INET;
    result->sin_port = htons(port);
    memcpy(&result->sin_addr, host_info->h_addr_list[0], host_info->h_length);
    return 0;
  }
}
