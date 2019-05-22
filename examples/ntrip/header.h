// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <string>

namespace ntrip {

struct header {
  std::string name;
  std::string value;
};

}  // namespace ntrip
