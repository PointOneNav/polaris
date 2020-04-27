// Copyright (C) Point One Navigation - All Rights Reserved
// Written by Jason Moran <jason@pointonenav.com>, March 2019

// Client for connecting to Polaris RTCM corrections service.
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

#include "glog/logging.h"

#include "polaris.h"

namespace point_one {
namespace polaris {

namespace {
constexpr int SOCKET_TIMEOUT_MS = 7000;
}  // namespace

class PolarisAsioClient {
 public:
  // How long to wait before sending a new position update.  Polaris
  // Service expects receiving a message within 5 seconds.
  static constexpr int DEFAULT_POSITION_SEND_INTERVAL_MSECS = 3000;

  PolarisAsioClient(boost::asio::io_service &io_service,
                    const std::string &api_key,
                    const std::string &tracking_id = "",
                    const PolarisConnectionSettings &connection_settings =
                        DEFAULT_CONNECTION_SETTINGS)
      : connection_settings_(connection_settings),
        api_key_(api_key),
        io_service_(io_service),
        resolver_(io_service),
        socket_(io_service),
        tracking_id_(tracking_id),
        pos_timer_(io_service),
        socket_timer_(io_service,
                      boost::posix_time::milliseconds(SOCKET_TIMEOUT_MS)),
        reconnect_timer_(io_service, boost::posix_time::milliseconds(
                                         connection_settings.interval_ms)) {}

  ~PolarisAsioClient() {
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
    LOG(INFO) << "Attempting to open socket tcp://" << connection_settings_.host
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
                          boost::bind(&PolarisAsioClient::HandleConnect, this,
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
    io_service_.post(boost::bind(&PolarisAsioClient::HandleSetPosition, this,
                                 x_meters, y_meters, z_meters, true));
  }

  // Send a position to Polaris, potentially updating the resulting stream.
  // Store the position for resending until position is updated.  Latitude,
  // longitude, altitude variant.
  void SetPositionLLA(double lat_deg, double lon_deg, double alt_m) {
    io_service_.post(boost::bind(&PolarisAsioClient::HandleSetPosition, this,
                                 lat_deg, lon_deg, alt_m, false));
  }

 private:
  // The size of the read buffer.
  static constexpr int BUF_SIZE = 1024;

  bool RequestToken() {
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
      post_body << "{\"grant_type\": \"authorization_code\", "
                     << "\"token_type\":  \"Bearer\", "
                     << "\"tracking_id\": \"" << tracking_id_ << "\", "
                     << "\"authorization_code\": \"" << api_key_ << "\"}\r\n";
      std::string post_body_str = post_body.str();

      request_stream << "POST "
                     << "/api/v1/auth/token"
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
        LOG(ERROR) << "Invalid HTTP response\n";
        return false;
      }

      switch(status_code) {
        case 200:
          LOG(INFO) << "Succesfully received polaris authentication token.";
          break;
        case 403:
          LOG(ERROR) << "Received a 403 from Polaris, check your API key.";
          return false;
        default:
          LOG(ERROR) << "Response returned with status code " << status_code;
          return false;
      }

      // Process the response headers.
      boost::asio::read_until(socket, response, "\r\n\r\n");
      std::string header;
      while (std::getline(response_stream, header) && header != "\r") {
        VLOG(2) << header;
      }

      // Parse json.
      boost::property_tree::ptree pt;
      if (response.size() > 0) {
        boost::property_tree::read_json(response_stream, pt);
      }

      try {
        api_token_.access_token = pt.get<std::string>("access_token");
        api_token_.expires_in = pt.get<double>("expires_in");
        api_token_.issued_at = pt.get<double>("issued_at");
      } catch (std::exception &e) {
        LOG(ERROR) << "Exception: " << e.what();
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
      LOG(INFO) << "Connecting to Point One Navigation's Polaris Service.";
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
    LOG(INFO) << "Authenticating with service using auth token.";
    if (api_token_.access_token.empty()) {
      if (!RequestToken()) {
        // Token request failures are fatal - probably an invalid API key. Don't
        // schedule a reconnect.
        return;
      }
    }
    AuthRequest request(api_token_.access_token);

    auto buf = std::make_shared<std::vector<uint8_t>>(request.GetSize());
    request.Serialize(buf->data());

    std::unique_lock<std::recursive_mutex> guard(lock_);
    boost::asio::async_write(
        socket_, boost::asio::buffer(*buf),
        boost::bind(&PolarisAsioClient::HandleAuthTokenWrite, this, buf,
                    boost::asio::placeholders::error));
  }

  // Currently Polaris Service does not respond with a 403 when an invalid token
  // is written.  Instead the connection is terminated by the server. That means
  // this function will not return an error code if a bad token is written.
  void HandleAuthTokenWrite(std::shared_ptr<std::vector<uint8_t>> buf,
                            const boost::system::error_code &err) {
    if (!err) {
      LOG(INFO) << "Authentication succeeded.";
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
    pos_timer_.expires_from_now(boost::posix_time::milliseconds(0));
    pos_timer_.async_wait(boost::bind(&PolarisAsioClient::PositionTimer, this,
                                      boost::asio::placeholders::error));

    VLOG(3) << "Starting socket read.";
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
      LOG_IF(INFO, !connection_active_) << "Connected and receiving corrections.";
      connection_active_ = true;
      polaris_bytes_received_callback_(buf_, bytes);
    }

    ScheduleAsyncReadWithTimeout();
  }

  void ScheduleAsyncReadWithTimeout() {
    socket_timer_.expires_from_now(
        boost::posix_time::milliseconds(SOCKET_TIMEOUT_MS));
    socket_timer_.async_wait(
        boost::bind(&PolarisAsioClient::HandleSocketTimeout, this,
                    boost::asio::placeholders::error));

    // Read more bytes if they are available.
    std::unique_lock<std::recursive_mutex> guard(lock_);
    boost::asio::async_read(
        socket_, boost::asio::buffer(buf_), boost::asio::transfer_at_least(1),
        boost::bind(&PolarisAsioClient::HandleSocketRead, this,
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
          boost::bind(&PolarisAsioClient::HandleReconnectTimeout, this,
                      boost::asio::placeholders::error));
      reconnect_set_ = true;
    } else {
      VLOG(4) << "Reconnect already scheduled in: "
              << reconnect_timer_.expires_from_now().seconds();
    }
  }

  // Called when reconnection timeout limit is hit triggering a reconnect if not
  // already aborted.
  void HandleReconnectTimeout(const boost::system::error_code &e) {
    if (e != boost::asio::error::operation_aborted) {
      VLOG(4) << "Reconnecting...";
      Connect();
    }
  }

  // Called when socket fails to read for a given time.
  void HandleSocketTimeout(const boost::system::error_code &e) {
    if (e != boost::asio::error::operation_aborted && connection_active_) {
      reconnect_timer_.cancel();
      connection_active_ = false;
      LOG(WARNING) << "Socket timedout on read, scheduling reconnect.";
      ScheduleReconnect(0);
    }
  }

  // Sets the position of the client for future broadcasting.
  void HandleSetPosition(double a, double b, double c, bool is_ecef) {
    pos_is_ecef_ = is_ecef;
    if (is_ecef) {
      ecef_pos_ = PositionEcef(a, b, c);
    } else {
      lla_pos_ = PositionLla(a, b, c);
    }
  }

  // Timer that sends position to polaris every connection_settings_.interval_ms
  // milliseconds. If the Polaris Server does not receive a message from the
  // client after 5 seconds it will close the socket.  Sending position
  // functions as a connection heartbeat.
  void PositionTimer(const boost::system::error_code &err) {
    if (err == boost::asio::error::operation_aborted) {
      VLOG(6) << "Position callback canceled";
      return;
    }
    SendPosition();
    VLOG(6) << "Scheduling position timer.";
    pos_timer_.expires_from_now(
        boost::posix_time::milliseconds(connection_settings_.interval_ms));
    pos_timer_.async_wait(boost::bind(&PolarisAsioClient::PositionTimer, this,
                                      boost::asio::placeholders::error));
  }

  // Encodes a position message and sends it via the Polaris Client.
  void SendPosition() {
    VLOG(4) << "Sending position";

    std::unique_ptr<PolarisRequest> request;
    if (pos_is_ecef_) {
      request.reset(new PositionEcefRequest(ecef_pos_));
    } else {
      request.reset(new PositionLlaRequest(lla_pos_));
    }

    auto buf = std::make_shared<std::vector<uint8_t>>(request->GetSize());
    request->Serialize(buf->data());

    std::unique_lock<std::recursive_mutex> guard(lock_);
    boost::asio::async_write(
        socket_, boost::asio::buffer(*buf),
        boost::bind(&PolarisAsioClient::HandlePositionWrite, this, buf,
                    boost::asio::placeholders::error));
    VLOG(6) << "Scheduled position";
  }

  // Handles writing position to Polaris Service. Attempts reconnect on failure.
  void HandlePositionWrite(std::shared_ptr<std::vector<uint8_t>> buf,
                           const boost::system::error_code &err) {
    if (!err) {
      VLOG(5) << "Successfully sent position to service.";
    } else {
      LOG(ERROR) << "Error sending position to service. Bad connection or "
                    "authentication?";
      ScheduleReconnect();
    }
  }

  // Synchronization for the socket.
  std::recursive_mutex lock_;

  // Settings for reaching polaris services.
  const PolarisConnectionSettings connection_settings_;

  // The serial number of the device for connecting with Polaris.
  std::string api_key_;

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

  // Optionally a unique id that can be used to aid in debugging when sharing an api key
  // with multiple clients.
  std::string tracking_id_;

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

  // Whether to send last position as ecef or meters.
  bool pos_is_ecef_ = true;

  // Last position in ECEF XYZ meters.
  PositionEcef ecef_pos_;

  // Last position in Latitude(deg), Longitude(deg), alt(m).
  PositionLla lla_pos_;
};

}  // namespace polaris
}  // namespace point_one
