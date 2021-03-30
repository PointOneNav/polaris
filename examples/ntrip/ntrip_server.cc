// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include "ntrip_server.h"
#include <signal.h>
#include <boost/bind.hpp>
namespace ntrip {

server::server(boost::asio::io_service& io_service, const std::string& address,
               const std::string& port, const std::string& doc_root)
    : io_service_(io_service),
      signals_(io_service_),
      acceptor_(io_service_),
      connection_manager_(),
      new_connection_(),
      request_handler_(doc_root) {
  // Register to handle the signals that indicate when the server should exit.
  // It is safe to register for the same signal multiple times in a program,
  // provided all registration for the specified signal is made through Asio.
  /*
  signals_.add(SIGINT);
  signals_.add(SIGTERM);
#if defined(SIGQUIT)
  signals_.add(SIGQUIT);
#endif  // defined(SIGQUIT)
  signals_.async_wait(boost::bind(&server::handle_stop, this));
  */

  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
}

void server::start_accept() {
  new_connection_.reset(
      new connection(io_service_, connection_manager_, request_handler_));
  acceptor_.async_accept(new_connection_->socket(),
                         boost::bind(&server::handle_accept, this,
                                     boost::asio::placeholders::error));
}

void server::SetGpggaCallback(std::function<void(const std::string &)> callback) {
    connection_manager_.SetGpggaCallback(callback);
}

void server::broadcast(const std::string& mount_point, const uint8_t* data,
                       size_t len) {
  connection_manager_.broadcast(mount_point,
                                std::string((const char*)data, len));
}

void server::handle_accept(const boost::system::error_code& e) {
  // Check whether the server was stopped by a signal before this completion
  // handler had a chance to run.
  if (!acceptor_.is_open()) {
    return;
  }

  if (!e) {
    connection_manager_.start(new_connection_);
  }

  start_accept();
}

void server::handle_stop() {
  // The server is stopped by cancelling all outstanding asynchronous
  // operations. Once all operations have finished the io_service::run() call
  // will exit.
  acceptor_.close();
  connection_manager_.stop_all();
}

}  // namespace ntrip
