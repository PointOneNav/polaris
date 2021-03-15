// Copyright (C) Point One Navigation - All Rights Reserved

#pragma once

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <glog/logging.h>

namespace point_one {
namespace utils {

// Simple wrapper around Boost asio serial port to add minimal error checking
// and logging.
class SimpleAsioSerialPort {
 public:
  explicit SimpleAsioSerialPort(boost::asio::io_service& io_service)
      : sp_(io_service) {}

  ~SimpleAsioSerialPort() { sp_.close(); }

  // Open serial port and return true on success.
  bool Open(const std::string& path, int baud_rate = 115200) {
    try {
      sp_.open(path);
    } catch (boost::system::system_error& e) {
      LOG(ERROR) << "Could not connect to serial port at " << path
                 << " -> Boost: " << e.what();
      return false;
    }
    // Setup The options
    sp_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    sp_.set_option(boost::asio::serial_port_base::character_size(8));
    sp_.set_option(boost::asio::serial_port_base::stop_bits(
        boost::asio::serial_port_base::stop_bits::one));
    sp_.set_option(boost::asio::serial_port_base::parity(
        boost::asio::serial_port_base::parity::none));
    sp_.set_option(boost::asio::serial_port_base::flow_control(
        boost::asio::serial_port_base::flow_control::none));

    AsyncReadData();
    return true;
  }

  void Write(const void* buf, size_t len) {
    VLOG(6) << "Forwarding " << len << " bytes to serial port.";
    sp_.write_some(boost::asio::buffer(buf, len));
  }

  void AsyncWrite(const void* buf, size_t len) {
    VLOG(6) << "Forwarding " << len << " bytes to serial port.";
    std::string data((const char*)buf, len);
    sp_.async_write_some(
        boost::asio::buffer(buf, len),
        boost::bind(&SimpleAsioSerialPort::OnWrite, this, data,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void OnWrite(const std::string& buffer_data,
               const boost::system::error_code& error_code,
               size_t bytes_transferred) {
    if (error_code) {
      LOG(ERROR) << "ERROR on receive " << error_code.message().c_str();
    }
  }

  void SetCallback(std::function<void(const void* data, size_t len)> callback) {
    this->callback = callback;
  }

  void AsyncReadData() {
    if (!sp_.is_open()) return;

    sp_.async_read_some(
        boost::asio::buffer(buf_, READ_SIZE),
        boost::bind(&SimpleAsioSerialPort::OnReceive, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
  }

  void OnReceive(const boost::system::error_code& error_code,
                 size_t bytes_transferred) {
    if (!sp_.is_open()) {
      return;
    }

    if (error_code) {
      LOG(FATAL) << "Error on receive " << error_code.message().c_str();

      AsyncReadData();
      return;
    }

    if (this->callback != nullptr) {
      callback(buf_, bytes_transferred);
    }
    AsyncReadData();
  }

 private:
  boost::asio::serial_port sp_;
  static const int READ_SIZE = 1024;
  char buf_[READ_SIZE];

  std::function<void(const void*, size_t)> callback;
};

}  // namespace utils
}  // namespace point_one
