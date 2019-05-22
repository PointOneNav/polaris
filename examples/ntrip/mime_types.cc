// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include "mime_types.h"

namespace ntrip {

struct mapping {
  const char* extension;
  const char* mime_type;
} mappings[] = {
    {"gif", "image/gif"},  {"htm", "text/html"}, {"html", "text/html"},
    {"jpg", "image/jpeg"}, {"png", "image/png"}, {0, 0}  // Marks end of list.
};

std::string extension_to_type(const std::string& extension) {
  for (mapping* m = mappings; m->extension; ++m) {
    if (m->extension == extension) {
      return m->mime_type;
    }
  }

  return "text/plain";
}

}  // namespace ntrip