// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include "connection.h"
#include "connection_manager.h"
#include "request_handler.h"

namespace ntrip {

/// The top-level class of the HTTP server.
class server {
 public:
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(boost::asio::io_service& io_service,
                  const std::string& address, const std::string& port,
                  const std::string& doc_root);

  void broadcast(const std::string& mount_point, uint8_t* data, size_t len);

  void SetGpggaCallback(std::function<void(const std::string &)> callback);

  void stop();

 private:
  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service &io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The connection manager which owns all live connections.
  connection_manager connection_manager_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  /// The handler for all incoming requests.
  request_handler request_handler_;
};

}  // namespace once