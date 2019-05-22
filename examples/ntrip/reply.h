// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include "header.h"

namespace ntrip {

/// A reply to be sent to a client.
struct reply {
  /// The status of the reply.
  enum status_type {
    ok = 200,
    icy_ok = 205,
    source_table_ok = 206,
    bad_request = 400,
    not_found = 404,
    internal_server_error = 500,
  } status;

  /// The headers to be included in the reply.
  std::vector<header> headers;

  /// The content to be sent in the reply.
  std::string content;

  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> to_buffers();

  /// Get a stock reply.
  static reply stock_reply(status_type status);

  std::string mount_point;

  std::string ntrip_gga;
};

}  // namespace ntrip
