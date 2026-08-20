/**************************************************************************/ /**
 * @brief Test access to the internals of the Polaris C client library.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "polaris_test_shim.h"

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

PolarisTestNetwork_t polaris_test_network;

static int TestGetAddrInfo(const char* node, const char* service,
                           const struct addrinfo* hints,
                           struct addrinfo** result);
static void TestFreeAddrInfo(struct addrinfo* ai);
static int TestConnect(int fd, const struct sockaddr* addr, socklen_t length);
static int TestPoll(struct pollfd* fds, nfds_t count, int timeout_ms);
static int TestGetSockOpt(int fd, int level, int name, void* value,
                          socklen_t* length);

// Interpose on the calls the library makes. These must be in place before the
// include below so that its calls resolve to them.
//
// Note that socket() and close() are deliberately left alone: PolarisContext_t
// has a member named `socket`, so a macro of that name would rename the member
// too. Letting real descriptors be created is harmless here because the
// interposed connect() means nothing is ever sent over them.
#define getaddrinfo TestGetAddrInfo
#define freeaddrinfo TestFreeAddrInfo
#define connect TestConnect
#define poll TestPoll
#define getsockopt TestGetSockOpt

#include "point_one/polaris/polaris.c"

// The library's other translation units are included rather than linked so that
// the interposed calls apply to all of them.
#include "point_one/polaris/polaris_internal.c"
#include "point_one/polaris/portability.c"

#undef getaddrinfo
#undef freeaddrinfo
#undef connect
#undef poll
#undef getsockopt

/******************************************************************************/
/*!
@brief Allocate one resolved IPv4 loopback address.

@param port The port to store in the address.

@return The newly allocated entry.
*/
static struct addrinfo* MakeAddress(int port) {
  struct sockaddr_in* address = calloc(1, sizeof(struct sockaddr_in));
  address->sin_family = AF_INET;
  address->sin_port = htons((uint16_t)port);
  address->sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  struct addrinfo* entry = calloc(1, sizeof(struct addrinfo));
  entry->ai_family = AF_INET;
  entry->ai_socktype = SOCK_STREAM;
  entry->ai_protocol = IPPROTO_TCP;
  entry->ai_addr = (struct sockaddr*)address;
  entry->ai_addrlen = sizeof(struct sockaddr_in);

  return entry;
}

/*!
@brief Build a resolver result holding two addresses.

The corrections service resolves to several A records, so the library has to be
able to move on to the next one when an address does not answer.

@param port The port to store in each address.

@return The head of the newly allocated list.
*/
static struct addrinfo* MakeMultiAddressResult(int port) {
  struct addrinfo* first = MakeAddress(port);
  first->ai_next = MakeAddress(port);
  return first;
}

/******************************************************************************/
static int TestGetAddrInfo(const char* node, const char* service,
                           const struct addrinfo* hints,
                           struct addrinfo** result) {
  if (polaris_test_network.getaddrinfo_result != 0) {
    if (polaris_test_network.getaddrinfo_result == EAI_SYSTEM) {
      errno = polaris_test_network.getaddrinfo_errno;
    }
    return polaris_test_network.getaddrinfo_result;
  }

  *result = MakeMultiAddressResult(atoi(service));
  return 0;
}

/******************************************************************************/
static void TestFreeAddrInfo(struct addrinfo* ai) {
  while (ai != NULL) {
    struct addrinfo* next = ai->ai_next;
    free(ai->ai_addr);
    free(ai);
    ai = next;
  }
}

/******************************************************************************/
static int TestConnect(int fd, const struct sockaddr* addr, socklen_t length) {
  ++polaris_test_network.connect_call_count;

  if (polaris_test_network.disconnect_after_first_connect != NULL &&
      polaris_test_network.connect_call_count == 1) {
    polaris_test_network.disconnect_after_first_connect->disconnected = 1;
  }

  if (polaris_test_network.connect_errno != 0) {
    errno = polaris_test_network.connect_errno;
    return -1;
  }
  return 0;
}

/******************************************************************************/
static int TestPoll(struct pollfd* fds, nfds_t count, int timeout_ms) {
  if (polaris_test_network.disconnect_during_poll != NULL) {
    polaris_test_network.disconnect_during_poll->disconnected = 1;
  }

  if (polaris_test_network.poll_result < 0) {
    errno = polaris_test_network.poll_errno;
  } else if (polaris_test_network.poll_result > 0) {
    fds[0].revents = POLLOUT;
  }

  return polaris_test_network.poll_result;
}

/******************************************************************************/
static int TestGetSockOpt(int fd, int level, int name, void* value,
                          socklen_t* length) {
  if (level == SOL_SOCKET && name == SO_ERROR) {
    *(int*)value = polaris_test_network.socket_error;
  }
  return 0;
}

/******************************************************************************/
void PolarisTest_ResetNetwork(void) {
  memset(&polaris_test_network, 0, sizeof(polaris_test_network));
  polaris_test_network.poll_result = 1;
}

/******************************************************************************/
int PolarisTest_IsNoNetworkError(int error_code) {
  return IsNoNetworkError(error_code);
}

/******************************************************************************/
int PolarisTest_ResolveAndConnect(PolarisContext_t* context,
                                  const char* endpoint_url, int endpoint_port) {
  return ResolveAndConnect(context, endpoint_url, endpoint_port);
}

/******************************************************************************/
int PolarisTest_ValidateUniqueID(const char* unique_id) {
  return ValidateUniqueID(unique_id);
}

/******************************************************************************/
int PolarisTest_OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                           int endpoint_port) {
  return OpenSocket(context, endpoint_url, endpoint_port);
}

/******************************************************************************/
int PolarisTest_SendData(PolarisContext_t* context, const void* buffer,
                         size_t size) {
  return SendData(context, buffer, size);
}

#if POLARIS_ENABLE_TLS
/******************************************************************************/
void PolarisTest_AttachClientTLS(PolarisContext_t* context, int fd) {
  SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_client_method());
  SSL* ssl = SSL_new(ssl_ctx);
  SSL_set_fd(ssl, fd);
  SSL_set_connect_state(ssl);

  context->ssl_ctx = ssl_ctx;
  context->ssl = ssl;
  context->use_tls = 1;
}
#endif  // POLARIS_ENABLE_TLS
