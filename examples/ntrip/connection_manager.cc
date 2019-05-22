// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include "connection_manager.h"
#include <algorithm>
#include <boost/bind.hpp>

#include "glog/logging.h"

namespace ntrip {

void connection_manager::start(connection_ptr c) {
  connections_.insert(c);
  c->start();
}

void connection_manager::stop(connection_ptr c) {
  connections_.erase(c);
  if (mounted_connections_.find(c->mount_point()) !=
          mounted_connections_.end() &&
      mounted_connections_[c->mount_point()].find(c) !=
          mounted_connections_[c->mount_point()].end()) {
    mounted_connections_[c->mount_point()].erase(c);
  }
  c->stop();
  LOG(INFO) << "Connection closed";
}

void connection_manager::SetGpggaCallback(
    std::function<void(const std::string &)> callback) {
  gpgga_callback_ = callback;
}

void connection_manager::SetGpgga(const std::string &data) {
  if (gpgga_callback_) {
    gpgga_callback_(data);
  }
}

void connection_manager::broadcast(const std::string &mount_point,
                                   const std::string &data) {
  if (mounted_connections_.find(mount_point) == mounted_connections_.end()) {
    return;
  }
  VLOG(4) << "Broadcasting bytes: " << data.size();
  for (auto c : mounted_connections_[mount_point]) {
    c->async_send(data);
  }
}

void connection_manager::upgrade_connection(connection_ptr c,
                                            const std::string &mount_point) {
  mounted_connections_[c->mount_point()].insert(c);
}

void connection_manager::stop_all() {
  std::for_each(connections_.begin(), connections_.end(),
                boost::bind(&connection::stop, _1));
  connections_.clear();
}

}  // namespace ntrip
