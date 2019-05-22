// Example ntrip client using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

using boost::asio::ip::tcp;

class client {
 public:
  client(boost::asio::io_service& io_service, const std::string& server,
         const std::string& path)
      : resolver_(io_service), socket_(io_service), response_(4096) {
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    std::ostream request_stream(&request_);
    request_stream << "GET " << path << " HTTP/1.0\r\n";
    request_stream << "User-Agent: NTRIP"
                   << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Start an asynchronous resolve to translate the server and service names
    // into a list of endpoints.
    tcp::resolver::query query(server, std::to_string(2101));
    resolver_.async_resolve(query,
                            boost::bind(&client::handle_resolve, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::iterator));
  }

 private:
  void handle_resolve(const boost::system::error_code& err,
                      tcp::resolver::iterator endpoint_iterator) {
    if (!err) {
      // Attempt a connection to each endpoint in the list until we
      // successfully establish a connection.
      boost::asio::async_connect(socket_, endpoint_iterator,
                                 boost::bind(&client::handle_connect, this,
                                             boost::asio::placeholders::error));
    } else {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

  void handle_connect(const boost::system::error_code& err) {
    if (!err) {
      // The connection was successful. Send the request.
      boost::asio::async_write(socket_, request_,
                               boost::bind(&client::handle_write_request, this,
                                           boost::asio::placeholders::error));
    } else {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

  void handle_write_request(const boost::system::error_code& err) {
    if (!err) {
      // Read the response status line. The response_ streambuf will
      // automatically grow to accommodate the entire line. The growth may be
      // limited by passing a maximum size to the streambuf constructor.
      boost::asio::async_read_until(
          socket_, response_, "\r\n",
          boost::bind(&client::handle_read_status_line, this,
                      boost::asio::placeholders::error));
    } else {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

  void handle_gga_write(const boost::system::error_code& err) {
    if (!err) {
      gga_sent_ = true;
      boost::asio::async_read(
          socket_, response_, boost::asio::transfer_at_least(1),
          boost::bind(&client::handle_read_icy, this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    } else {
      std::cout << "Error: " << err.message() << "\n";
    }
  }

  void handle_read_status_line(const boost::system::error_code& err) {
    if (!err) {
      // Check that response is OK.
      std::istream response_stream(&response_);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;
      std::string status_message;
      std::getline(response_stream, status_message);

      if (!response_stream) {
        std::cout << "Invalid response\n";
        return;
      }
      if (http_version.substr(0, 5) == "HTTP/") {
        return handle_http();
      } else if (http_version.substr(0, 11) == "SOURCETABLE") {
        std::cout << "Received source table: " << std::endl;
        return handle_source_table();
      } else if (http_version.substr(0, 3) == "ICY") {
        std::cout << "Upgraded to ICY" << std::endl;
        return handle_icy();
      } else {
        std::cout << "Invalid response\n";
        std::cout << http_version << std::endl;
        return;
      }
      if (status_code != 200) {
        std::cout << "Response returned with status code ";
        std::cout << status_code << "\n";
        return;
      }

    } else {
      std::cout << "Error: " << err << "\n";
    }
  }

  void handle_http() {
    boost::asio::async_read_until(
        socket_, response_, "\r\n\r\n",
        boost::bind(&client::handle_read_headers, this,
                    boost::asio::placeholders::error));
  }

  void handle_source_table() {
    boost::asio::async_read_until(
        socket_, response_, "\r\n\r\n",
        boost::bind(&client::handle_read_content, this,
                    boost::asio::placeholders::error));
  }

  void handle_icy() {
    // Continue reading remaining data until EOF.
    if (!gga_sent_) {
      boost::asio::async_write(socket_, boost::asio::buffer(gga_string_),
                               boost::bind(&client::handle_gga_write, this,
                                           boost::asio::placeholders::error));
      return;
    }

    boost::asio::async_read(
        socket_, response_, boost::asio::transfer_at_least(1),
        boost::bind(&client::handle_read_icy, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void handle_read_icy(const boost::system::error_code& err,
                       size_t bytes_transferred) {
    if (!err && bytes_transferred > 0) {
      // Write all of the data that has been read so far.
      std::cout << "Read: " << bytes_transferred << std::endl;
      // TODO: Do something with this stream data.
      std::istream response_stream(&response_);
      std::string stream_data;
      response_stream >> stream_data;

      // Continue reading remaining data until EOF.
      boost::asio::async_read(
          socket_, response_, boost::asio::transfer_at_least(1),
          boost::bind(&client::handle_read_icy, this,
                      boost::asio::placeholders::error,
                      boost::asio::placeholders::bytes_transferred));
    } else if (err != boost::asio::error::eof) {
      std::cout << "ICY read Error: " << err << "\n";
    }
  }

  void handle_read_headers(const boost::system::error_code& err) {
    if (!err) {
      // Process the response headers.
      std::istream response_stream(&response_);
      std::string header;
      while (std::getline(response_stream, header) && header != "\r")
        std::cout << header << "\n";
      std::cout << "\n";

      // Write whatever content we already have to output.
      if (response_.size() > 0) std::cout << &response_;

      // Start reading remaining data until EOF.
      boost::asio::async_read(socket_, response_,
                              boost::asio::transfer_at_least(1),
                              boost::bind(&client::handle_read_content, this,
                                          boost::asio::placeholders::error));
    } else {
      std::cout << "Error: " << err << "\n";
    }
  }

  void handle_read_content(const boost::system::error_code& err) {
    if (!err) {
      // Write all of the data that has been read so far.
      std::cout << &response_;

      // Continue reading remaining data until EOF.
      boost::asio::async_read(socket_, response_,
                              boost::asio::transfer_at_least(1),
                              boost::bind(&client::handle_read_content, this,
                                          boost::asio::placeholders::error));
    } else if (err != boost::asio::error::eof) {
      std::cout << "Error: " << err << "\n";
    }
  }
  tcp::resolver resolver_;
  tcp::socket socket_;
  boost::asio::streambuf request_;
  boost::asio::streambuf response_;
  bool gga_sent_ = false;
  std::string gga_string_ =
      "$GPGGA,165631.00,3220.8483085,N,12200.900759,W,1,05,01.9,+00400,M,,M,,*?"
      "?";
};

int main(int argc, char* argv[]) {
  try {
    if (argc != 3) {
      std::cout << "Usage: async_client <server> <path>\n";
      std::cout << "Example:\n";
      std::cout << "  async_client www.boost.org /LICENSE_1_0.txt\n";
      return 1;
    }

    boost::asio::io_service io_service;
    client c(io_service, argv[1], argv[2]);
    io_service.run();
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << "\n";
  }

  return 0;
}
