// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "reply.h"
#include "request.h"
#include "request_handler.h"
#include "request_parser.h"

namespace ntrip {

class connection_manager;

/// Represents a single connection from a client.
class connection : public boost::enable_shared_from_this<connection>,
                   private boost::noncopyable {
 public:
  /// Construct a connection with the given io_service.
  explicit connection(boost::asio::io_service& io_service,
                      connection_manager& manager, request_handler& handler);

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

  void async_send(const std::string& data);

  /// Stop all asynchronous operations associated with the connection.
  void stop();

  std::string mount_point() { return mount_point_; }

 private:
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e,
                   std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e);

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// The manager for this connection.
  connection_manager& connection_manager_;

  /// The handler used to process the incoming request.
  request_handler& request_handler_;

  /// Buffer for incoming data.
  boost::array<char, 8192> buffer_;

  // Send buffer
  std::string broadcast_buffer_;

  /// The incoming request.
  request request_;

  /// The parser for the incoming request.
  request_parser request_parser_;

  /// The reply to be sent back to the client.
  reply reply_;

  bool connection_upgraded_;

  std::string mount_point_;
};

typedef boost::shared_ptr<connection> connection_ptr;

}  // namespace ntrip
