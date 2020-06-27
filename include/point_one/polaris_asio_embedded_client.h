// Copyright (C) Point One Navigation - All Rights Reserved

// Client for connecting to Polaris RTCM corrections service using the
// embedded signing authentication method
#pragma once

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cmath>
#include <iostream>
#include <istream>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "glog/logging.h"

#include "polaris.h"

namespace point_one {
namespace polaris {

namespace {
constexpr int SOCKET_TIMEOUT_MS = 7000;
}  // namespace

class PolarisAsioEmbeddedClient {
 public:
  // How long to wait before sending a new position update.  Polaris
  // Service expects receiving a message within 5 seconds.
  static constexpr int DEFAULT_POSITION_SEND_INTERVAL_MSECS = 3000;

  PolarisAsioEmbeddedClient(boost::asio::io_service &io_service,
                    const std::string &signing_secret,
                    const std::string &unique_id,
                    const std::string &company_id,
                    const PolarisConnectionSettings &connection_settings =
                        DEFAULT_CONNECTION_SETTINGS)
      : connection_settings_(connection_settings),
        signing_secret_(signing_secret),
        io_service_(io_service),
        resolver_(io_service),
        socket_(io_service),
        unique_id_(unique_id),
        company_id_(company_id),
        pos_timer_(io_service),
        reconnect_timer_(io_service, boost::posix_time::milliseconds(
                                         connection_settings.interval_ms)),
        socket_timer_(io_service,
                      boost::posix_time::milliseconds(SOCKET_TIMEOUT_MS)) {}

  ~PolarisAsioEmbeddedClient() {
    polaris_bytes_received_callback_ = nullptr;
    pos_timer_.cancel();
    socket_timer_.cancel();
    reconnect_timer_.cancel();

    std::unique_lock<std::recursive_mutex> guard(lock_);
    if (socket_.is_open()) {
      socket_.close();
    }
  }

  // Connect to Polaris Client and start sending position.
  // Throws Boost system error if url cannot be resolved
  void Connect() {
    connected_ = false;
    connection_active_ = false;
    VLOG(1) << "Attempting to open socket tcp://" << connection_settings_.host
            << ":" << connection_settings_.port;
    std::unique_lock<std::recursive_mutex> guard(lock_);
    try {
      // Close socket if already open.
      socket_.close();

      // Resolve TCP endpoint.
      boost::asio::ip::tcp::resolver::query query(
          connection_settings_.host, std::to_string(connection_settings_.port));
      boost::asio::ip::tcp::resolver::iterator iter = resolver_.resolve(query);
      endpoint_ = iter->endpoint();
    } catch (boost::system::system_error &e) {
      LOG(WARNING) << "Error connecting to Polaris at tcp://"
                   << connection_settings_.host << ":"
                   << connection_settings_.port;
      ScheduleReconnect();
      return;
    }

    socket_.async_connect(endpoint_,
                          boost::bind(&PolarisAsioEmbeddedClient::HandleConnect, this,
                                      boost::asio::placeholders::error));
  }

  // The callback with me iniated when the read buffer is full. Not guaranteed
  // to be a full RTCM message.
  void SetPolarisBytesReceived(
      std::function<void(uint8_t *, uint16_t)> callback) {
    polaris_bytes_received_callback_ = callback;
  }

  // Send a position to Polaris, potentially updating the resulting stream.
  // Store the position for resending until position is updated.  ECEF
  // variant.
  void SetPositionECEF(double x_meters, double y_meters, double z_meters) {
    io_service_.post(boost::bind(&PolarisAsioEmbeddedClient::HandleSetPosition, this,
                                 x_meters, y_meters, z_meters));
  }


 private:
  // The size of the read buffer.
  static constexpr int BUF_SIZE = 1024;

  std::string RequestChallenge() {
    try {
      // Get a list of endpoints corresponding to the server name.
      boost::asio::ip::tcp::resolver resolver(io_service_);
      boost::asio::ip::tcp::resolver::query query(connection_settings_.api_host,
                                                  "http");
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
          resolver.resolve(query);

      // Try each endpoint until we successfully establish a connection.
      boost::asio::ip::tcp::socket socket(io_service_);
      boost::asio::connect(socket, endpoint_iterator);

      // Form the request. We specify the "Connection: close" header so that the
      // server will close the socket after transmitting the response.
      boost::asio::streambuf request;
      std::ostream request_stream(&request);



      request_stream << "GET "
                     << "/api/v1/auth/challenge/request"
                     << " HTTP/1.1\r\n";
      request_stream << "Host: " << connection_settings_.api_host << ":80\r\n";
      request_stream << "Connection: close\r\n";
      request_stream << "Cache-Control: no-cache\r\n";
      request_stream << "\r\n";
      request_stream << "\r\n";

      // Send the request.
      boost::asio::write(socket, request);

      // Read the response status line. The response streambuf will
      // automatically grow to accommodate the entire line. The growth may be
      // limited by passing a maximum size to the streambuf constructor.
      boost::asio::streambuf response;
      boost::asio::read_until(socket, response, "\r\n");

      // Check that response is OK.
      std::istream response_stream(&response);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        LOG(ERROR) << "Invalid HTTP response to challenge request.";
        return "";
      }

      switch(status_code) {
        case 200:
          VLOG(1) << "Received polaris challenge.";
          break;
        default:
          LOG(ERROR) << "Challenge resquest returned with status code "
                     << status_code << ".";
          return "";
      }

      // Process the response headers.
      boost::asio::read_until(socket, response, "\r\n\r\n");
      std::string header;
      VLOG(3) << "Response:";
      while (std::getline(response_stream, header) && header != "\r") {
        VLOG(3) << header;
      }

      // Get the entire reply from the socket.
      std::stringstream reply_body;
      if (response.size() > 0) {
        reply_body << &response;
      }
      boost::system::error_code error;
      while (boost::asio::read(socket, response,
                               boost::asio::transfer_at_least(1), error)) {
        reply_body << &response;
      }
      if (error != boost::asio::error::eof) {
        LOG(ERROR) << "Error occured while receiving challenge: "
                   << error.message();
        return "";
      }

      std::string challenge = "";
      try {
        // Parse json.
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(reply_body, pt);
        challenge = pt.get<std::string>("challenge");
        VLOG(3) << "Challenge is : " << challenge;
      } catch (std::exception &e) {
        LOG(ERROR) << "Exception in parsing challenge request response: " << e.what();
        return "";
      }

      return challenge;
    } catch (std::exception &e) {
      LOG(ERROR) << "Exception: " << e.what();
    }
    return "";
  }

  std::string BuildChallengeSignature(std::string challenge, uint64_t time) {
    // concatenate the string and calculate the message digest

    // challenge| timestamp | company_id | serial_number
    std::stringstream ss;
    ss << challenge << std::to_string(time) << company_id_ << unique_id_;

    unsigned int HMAC_SHA512_HASH_LEN = 64;
    uint8_t buf[HMAC_SHA512_HASH_LEN];
    std::string message = ss.str();
    HMAC_CTX hmac_ctx;
    HMAC_CTX_init(&hmac_ctx);
    HMAC_Init_ex(&hmac_ctx, (void *)signing_secret_.data(),
                 signing_secret_.length(), EVP_sha512(), NULL);
    HMAC_Update(&hmac_ctx, (uint8_t *)message.data(), message.length());
    HMAC_Final(&hmac_ctx, buf, &HMAC_SHA512_HASH_LEN);
    unsigned char encodedData[128] = {};  // this is beyond safe, the max length
                                          // of this is 1.25 * the HASH LEN

    EVP_EncodeBlock(encodedData, buf, HMAC_SHA512_HASH_LEN);
    std::string output((char *)encodedData);
    VLOG(2) << "Challenge Response Signature :" << std::endl << output;
    return output;
  }

  bool GetAuthToken(std::string challenge, uint64_t time, std::string signature) {
    try {
      // Get a list of endpoints corresponding to the server name.
      boost::asio::ip::tcp::resolver resolver(io_service_);
      boost::asio::ip::tcp::resolver::query query(connection_settings_.api_host,
                                                  "http");
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator =
              resolver.resolve(query);

      // Try each endpoint until we successfully establish a connection.
      boost::asio::ip::tcp::socket socket(io_service_);
      boost::asio::connect(socket, endpoint_iterator);

      // Form the request. We specify the "Connection: close" header so that the
      // server will close the socket after transmitting the response.
      boost::asio::streambuf request;
      std::ostream request_stream(&request);


      std::stringstream post_body;
      post_body << "{\"challenge\": \"" << challenge << "\", "
                << "\"timestamp\":  \"" << std::to_string(time) << "\", "
                << "\"company_id\": \"" << company_id_ << "\", "
                << "\"unique_id\": \"" << unique_id_ << "\", "
                << "\"signature\": \"" << signature << "\""
                << "}\r\n";
      std::string post_body_str = post_body.str();
      LOG(INFO) << " JSON " << std::endl << post_body_str;

      request_stream << "POST "
                     << "/api/v1/auth/challenge/response"
                     << " HTTP/1.1\r\n";
      request_stream << "Host: " << connection_settings_.api_host << ":80\r\n";
      request_stream << "Content-Type: application/json\r\n";
      request_stream << "Content-Length: " << post_body_str.size()
                     << "\r\n";
      request_stream << "Accept: */*\r\n";
      request_stream << "Connection: close\r\n";
      request_stream << "Cache-Control: no-cache\r\n";
      request_stream << "\r\n";
      request_stream << post_body_str;


      // Send the request.
      boost::asio::write(socket, request);

      // Read the response status line. The response streambuf will
      // automatically grow to accommodate the entire line. The growth may be
      // limited by passing a maximum size to the streambuf constructor.
      boost::asio::streambuf response;
      boost::asio::read_until(socket, response, "\r\n");

      // Check that response is OK.
      std::istream response_stream(&response);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);
      if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        LOG(ERROR) << "Invalid HTTP response to challenge response.";
        return false;
      }

      switch(status_code) {
        case 200:
          VLOG(1) << "Received polaris authentication token.";
          break;
        case 403:
          LOG(ERROR) << "Received an authentication forbidden (403) response "
                        "from Polaris. Please check challenge.";
          return false;
        default:
          LOG(ERROR) << "Authentication challenge response returned with status code "
                     << status_code << ".";
          return false;
      }

      // Process the response headers.
      boost::asio::read_until(socket, response, "\r\n\r\n");
      std::string header;
      VLOG(3) << "Response:";
      while (std::getline(response_stream, header) && header != "\r") {
        VLOG(3) << header;
      }

      // Get the entire reply from the socket.
      std::stringstream reply_body;
      if (response.size() > 0) {
        reply_body << &response;
      }
      boost::system::error_code error;
      while (boost::asio::read(socket, response,
                               boost::asio::transfer_at_least(1), error)) {
        reply_body << &response;
      }
      if (error != boost::asio::error::eof) {
        LOG(ERROR) << "Error occured while receiving auth token: "
                   << error.message();
        return false;
      }

      try {
        // Parse json.
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(reply_body, pt);
        api_token_.access_token = pt.get<std::string>("access_token");
        api_token_.expires_in = pt.get<double>("expires_in");
        api_token_.issued_at = pt.get<double>("issued_at");
        VLOG(3) << "Access token: " << api_token_.access_token;
        VLOG(3) << "Expires in: " << std::fixed << api_token_.expires_in;
        VLOG(3) << "Issued at: " << std::fixed << api_token_.issued_at;

      } catch (std::exception &e) {
        LOG(ERROR) << "Exception in parsing auth token response: " << e.what();
        return false;
      }

      return true;
    } catch (std::exception &e) {
      LOG(ERROR) << "Exception: " << e.what();
    }
    return false;
  }

  // Send authentication if connection is successful, otherwise reconnect..
  void HandleConnect(const boost::system::error_code &err) {
    if (!err) {
      LOG(INFO) << "Connected to Point One Navigation's Polaris Service.";
      // The connection was successful. Send the request.
      SendAuth();
    } else {
      connected_ = false;
      connection_active_ = false;
      LOG(ERROR) << "Failed to connect to tcp://" << connection_settings_.host
                 << ":" << connection_settings_.port << ".";
      ScheduleReconnect();
    }
  }

  // Sends authentication to the tcp server.  Result is bound to async callback
  // HandleAuthTokenWrite.
  void SendAuth() {
    VLOG(1) << "Authenticating with service using auth token.";
    if (api_token_.access_token.empty()) {
      std::string challenge  = RequestChallenge();
      LOG(INFO) << " GOT CHALLENGE " << challenge;
      uint64_t time = std::time(0);
      std::string signature = BuildChallengeSignature(challenge, time);
      if (!GetAuthToken(challenge, time, signature)) {
        LOG(WARNING) << "Failed to get the auth token exchange!";
        return;
      }
    }

    AuthRequest request(api_token_.access_token);
    auto buf = std::make_shared<std::vector<uint8_t>>(request.GetSize());
    request.Serialize(buf->data());

    std::unique_lock<std::recursive_mutex> guard(lock_);
    boost::asio::async_write(
        socket_, boost::asio::buffer(*buf),
        boost::bind(&PolarisAsioEmbeddedClient::HandleAuthTokenWrite, this, buf,
                    boost::asio::placeholders::error));
  }

  // Currently Polaris Service does not respond with a 403 when an invalid token
  // is written.  Instead the connection is terminated by the server. That means
  // this function will not return an error code if a bad token is written.
  void HandleAuthTokenWrite(std::shared_ptr<std::vector<uint8_t>> buf,
                            const boost::system::error_code &err) {
    if (!err) {
      LOG(INFO) << "Polaris authentication succeeded.";
      connected_ = true;
      RunStream();
    } else {
      connected_ = false;
      connection_active_ = false;
      LOG(ERROR) << "Polaris authentication failed.";
      api_token_ = PolarisAuthToken();  // Reset token.
      ScheduleReconnect();
    }
  }

  // Authentication was a success, start streaming position and reading from
  // socket.
  void RunStream() {
    LOG(INFO) << "Starting Polaris Client.";

    if (stream_selection_source_ == StreamSelectionSource::POSITION) {
      pos_timer_.expires_from_now(boost::posix_time::milliseconds(0));
      pos_timer_.async_wait(boost::bind(&PolarisAsioEmbeddedClient::PositionTimer, this,
                                        boost::asio::placeholders::error));
    }
    VLOG(2) << "Scheduling initial socket read.";
    ScheduleAsyncReadWithTimeout();
  }

  // Handle reading of bytes.
  void HandleSocketRead(const boost::system::error_code err, size_t bytes) {
    if (err) {
      connected_ = false;
      connection_active_ = false;
      LOG(ERROR) << "Error reading bytes.";
      return;
    }

    if (bytes && polaris_bytes_received_callback_) {
      VLOG(4) << "Received " << bytes << " bytes from Polaris service.";
      LOG_IF(INFO, !connection_active_)
          << "Connected and receiving corrections.";
      connection_active_ = true;
      polaris_bytes_received_callback_(buf_, bytes);
    }

    ScheduleAsyncReadWithTimeout();
  }

  void ScheduleAsyncReadWithTimeout() {
    socket_timer_.expires_from_now(
        boost::posix_time::milliseconds(SOCKET_TIMEOUT_MS));
    socket_timer_.async_wait(
        boost::bind(&PolarisAsioEmbeddedClient::HandleSocketTimeout, this,
                    boost::asio::placeholders::error));

    // Read more bytes if they are available.
    std::unique_lock<std::recursive_mutex> guard(lock_);
    boost::asio::async_read(
        socket_, boost::asio::buffer(buf_), boost::asio::transfer_at_least(1),
        boost::bind(&PolarisAsioEmbeddedClient::HandleSocketRead, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  // Schedules reconnection if no reconnections are pending.
  // Will wait wait_ms milliseconds before executing connection.
  void ScheduleReconnect(int wait_ms = 5000) {
    if (reconnect_timer_.expires_from_now() <
            boost::posix_time::milliseconds(0) ||
        !reconnect_set_) {
      pos_timer_.cancel();
      reconnect_timer_.expires_from_now(
          boost::posix_time::milliseconds(wait_ms));
      reconnect_timer_.async_wait(
          boost::bind(&PolarisAsioEmbeddedClient::HandleReconnectTimeout, this,
                      boost::asio::placeholders::error));
      reconnect_set_ = true;
    } else {
      VLOG(4) << "Reconnect already scheduled in: "
              << reconnect_timer_.expires_from_now().seconds() << " seconds";
    }
  }

  // Called when reconnection timeout limit is hit triggering a reconnect if not
  // already aborted.
  void HandleReconnectTimeout(const boost::system::error_code &e) {
    if (e != boost::asio::error::operation_aborted) {
      VLOG(1) << "Reconnecting...";
      Connect();
    }
  }

  // Called when socket fails to read for a given time.
  void HandleSocketTimeout(const boost::system::error_code &e) {
    if (e != boost::asio::error::operation_aborted && connection_active_) {
      reconnect_timer_.cancel();
      connection_active_ = false;
      LOG(WARNING) << "Socket read timed out. Scheduling reconnect.";
      ScheduleReconnect(0);
    }
  }

  // Sets the position of the client for future broadcasting.
  void HandleSetPosition(double a, double b, double c) {
    ecef_pos_ = PositionEcef(a, b, c);

    if (stream_selection_source_ != StreamSelectionSource::POSITION) {
      VLOG(1) << "Scheduling first position update.";
      stream_selection_source_ = StreamSelectionSource::POSITION;
      if (connected_) {
        pos_timer_.expires_from_now(boost::posix_time::milliseconds(0));
        pos_timer_.async_wait(boost::bind(&PolarisAsioEmbeddedClient::PositionTimer,
                                          this,
                                          boost::asio::placeholders::error));
      }
    }
  }

  // Timer that sends position to polaris every connection_settings_.interval_ms
  // milliseconds. If the Polaris Server does not receive a message from the
  // client after 5 seconds it will close the socket.  Sending position
  // functions as a connection heartbeat.
  void PositionTimer(const boost::system::error_code &err) {
    if (err == boost::asio::error::operation_aborted) {
      VLOG(4) << "Position callback canceled";
      return;
    }
    SendPosition();
    VLOG(4) << "Scheduling position timer.";
    pos_timer_.expires_from_now(
        boost::posix_time::milliseconds(connection_settings_.interval_ms));
    pos_timer_.async_wait(boost::bind(&PolarisAsioEmbeddedClient::PositionTimer, this,
                                      boost::asio::placeholders::error));
  }

  // Encodes a position message and sends it via the Polaris Client.
  void SendPosition() {
    VLOG(2) << "Sending position update.";

    std::unique_ptr<PolarisRequest> request;

    request.reset(new PositionEcefRequest(ecef_pos_));
    VLOG(2) << "Position is ECEF: X:" << ecef_pos_.pos[0] <<" Y:" << ecef_pos_.pos[1] << " Z:" << ecef_pos_.pos[2];


    auto buf = std::make_shared<std::vector<uint8_t>>(request->GetSize());
    request->Serialize(buf->data());

    std::unique_lock<std::recursive_mutex> guard(lock_);
    boost::asio::async_write(
        socket_, boost::asio::buffer(*buf),
        boost::bind(&PolarisAsioEmbeddedClient::HandleResponse, this,
                    boost::asio::placeholders::error, "position update"));
  }


  // Schedule a service reconnect if a request fails to send.
  void HandleResponse(const boost::system::error_code &err,
                      const std::string &data_type = "data") {
    if (!err) {
      VLOG(3) << "Successfully sent " << data_type << " to service.";
    } else {
      LOG(ERROR) << "Error sending " << data_type
                 << " to service. Bad connection or authentication? Scheduling "
                    "reconnect.";
      ScheduleReconnect();
    }
  }

  enum class StreamSelectionSource { NONE, POSITION };

  // Synchronization for the socket.
  std::recursive_mutex lock_;

  // Settings for reaching polaris services.
  const PolarisConnectionSettings connection_settings_;

  // The signing secret for signing challenge responses
  std::string signing_secret_;

  // The issued token after authenticated with Polaris
  PolarisAuthToken api_token_;

  // The asio event loop service.
  boost::asio::io_service &io_service_;

  // Boost tcp endpoint resolver.
  boost::asio::ip::tcp::resolver resolver_;

  // The resolved endpoint to which to connect.
  boost::asio::ip::tcp::endpoint endpoint_;

  // The TCP socket that gets created.
  boost::asio::ip::tcp::socket socket_;

  // The TCP read buffer.
  uint8_t buf_[BUF_SIZE];

  // A callback to receive bytes of BUF_SIZE when read. Not guaranteed to be
  // framed.
  std::function<void(uint8_t *, uint16_t)> polaris_bytes_received_callback_;

  // The unique_id and company_id are required to authenticate a device issued by a paritcular
  // company. Often the unique_id is a units permanent serial number.
  std::string unique_id_;
  std::string company_id_;

  // A deadline time for resending postion.
  boost::asio::deadline_timer pos_timer_;

  // A timer for connecting retries.
  boost::asio::deadline_timer reconnect_timer_;

  // A timer for managing read timeouts.
  boost::asio::deadline_timer socket_timer_;

  // Tracks whether the reconnect_timer_ has ever been initiated.
  bool reconnect_set_ = false;

  // Whether or not the system is connected and authenticated.
  bool connected_ = false;

  // Whether or not the system is receiving data.
  bool connection_active_ = false;

  // The typeof input used to select a data stream.
  StreamSelectionSource stream_selection_source_ = StreamSelectionSource::NONE;

  // Last position in ECEF XYZ meters.
  PositionEcef ecef_pos_;


};

}  // namespace polaris
}  // namespace point_one
