// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include "connection.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>
#include <vector>
#include "connection_manager.h"
#include "request_handler.h"

#include "glog/logging.h"

namespace ntrip {

connection::connection(boost::asio::io_service& io_service,
                       connection_manager& manager, request_handler& handler)
    : socket_(io_service),
      connection_manager_(manager),
      request_handler_(handler),
      connection_upgraded_(false) {}

boost::asio::ip::tcp::socket& connection::socket() { return socket_; }

void connection::start() {
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      boost::bind(&connection::handle_read, shared_from_this(),
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

void connection::async_send(const std::string& data) {
  broadcast_buffer_ = data;
  boost::asio::async_write(
      socket_, boost::asio::buffer(data),
      boost::bind(&connection::handle_write, shared_from_this(),
                  boost::asio::placeholders::error));
}

void connection::stop() { socket_.close(); }

void connection::handle_read(const boost::system::error_code& e,
                             std::size_t bytes_transferred) {
  if (!e) {
    VLOG(4) << std::string(buffer_.data(), buffer_.size());

    boost::tribool result;
    if (boost::istarts_with(buffer_, "$GPGGA") ||
        boost::istarts_with(buffer_, "$INGGA")) {
      if (connection_upgraded_) {
        connection_manager_.SetGpgga(
            std::string(buffer_.data(), buffer_.size()));
        socket_.async_read_some(
            boost::asio::buffer(buffer_),
            boost::bind(&connection::handle_read, shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
      } else {
        reply_ = reply::stock_reply(reply::bad_request);
        boost::asio::async_write(
            socket_, reply_.to_buffers(),
            boost::bind(&connection::handle_write, shared_from_this(),
                        boost::asio::placeholders::error));
      }

    } else if (!connection_upgraded_) {
      boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
          request_, buffer_.data(), buffer_.data() + bytes_transferred);
    } else {
      socket_.async_read_some(
          boost::asio::buffer(buffer_),
          boost::bind(&connection::handle_read, shared_from_this(),
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    }

    if (result) {
      request_handler_.handle_request(request_, reply_);
      if (reply_.status == reply::icy_ok) {
        LOG(INFO) << "Connection Upgraded!";
        connection_upgraded_ = true;
        mount_point_ = reply_.mount_point;
        if (!reply_.ntrip_gga.empty()) {
          connection_manager_.SetGpgga(reply_.ntrip_gga);
        }
      }

      boost::asio::async_write(
          socket_, reply_.to_buffers(),
          boost::bind(&connection::handle_write, shared_from_this(),
                      boost::asio::placeholders::error));
    } else if (!result) {
      reply_ = reply::stock_reply(reply::bad_request);
      boost::asio::async_write(
          socket_, reply_.to_buffers(),
          boost::bind(&connection::handle_write, shared_from_this(),
                      boost::asio::placeholders::error));
    } else {
      socket_.async_read_some(
          boost::asio::buffer(buffer_),
          boost::bind(&connection::handle_read, shared_from_this(),
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    }
  } else if (e != boost::asio::error::operation_aborted) {
    LOG(ERROR) << "Read error: " << e;
    connection_manager_.stop(shared_from_this());
  }
}

void connection::handle_write(const boost::system::error_code& e) {
  if (!e) {
    if (connection_upgraded_) {
      connection_manager_.upgrade_connection(shared_from_this(), mount_point_);
      socket_.async_read_some(
          boost::asio::buffer(buffer_),
          boost::bind(&connection::handle_read, shared_from_this(),
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    } else {
      // Initiate graceful connection closure.
      LOG(INFO) << "Closing connection: " << connection_upgraded_;

      boost::system::error_code ignored_ec;
      socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }
  } else if (e != boost::asio::error::operation_aborted) {
    LOG(ERROR) << "Write error: " << e;
    connection_manager_.stop(shared_from_this());
  }
}

}  // namespace ntrip
