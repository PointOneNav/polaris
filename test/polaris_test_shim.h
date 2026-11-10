/**************************************************************************/ /**
 * @brief Test access to the internals of the Polaris C client library.
 *
 * The library is compiled as C, so the shim that includes it is also C. That
 * keeps the code under test identical to what ships, rather than whatever a C++
 * compiler makes of it.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include "point_one/polaris/polaris.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Controls the socket calls interposed by the shim.
 *
 * This lets the connection failure modes be exercised without depending on the
 * network, or on the machine having (or lacking) a route to anywhere.
 */
typedef struct {
  /**
   * The value for `getaddrinfo()` to return, or 0 to resolve to two loopback
   * addresses, mimicking a hostname with more than one A record.
   */
  int getaddrinfo_result;

  /** The `errno` to report when @ref getaddrinfo_result is `EAI_SYSTEM`. */
  int getaddrinfo_errno;

  /**
   * The `errno` for `connect()` to fail with, or 0 to report immediate success.
   * `EINPROGRESS` defers the result to @ref poll_result.
   */
  int connect_errno;

  /**
   * The value for `poll()` to return: 0 for a timeout, 1 for the socket
   * becoming writable, or -1 for an error.
   */
  int poll_result;

  /** The `errno` to report when @ref poll_result is negative. */
  int poll_errno;

  /** The value reported by `getsockopt(SO_ERROR)` after `poll()` returns. */
  int socket_error;

  /** The number of `connect()` calls made since the last reset. */
  int connect_call_count;

  /**
   * When set, `poll()` marks this context disconnected before returning, which
   * is what @ref Polaris_Disconnect() does when called from another thread
   * while a connection is being established.
   */
  PolarisContext_t* disconnect_during_poll;

  /**
   * When set, `connect()` marks this context disconnected after the first
   * attempt, modelling a disconnect request that arrives while the remaining
   * resolved addresses are still being tried.
   */
  PolarisContext_t* disconnect_after_first_connect;

  /**
   * When set, `connect()` marks this context disconnected during the attempt
   * on the last of the two resolved addresses, modelling a disconnect request
   * that arrives when there are no more addresses left to try.
   */
  PolarisContext_t* disconnect_during_last_connect;

  /**
   * When set, `getaddrinfo()` calls @ref Polaris_Disconnect() on this context
   * before resolving, modelling a disconnect request that arrives from another
   * thread before any socket has been opened.
   */
  PolarisContext_t* disconnect_during_getaddrinfo;
} PolarisTestNetwork_t;

extern PolarisTestNetwork_t polaris_test_network;

/**
 * @brief Restore the interposed calls to their default (succeeding) behavior.
 */
void PolarisTest_ResetNetwork(void);

/** @brief Call the library's `IsNoNetworkError()`. */
int PolarisTest_IsNoNetworkError(int error_code);

/** @brief Call the library's `ResolveAndConnect()`. */
int PolarisTest_ResolveAndConnect(PolarisContext_t* context,
                                  const char* endpoint_url, int endpoint_port);

/** @brief Call the library's `ValidateUniqueID()`. */
int PolarisTest_ValidateUniqueID(const char* unique_id);

/** @brief Call the library's `OpenSocket()`. */
int PolarisTest_OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                           int endpoint_port);

/** @brief Call the library's `SendData()`. */
int PolarisTest_SendData(PolarisContext_t* context, const void* buffer,
                         size_t size);

/**
 * @brief Get the number of API calls currently using the connection.
 *
 * @param context The Polaris context to be checked.
 *
 * @return The number of calls.
 */
int PolarisTest_GetNumConnectionUsers(PolarisContext_t* context);

#if POLARIS_ENABLE_TLS
/**
 * @brief Configure `context` to send through a TLS session on `fd`.
 *
 * The handshake is deliberately not performed. `SSL_write()` writes the
 * ClientHello to the socket, and that write is made by the TLS library itself,
 * which is the case the SIGPIPE suppression in `SendData()` has to cover.
 * Whether the bytes are a handshake record or application data makes no
 * difference to it.
 *
 * This lives here rather than in the test because the TLS headers are only
 * usable from C, matching how the library is built. See the note on `ssl_ctx`
 * in polaris.h.
 *
 * @param context The Polaris context to configure.
 * @param fd The socket to attach the session to.
 */
void PolarisTest_AttachClientTLS(PolarisContext_t* context, int fd);
#endif  // POLARIS_ENABLE_TLS

#ifdef __cplusplus
}  // extern "C"
#endif
