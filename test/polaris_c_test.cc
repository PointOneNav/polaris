/**************************************************************************/ /**
 * @brief Unit tests for the Polaris C client library.
 *
 * The library's file-static functions and its socket calls are reached through
 * polaris_test_shim.c, which lets the connection failure modes (no route,
 * unreachable resolver, unresponsive endpoint) be exercised deterministically,
 * without depending on the machine's network.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>

#include <gtest/gtest.h>

#if POLARIS_ENABLE_TLS
#  include <openssl/ssl.h>
#endif

#include "polaris_test_shim.h"

namespace {

class PolarisCTest : public ::testing::Test {
 protected:
  void SetUp() override {
    PolarisTest_ResetNetwork();
    Polaris_Init(&context_);
    Polaris_SetLogLevel(POLARIS_LOG_LEVEL_FATAL);
  }

  void TearDown() override { context_.socket = P1_INVALID_SOCKET; }

  PolarisContext_t context_;
};

// ---------------------------------------------------------------------------
// Error classification.
// ---------------------------------------------------------------------------

/******************************************************************************/
TEST_F(PolarisCTest, NoNetworkErrorsAreRecognized) {
  EXPECT_TRUE(PolarisTest_IsNoNetworkError(ENETDOWN));
  EXPECT_TRUE(PolarisTest_IsNoNetworkError(ENETUNREACH));
  EXPECT_TRUE(PolarisTest_IsNoNetworkError(EHOSTDOWN));
  EXPECT_TRUE(PolarisTest_IsNoNetworkError(EHOSTUNREACH));
  EXPECT_TRUE(PolarisTest_IsNoNetworkError(EADDRNOTAVAIL));
  EXPECT_TRUE(PolarisTest_IsNoNetworkError(EAFNOSUPPORT));
}

/******************************************************************************/
TEST_F(PolarisCTest, EndpointErrorsAreNotTreatedAsNoNetwork) {
  // A refused or reset connection means we reached the network and the endpoint
  // rejected us, which retrying will not fix.
  EXPECT_FALSE(PolarisTest_IsNoNetworkError(ECONNREFUSED));
  EXPECT_FALSE(PolarisTest_IsNoNetworkError(ECONNRESET));
  EXPECT_FALSE(PolarisTest_IsNoNetworkError(ETIMEDOUT));
  EXPECT_FALSE(PolarisTest_IsNoNetworkError(EINTR));
  EXPECT_FALSE(PolarisTest_IsNoNetworkError(0));
}

// ---------------------------------------------------------------------------
// Address resolution.
// ---------------------------------------------------------------------------

/******************************************************************************/
TEST_F(PolarisCTest, UnreachableResolverReportsNoNetwork) {
  // A device with no connectivity cannot reach a DNS server, which the resolver
  // reports as a temporary failure.
  polaris_test_network.getaddrinfo_result = EAI_AGAIN;
  EXPECT_EQ(POLARIS_NO_NETWORK,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

/******************************************************************************/
TEST_F(PolarisCTest, UnknownHostIsNotReportedAsNoNetwork) {
  // An unknown hostname is a configuration problem, not a transient one.
  polaris_test_network.getaddrinfo_result = EAI_NONAME;
  EXPECT_EQ(POLARIS_SOCKET_ERROR,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

/******************************************************************************/
TEST_F(PolarisCTest, ResolverSystemErrorIsClassifiedByErrno) {
  polaris_test_network.getaddrinfo_result = EAI_SYSTEM;
  polaris_test_network.getaddrinfo_errno = ENETUNREACH;
  EXPECT_EQ(POLARIS_NO_NETWORK,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));

  polaris_test_network.getaddrinfo_errno = ENOMEM;
  EXPECT_EQ(POLARIS_SOCKET_ERROR,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

// ---------------------------------------------------------------------------
// Connection establishment.
// ---------------------------------------------------------------------------

/******************************************************************************/
TEST_F(PolarisCTest, ConnectsToTheFirstAddressOffered) {
  EXPECT_EQ(POLARIS_SUCCESS,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
  EXPECT_EQ(1, polaris_test_network.connect_call_count)
      << "the remaining addresses should not be tried after one connects";
}

/******************************************************************************/
TEST_F(PolarisCTest, FallsBackToTheNextAddressWhenTheFirstIsRefused) {
  // Mimics one of the endpoint's addresses not answering.
  polaris_test_network.connect_errno = ECONNREFUSED;
  PolarisTest_ResolveAndConnect(&context_, "example.com", 80);
  EXPECT_EQ(2, polaris_test_network.connect_call_count)
      << "every resolved address should have been tried";
}

/******************************************************************************/
TEST_F(PolarisCTest, UnroutableAddressReportsNoNetwork) {
  polaris_test_network.connect_errno = ENETUNREACH;
  EXPECT_EQ(POLARIS_NO_NETWORK,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

/******************************************************************************/
TEST_F(PolarisCTest, RefusedConnectionReportsSocketError) {
  polaris_test_network.connect_errno = ECONNREFUSED;
  EXPECT_EQ(POLARIS_SOCKET_ERROR,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

/******************************************************************************/
TEST_F(PolarisCTest, UnresponsiveEndpointTimesOut) {
  // The connection is accepted for delivery but never completes, which is what
  // a silently dropped SYN looks like.
  polaris_test_network.connect_errno = EINPROGRESS;
  polaris_test_network.poll_result = 0;
  EXPECT_EQ(POLARIS_TIMED_OUT,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

/******************************************************************************/
TEST_F(PolarisCTest, DeferredConnectFailureIsClassified) {
  // The failure is reported later via SO_ERROR rather than by connect() itself.
  polaris_test_network.connect_errno = EINPROGRESS;
  polaris_test_network.poll_result = 1;
  polaris_test_network.socket_error = ENETUNREACH;
  EXPECT_EQ(POLARIS_NO_NETWORK,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));

  polaris_test_network.socket_error = ECONNREFUSED;
  EXPECT_EQ(POLARIS_SOCKET_ERROR,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

/******************************************************************************/
TEST_F(PolarisCTest, DisconnectWhileWaitingForConnectIsNotAnError) {
  // Polaris_Disconnect() interrupts the poll and closes the socket. The user
  // asked us to stop, so this must not be reported as a failure.
  polaris_test_network.connect_errno = EINPROGRESS;
  polaris_test_network.poll_result = -1;
  polaris_test_network.poll_errno = EINTR;
  polaris_test_network.disconnect_during_poll = &context_;
  EXPECT_EQ(POLARIS_CONNECTION_CLOSED,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
}

/******************************************************************************/
TEST_F(PolarisCTest, DisconnectBeforeConnectingReportsTheSameResult) {
  // Cancellation must be reported the same way wherever it lands, so that
  // callers do not have to distinguish "canceled" from "failed" by timing.
  context_.disconnected = 1;
  EXPECT_EQ(POLARIS_CONNECTION_CLOSED,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
  EXPECT_EQ(0, polaris_test_network.connect_call_count)
      << "no connection should be attempted after a disconnect request";
}

/******************************************************************************/
TEST_F(PolarisCTest, DisconnectStopsBeforeTryingRemainingAddresses) {
  // The first address fails and the user disconnects, so the second address
  // must not be tried.
  polaris_test_network.connect_errno = ECONNREFUSED;
  polaris_test_network.disconnect_after_first_connect = &context_;
  EXPECT_EQ(POLARIS_CONNECTION_CLOSED,
            PolarisTest_ResolveAndConnect(&context_, "example.com", 80));
  EXPECT_EQ(1, polaris_test_network.connect_call_count)
      << "the second address should not be tried after a disconnect request";
}

// ---------------------------------------------------------------------------
// Input validation.
// ---------------------------------------------------------------------------

/******************************************************************************/
TEST_F(PolarisCTest, UniqueIDsAreValidated) {
  EXPECT_EQ(POLARIS_SUCCESS, PolarisTest_ValidateUniqueID(NULL));
  EXPECT_EQ(POLARIS_SUCCESS, PolarisTest_ValidateUniqueID(""));
  EXPECT_EQ(POLARIS_SUCCESS, PolarisTest_ValidateUniqueID("abc-DEF_123"));

  EXPECT_EQ(POLARIS_ERROR, PolarisTest_ValidateUniqueID("has space"));
  EXPECT_EQ(POLARIS_ERROR, PolarisTest_ValidateUniqueID("has.dot"));

  const std::string too_long(POLARIS_MAX_UNIQUE_ID_SIZE + 1, 'a');
  EXPECT_EQ(POLARIS_ERROR, PolarisTest_ValidateUniqueID(too_long.c_str()));
  const std::string max_length(POLARIS_MAX_UNIQUE_ID_SIZE, 'a');
  EXPECT_EQ(POLARIS_SUCCESS, PolarisTest_ValidateUniqueID(max_length.c_str()));
}

/******************************************************************************/
TEST_F(PolarisCTest, AuthTokensAreValidated) {
  EXPECT_EQ(POLARIS_ERROR, Polaris_SetAuthToken(&context_, ""));

  const std::string too_long(POLARIS_MAX_TOKEN_SIZE + 1, 'a');
  EXPECT_EQ(POLARIS_NOT_ENOUGH_SPACE,
            Polaris_SetAuthToken(&context_, too_long.c_str()));

  EXPECT_EQ(POLARIS_SUCCESS, Polaris_SetAuthToken(&context_, "a-token"));
  EXPECT_STREQ("a-token", context_.auth_token);
}

/******************************************************************************/
TEST_F(PolarisCTest, SendsAreRejectedWhenNotConnected) {
  context_.socket = P1_INVALID_SOCKET;
  EXPECT_EQ(POLARIS_SOCKET_ERROR,
            Polaris_SendLLAPosition(&context_, 37.0, -122.0, 0.0));
  EXPECT_EQ(POLARIS_SOCKET_ERROR,
            Polaris_SendECEFPosition(&context_, 1.0, 2.0, 3.0));
  EXPECT_EQ(POLARIS_SOCKET_ERROR, Polaris_RequestBeacon(&context_, "beacon"));
}

// ---------------------------------------------------------------------------
// TLS configuration.
// ---------------------------------------------------------------------------

/******************************************************************************/
TEST_F(PolarisCTest, TLSRequestIsHonoredOrRefusedButNeverDowngradedSilently) {
#if POLARIS_ENABLE_TLS
  EXPECT_EQ(POLARIS_SUCCESS, Polaris_UseTLS(&context_, 1));
  EXPECT_EQ(1, context_.use_tls);
#else
  // Without TLS support the request must be refused, and must not leave the
  // context configured for a plaintext connection the caller did not ask for.
  EXPECT_EQ(POLARIS_ERROR, Polaris_UseTLS(&context_, 1));
  EXPECT_EQ(1, context_.use_tls);
  EXPECT_EQ(POLARIS_ERROR,
            PolarisTest_OpenSocket(&context_, "example.com", 80));
#endif

  EXPECT_EQ(POLARIS_SUCCESS, Polaris_UseTLS(&context_, 0));
  EXPECT_EQ(0, context_.use_tls);
}

// ---------------------------------------------------------------------------
// Pipe failures.
// ---------------------------------------------------------------------------

/** @brief The result of a send performed in a forked child. */
struct SendOutcome {
  /** Whether the child was killed by SIGPIPE rather than returning normally. */
  bool killed_by_sigpipe;

  /** The value returned by the library's `SendData()`. */
  int ret;

  /** The `errno` reported once `SendData()` returned. */
  int err;
};

/*!
@brief Open a socket whose peer has already closed.

A closed AF_UNIX peer fails the very next write, so a pipe failure can be
produced without depending on TCP teardown timing.

@return The local end of the socket pair, or -1 on failure.
*/
int OpenSocketWithClosedPeer() {
  int fds[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0) {
    return -1;
  }
  close(fds[1]);
  return fds[0];
}

#if POLARIS_ENABLE_TLS
/*!
@brief Configure `context` to send through a TLS session on `fd`.

The handshake is deliberately not performed. `SSL_write()` writes the
ClientHello to the socket, and that write is made by the TLS library itself,
which is the case the suppression in `SendData()` has to cover. Whether the
bytes are a handshake record or application data makes no difference to it.

@param context The Polaris context to configure.
@param fd The socket to attach the session to.
*/
void AttachClientTLS(PolarisContext_t* context, int fd) {
  SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_client_method());
  SSL* ssl = SSL_new(ssl_ctx);
  SSL_set_fd(ssl, fd);
  SSL_set_connect_state(ssl);

  context->ssl_ctx = ssl_ctx;
  context->ssl = ssl;
  context->use_tls = 1;
}
#endif  // POLARIS_ENABLE_TLS

/*!
@brief Send to a closed peer and report how the send behaved.

The send runs in a forked child so that a SIGPIPE which is not suppressed fails
this one test, instead of killing the test binary along with every test after
it.

@param use_tls If `true`, send through a TLS session rather than calling
       `send()` directly.

@return What happened to the send.
*/
SendOutcome SendToClosedPeer(bool use_tls) {
  int results[2];
  if (pipe(results) != 0) {
    ADD_FAILURE() << "could not create the result pipe";
    return SendOutcome{false, 0, 0};
  }

  const pid_t pid = fork();
  if (pid < 0) {
    ADD_FAILURE() << "could not fork";
    return SendOutcome{false, 0, 0};
  }

  // In the child, perform the send and report the result back.
  if (pid == 0) {
    close(results[0]);

    PolarisContext_t context;
    Polaris_Init(&context);
    Polaris_SetLogLevel(POLARIS_LOG_LEVEL_FATAL);
    context.socket = OpenSocketWithClosedPeer();

    if (use_tls) {
#if POLARIS_ENABLE_TLS
      AttachClientTLS(&context, context.socket);
#endif
    } else {
      Polaris_UseTLS(&context, 0);
    }

    errno = 0;
    const int ret = PolarisTest_SendData(&context, "x", 1);
    const int child_result[2] = {ret, errno};
    ssize_t ignored = write(results[1], child_result, sizeof(child_result));
    (void)ignored;
    _exit(0);
  }

  // In the parent, collect the reported result and how the child exited.
  close(results[1]);
  int child_result[2] = {0, 0};
  const ssize_t bytes = read(results[0], child_result, sizeof(child_result));
  close(results[0]);

  int status = 0;
  waitpid(pid, &status, 0);

  SendOutcome outcome{};
  outcome.killed_by_sigpipe =
      WIFSIGNALED(status) && WTERMSIG(status) == SIGPIPE;
  if (bytes == static_cast<ssize_t>(sizeof(child_result))) {
    outcome.ret = child_result[0];
    outcome.err = child_result[1];
  }
  return outcome;
}

/******************************************************************************/
TEST_F(PolarisCTest, SigPipeIsFatalUnlessSuppressed) {
  // The tests below only prove anything if SIGPIPE would otherwise kill the
  // process. If something in this binary ignored it, they would pass without
  // exercising the suppression at all.
  struct sigaction current;
  ASSERT_EQ(0, sigaction(SIGPIPE, NULL, &current));
  EXPECT_TRUE(current.sa_handler == SIG_DFL)
      << "SIGPIPE must be left at its default disposition for these tests";
}

/******************************************************************************/
TEST_F(PolarisCTest, BrokenPipeIsReportedRatherThanFatalWithoutTLS) {
  // Losing connectivity mid-drive, e.g. a vehicle leaving WiFi range, makes the
  // next send hit a closed pipe. It has to come back as an error the caller can
  // act on rather than a signal that takes down the application.
  const SendOutcome outcome = SendToClosedPeer(false);
  EXPECT_FALSE(outcome.killed_by_sigpipe)
      << "send() raised a fatal SIGPIPE; MSG_NOSIGNAL is not taking effect";
  EXPECT_EQ(-1, outcome.ret);
  EXPECT_EQ(EPIPE, outcome.err);
}

#if POLARIS_ENABLE_TLS
/******************************************************************************/
TEST_F(PolarisCTest, BrokenPipeIsReportedRatherThanFatalWithTLS) {
  // The TLS library writes to the socket internally, so it cannot be passed
  // MSG_NOSIGNAL and relies on the separate handling in SendData().
  const SendOutcome outcome = SendToClosedPeer(true);
  EXPECT_FALSE(outcome.killed_by_sigpipe)
      << "SSL_write() raised a fatal SIGPIPE; it is not being suppressed";
  EXPECT_LE(outcome.ret, 0) << "the write should have reported a failure";
  EXPECT_EQ(EPIPE, outcome.err);
}
#endif  // POLARIS_ENABLE_TLS

}  // namespace
