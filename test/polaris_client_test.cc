/**************************************************************************/ /**
 * @brief Unit tests for the Polaris C++ client.
 *
 * These tests run the client against a stand-in endpoint listening on loopback
 * so that reconnect behavior can be observed without touching the network. The
 * client does not interpret the corrections stream, so the payload only needs
 * to be the right size.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include "point_one/polaris/polaris_client.h"

using namespace point_one::polaris;

namespace {

// The client treats a connection as healthy once it has received more than the
// maximum size of a single RTCM 1029 message, since the network answers a
// rejected request with one of those and then disconnects.
constexpr int kHealthyByteCount = 600;
constexpr int kUnhealthyByteCount = 100;

/**
 * @brief A stand-in for the Polaris corrections endpoint.
 *
 * Accepts connections on loopback, sends a fixed number of bytes, then closes,
 * forcing the client to reconnect.
 */
class FakeEndpoint {
 public:
  explicit FakeEndpoint(int bytes_per_connection)
      : bytes_per_connection_(bytes_per_connection) {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    EXPECT_GE(listen_fd_, 0);

    int reuse = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;  // Any available port.
    EXPECT_EQ(0, bind(listen_fd_, reinterpret_cast<sockaddr*>(&address),
                      sizeof(address)));
    EXPECT_EQ(0, listen(listen_fd_, 8));

    socklen_t length = sizeof(address);
    EXPECT_EQ(0, getsockname(listen_fd_, reinterpret_cast<sockaddr*>(&address),
                             &length));
    port_ = ntohs(address.sin_port);

    thread_.reset(new std::thread(&FakeEndpoint::Serve, this));
  }

  ~FakeEndpoint() {
    running_ = false;
    shutdown(listen_fd_, SHUT_RDWR);
    close(listen_fd_);
    if (thread_) {
      thread_->join();
    }
  }

  int port() const { return port_; }

  int connection_count() const { return connection_count_; }

 private:
  void Serve() {
    while (running_) {
      int fd = accept(listen_fd_, nullptr, nullptr);
      if (fd < 0) {
        return;
      }

      ++connection_count_;

      // Read whatever control message the client opens with, then stream.
      std::vector<uint8_t> scratch(256);
      recv(fd, scratch.data(), scratch.size(), MSG_DONTWAIT);

      std::vector<uint8_t> payload(bytes_per_connection_, 0);
      send(fd, payload.data(), payload.size(), MSG_NOSIGNAL);

      // Give the client a moment to read before dropping the connection.
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      close(fd);
    }
  }

  int bytes_per_connection_;
  int listen_fd_ = -1;
  int port_ = 0;
  std::atomic<bool> running_{true};
  std::atomic<int> connection_count_{0};
  std::unique_ptr<std::thread> thread_;
};

/**
 * @brief Configure a client to talk to a stand-in endpoint over loopback.
 */
void PointAtFakeEndpoint(PolarisClient* client, int port) {
  client->SetNoAuthID("unit-test");
  client->UseTLS(false);
  client->SetPolarisEndpoint("127.0.0.1", port);
}

class PolarisClientTest : public ::testing::Test {
 protected:
  void TearDown() override { client_.Disconnect(); }

  PolarisClient client_;
};

/******************************************************************************/
TEST_F(PolarisClientTest, ReceivesCorrections) {
  FakeEndpoint endpoint(kHealthyByteCount);
  std::atomic<size_t> received{0};

  PointAtFakeEndpoint(&client_, endpoint.port());
  client_.SetRTCMCallback([&](const uint8_t* buffer, size_t size_bytes) {
    received += size_bytes;
  });
  client_.SendLLAPosition(37.0, -122.0, 0.0);

  client_.RunAsync();
  std::this_thread::sleep_for(std::chrono::seconds(2));
  client_.Disconnect();

  EXPECT_GT(received.load(), 0u);
}

/******************************************************************************/
TEST_F(PolarisClientTest, HealthyConnectionsAreNotPenalized) {
  // Each connection delivers enough data to count as healthy, so the backoff
  // must be reset rather than growing toward its maximum.
  FakeEndpoint endpoint(kHealthyByteCount);
  PointAtFakeEndpoint(&client_, endpoint.port());
  client_.SendLLAPosition(37.0, -122.0, 0.0);

  client_.RunAsync();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  client_.Disconnect();

  // Without the reset the delays would be 1s, 2s and 4s, allowing at most about
  // three connections in this window.
  EXPECT_GE(endpoint.connection_count(), 5)
      << "a connection that keeps delivering data should reconnect promptly";
}

/******************************************************************************/
TEST_F(PolarisClientTest, UnhealthyConnectionsBackOff) {
  // Each connection delivers too little data to count as healthy, which is what
  // an error response looks like, so the delay must grow.
  FakeEndpoint endpoint(kUnhealthyByteCount);
  PointAtFakeEndpoint(&client_, endpoint.port());
  client_.SendLLAPosition(37.0, -122.0, 0.0);

  client_.RunAsync();
  std::this_thread::sleep_for(std::chrono::seconds(5));
  client_.Disconnect();

  // Delays of 1s, 2s and 4s permit only a handful of attempts in this window.
  EXPECT_LE(endpoint.connection_count(), 4)
      << "repeated short connections should be retried more and more slowly";
  EXPECT_GE(endpoint.connection_count(), 1);
}

/******************************************************************************/
TEST_F(PolarisClientTest, DisconnectReturnsPromptlyWhileStreaming) {
  FakeEndpoint endpoint(kHealthyByteCount);
  PointAtFakeEndpoint(&client_, endpoint.port());
  client_.SendLLAPosition(37.0, -122.0, 0.0);

  client_.RunAsync();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  auto start = std::chrono::steady_clock::now();
  client_.Disconnect();
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();

  EXPECT_LT(elapsed_ms, 1000) << "Disconnect() should not wait for a retry";
}

/******************************************************************************/
TEST_F(PolarisClientTest, DisconnectInterruptsTheRetryDelay) {
  // Nothing is listening, so the client will be waiting out a backoff delay
  // when the disconnect arrives.
  client_.SetNoAuthID("unit-test");
  client_.UseTLS(false);
  client_.SetPolarisEndpoint("127.0.0.1", 1);  // Reserved; refuses connections.
  client_.SendLLAPosition(37.0, -122.0, 0.0);

  client_.RunAsync();
  std::this_thread::sleep_for(std::chrono::milliseconds(2500));

  auto start = std::chrono::steady_clock::now();
  client_.Disconnect();
  auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::steady_clock::now() - start)
                        .count();

  EXPECT_LT(elapsed_ms, 500)
      << "a disconnect during the backoff delay should not wait it out";
}

/******************************************************************************/
TEST_F(PolarisClientTest, RunReturnsAfterDisconnect) {
  // Disconnect() joins the run thread, so a hang here means Run() never exited.
  FakeEndpoint endpoint(kHealthyByteCount);
  PointAtFakeEndpoint(&client_, endpoint.port());
  client_.SendLLAPosition(37.0, -122.0, 0.0);

  client_.RunAsync();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  client_.Disconnect();

  // A second disconnect must be harmless.
  client_.Disconnect();
}

}  // namespace
