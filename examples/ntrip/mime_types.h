// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <string>

namespace ntrip {

/// Convert a file extension into a MIME type.
std::string extension_to_type(const std::string& extension);

}  // namespace ntrip