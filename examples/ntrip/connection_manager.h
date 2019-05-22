// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <boost/noncopyable.hpp>
#include <map>
#include <set>
#include <unordered_set>
#include "connection.h"

namespace ntrip {
/// Manages open connections so that they may be cleanly stopped when the server
/// needs to shut down.
class connection_manager : private boost::noncopyable {
 public:
  /// Add the specified connection to the manager and start it.
  void start(connection_ptr c);

  void upgrade_connection(connection_ptr c, const std::string &mount_point);

  void broadcast(const std::string &mount_point, const std::string &data);

  void SetGpggaCallback(std::function<void(const std::string &)> callback);

  void SetGpgga(const std::string &);

  /// Stop the specified connection.
  void stop(connection_ptr c);

  /// Stop all connections.
  void stop_all();

 private:
  /// The managed connections.
  std::set<connection_ptr> connections_;

  std::map<std::string, std::set<connection_ptr>> mounted_connections_;

  std::function<void(const std::string &)> gpgga_callback_;
};

}  // namespace ntrip
