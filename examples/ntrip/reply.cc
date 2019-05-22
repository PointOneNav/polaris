// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include "reply.h"
#include <boost/lexical_cast.hpp>
#include <string>

namespace ntrip {

namespace status_strings {

const std::string ok = "HTTP/1.0 200 OK\r\n";
const std::string icy_ok = "ICY 200 OK\r\n";
const std::string source_table_ok = "SOURCETABLE 200 OK\r\n";
const std::string bad_request = "HTTP/1.0 400 Bad Request\r\n";
const std::string not_found = "HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
    "HTTP/1.0 500 Internal Server Error\r\n";

boost::asio::const_buffer to_buffer(reply::status_type status) {
  switch (status) {
    case reply::ok:
      return boost::asio::buffer(ok);
    case reply::icy_ok:
      return boost::asio::buffer(icy_ok);
    case reply::source_table_ok:
      return boost::asio::buffer(source_table_ok);
    case reply::bad_request:
      return boost::asio::buffer(bad_request);
    case reply::not_found:
      return boost::asio::buffer(not_found);
    case reply::internal_server_error:
      return boost::asio::buffer(internal_server_error);
    default:
      return boost::asio::buffer(internal_server_error);
  }
}

}  // namespace status_strings

namespace misc_strings {

const char name_value_separator[] = {':', ' '};
const char crlf[] = {'\r', '\n'};

}  // namespace misc_strings

std::vector<boost::asio::const_buffer> reply::to_buffers() {
  std::vector<boost::asio::const_buffer> buffers;
  buffers.push_back(status_strings::to_buffer(status));
  for (std::size_t i = 0; i < headers.size(); ++i) {
    header& h = headers[i];
    buffers.push_back(boost::asio::buffer(h.name));
    buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
    buffers.push_back(boost::asio::buffer(h.value));
    buffers.push_back(boost::asio::buffer(misc_strings::crlf));
  }
  buffers.push_back(boost::asio::buffer(misc_strings::crlf));
  buffers.push_back(boost::asio::buffer(content));
  return buffers;
}

namespace stock_replies {

const char ok[] = "";
const char icy_ok[] = "";
const char source_table_ok[] = "";
const char bad_request[] =
    "<html>"
    "<head><title>Bad Request</title></head>"
    "<body><h1>400 Bad Request</h1></body>"
    "</html>";
const char not_found[] =
    "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>";
const char internal_server_error[] =
    "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>";

std::string to_string(reply::status_type status) {
  switch (status) {
    case reply::ok:
      return ok;
    case reply::icy_ok:
      return ok;
    case reply::source_table_ok:
      return ok;
    case reply::bad_request:
      return bad_request;
    case reply::not_found:
      return not_found;
    case reply::internal_server_error:
      return internal_server_error;
    default:
      return internal_server_error;
  }
}

}  // namespace stock_replies

reply reply::stock_reply(reply::status_type status) {
  reply rep;
  rep.status = status;
  rep.content = stock_replies::to_string(status);
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = "text/html";
  return rep;
}

}  // namespace ntrip