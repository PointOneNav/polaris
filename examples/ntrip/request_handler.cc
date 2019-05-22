// Example ntrip server using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include "request_handler.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

#include "glog/logging.h"
#include "mime_types.h"
#include "reply.h"
#include "request.h"

namespace ntrip {

request_handler::request_handler(const std::string& doc_root)
    : doc_root_(doc_root) {}

void request_handler::handle_source_table_request(reply& rep) {
  rep.status = reply::source_table_ok;
  // Thu, 04 April 2019 19:31:18 UTC
  std::stringstream content_ss;
  content_ss << "STR;Polaris;PointOneNavigation;RTCM "
                "3.3;1010(1),1012(1),1006(30);2;GPS+GLO;PointOne;USA;32.56;-"
                "127.63;1;0;P1;none;N;N;4096;;\r\n";
  content_ss << "ENDSOURCETABLE\r\n";
  content_ss << "\r\n";

  std::stringstream date_ss;
  boost::posix_time::ptime t =
      boost::posix_time::microsec_clock::universal_time();
  date_ss << boost::posix_time::to_iso_extended_string(t) << "Z\n";

  rep.content = content_ss.str();
  rep.headers.resize(4);
  rep.headers[0].name = "Server";
  rep.headers[0].value = "PointOne Navigation Polaris NTRIP Proxy Example";
  rep.headers[1].name = "Date";
  rep.headers[1].value = date_ss.str();
  rep.headers[2].name = "Content-Length";
  rep.headers[2].value = boost::lexical_cast<std::string>(rep.content.size());
  rep.headers[3].name = "Content-Type";
  rep.headers[3].value = "text/plain";
}

void request_handler::handle_mountpoint_request(const std::string& mount_point,
                                                const std::string& ntrip_gga,
                                                reply& rep) {
  rep = reply::stock_reply(reply::icy_ok);
  rep.mount_point = mount_point;
  rep.ntrip_gga = ntrip_gga;
}

void request_handler::handle_normal_http_request(const request& req,
                                                 reply& rep) {
  // Decode url to path.
  std::string request_path;
  if (!url_decode(req.uri, request_path)) {
    LOG(INFO) << "Bad request";
    rep = reply::stock_reply(reply::bad_request);
    return;
  }

  // Request path must be absolute and not contain "..".
  if (request_path.empty() || request_path[0] != '/' ||
      request_path.find("..") != std::string::npos) {
    LOG(INFO) << "Bad request: " << request_path;
    rep = reply::stock_reply(reply::bad_request);
    return;
  }
  // If path ends in slash (i.e. is a directory) then add "index.html".
  if (request_path[request_path.size() - 1] == '/') {
    request_path += "index.html";
  }

  // Determine the file extension.
  std::size_t last_slash_pos = request_path.find_last_of("/");
  std::size_t last_dot_pos = request_path.find_last_of(".");
  std::string extension;
  if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
    extension = request_path.substr(last_dot_pos + 1);
  }

  // Open the file to send back.
  std::string full_path = doc_root_ + request_path;
  std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
  if (!is) {
    LOG(INFO) << full_path;
    rep = reply::stock_reply(reply::not_found);
    return;
  }

  // Fill out the reply to be sent to the client.
  rep.status = reply::ok;
  char buf[512];
  while (is.read(buf, sizeof(buf)).gcount() > 0)
    rep.content.append(buf, is.gcount());
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = extension_to_type(extension);
}

void request_handler::handle_request(const request& req, reply& rep) {
  bool ntrip_user_agent = false;
  std::string ntrip_gga;
  for (const auto& header : req.headers) {
    std::string data = header.name;
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    std::string value = header.value;
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    if (data == "user-agent" && boost::istarts_with(value, "ntrip")) {
      LOG(INFO) << value;
      ntrip_user_agent = true;
    }
    if (data == "ntrip-gga") {
      ntrip_gga = header.value;
    }
  }

  LOG(INFO) << ntrip_user_agent;
  if (!ntrip_user_agent) {
    handle_normal_http_request(req, rep);
  } else if (mountpoints_.find(req.uri) != mountpoints_.end()) {
    handle_mountpoint_request(req.uri, ntrip_gga, rep);
  } else {
    LOG(INFO) << req.uri;
    handle_source_table_request(rep);
  }
}

bool request_handler::url_decode(const std::string& in, std::string& out) {
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else if (in[i] == '+') {
      out += ' ';
    } else {
      out += in[i];
    }
  }
  return true;
}

}  // namespace ntrip