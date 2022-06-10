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
