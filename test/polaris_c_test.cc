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
#include <string>

#include <gtest/gtest.h>

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

}  // namespace
