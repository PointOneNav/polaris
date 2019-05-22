// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <boost/noncopyable.hpp>
#include <string>
#include <unordered_set>

namespace ntrip {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler : private boost::noncopyable {
 public:
  /// Construct with a directory containing files to be served.
  explicit request_handler(const std::string& doc_root);

  void handle_source_table_request(reply& rep);

  void handle_mountpoint_request(const std::string& endpoint,
                                 const std::string& ntrip_gga, reply& rep);

  void handle_normal_http_request(const request& req, reply& rep);

  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);

 private:
  /// The directory containing the files to be served.
  std::string doc_root_;

  std::unordered_set<std::string> mountpoints_ = {"/Polaris"};

  /// Perform URL-decoding on a string. Returns false if the encoding was
  /// invalid.
  static bool url_decode(const std::string& in, std::string& out);
};

}  // namespace ntrip
