// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <string>
#include <vector>
#include "header.h"

namespace ntrip {

/// A request received from a client.
struct request {
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
};

}  // namespace ntrip
