/**************************************************************************/ /**
 * @brief Example of receiving positions from a GNSS receiver and forwarding
 *        Polaris corrections to the receiver over a serial port.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <iomanip>
#include <iostream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio.hpp>
#include <gflags/gflags.h>

#include <point_one/polaris/polaris_client.h>

#include "simple_asio_serial_port.h"

// Allows for prebuilt versions of gflags/google that don't have gflags/google
// namespace.
namespace gflags {}
namespace google {}
using namespace gflags;
using namespace google;

using namespace point_one::polaris;

// Polaris options:
DEFINE_string(polaris_api_key, "",
              "The service API key. Contact account administrator or "
              "sales@pointonenav.com if unknown.");

DEFINE_string(polaris_unique_id, "",
              "The unique ID to assign to this Polaris connection.");

// Serial port forwarding options.
DEFINE_string(receiver_serial_port, "/dev/ttyUSB0",
              "The path to the serial port for which to forward corrections.");

DEFINE_int32(receiver_serial_baud, 460800, "The baud rate of the serial port.");

namespace {
// Max size of string buffer to hold before clearing NMEA data. Should be larger
// than max expected frame size.
constexpr double MAX_NMEA_SENTENCE_LENGTH = 1024;

// Variable to hold NMEA data for this example.  If using in production
// you should use a production grade NMEA parser.
std::string nmea_sentence_buffer;
}  // namespace

// Converts from GPGGA ddMM (degrees arc minutes) format to decimal degrees
double ConvertGGADegreesToDecimalDegrees(double gga_degrees) {
  double degrees = std::floor(gga_degrees / 100.0);
  degrees += (gga_degrees - degrees * 100) / 60.0;
  return degrees;
}

// Handle an nmea string.  If message is a parseable gga message, send position
// to server.
void OnNmea(const std::string& nmea_str, PolarisClient* polaris_client) {
  // Some receivers put out INGGA messages as opposed to GPGGA.
  if (!(boost::istarts_with(nmea_str, "$GPGGA") ||
        boost::istarts_with(nmea_str, "$GNGGA") ||
        boost::istarts_with(nmea_str, "$INGGA"))) {
    VLOG(2) << nmea_str;
    return;
  }
  LOG(INFO) << "Got GGA: " << nmea_str;
  std::stringstream ss(nmea_str);
  std::vector<std::string> result;

  while (ss.good()) {
    std::string substr;
    std::getline(ss, substr, ',');
    result.push_back(substr);
  }
  std::string::size_type sz;
  try {
    // TODO: This is not exactly correct but should not really matter because
    // its just beacon association.
    double lat = ConvertGGADegreesToDecimalDegrees(std::stod(result[2], &sz)) *
                 (result[3] == "N" ? 1 : -1);
    double lon = ConvertGGADegreesToDecimalDegrees(std::stod(result[4], &sz)) *
                 (result[5] == "E" ? 1 : -1);
    double alt = std::stod(result[9], &sz);
    VLOG(3) << "Setting position: lat: " << lat << " lon: " << lon
              << " alt: " << alt;
    polaris_client->SendLLAPosition(lat, lon, alt);
  } catch (const std::exception&) {
    LOG(ERROR) << "Bad parse of received NMEA string: " << nmea_str;
    LOG(WARNING) << "Could not send position to server for bad NMEA string.";
    return;
  }
}

// Process receiver incomming messages.  This example code expects received data
// to be ascii NMEA messages.
void OnSerialData(const void* data, size_t length,
                  PolarisClient* polaris_client) {
  std::string nmea_data((const char*)data, length);
  for (const char& c : nmea_data) {
    if (c == '$') {
      OnNmea(nmea_sentence_buffer, polaris_client);
      nmea_sentence_buffer.clear();
    }
    nmea_sentence_buffer.push_back(c);
  }

  if (nmea_sentence_buffer.size() > MAX_NMEA_SENTENCE_LENGTH) {
    LOG(WARNING) << "Clearing NMEA buffer.  Are you sending NMEA ascii data?";
    nmea_sentence_buffer.clear();
  }
}

int main(int argc, char* argv[]) {
  // Parse commandline flags.
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;
  ParseCommandLineFlags(&argc, &argv, true);

  // Setup logging interface.
  InitGoogleLogging(argv[0]);

  // Setup asio work loop.
  boost::asio::io_service io_loop;
  boost::asio::io_service::work work(io_loop);

  // Create connection to receiver to forward correction data received from
  // Polaris.
  point_one::utils::SimpleAsioSerialPort serial_port_correction_forwarder(
      io_loop);
  if (!serial_port_correction_forwarder.Open(FLAGS_receiver_serial_port,
                                             FLAGS_receiver_serial_baud)) {
    LOG(ERROR) << "Could not open serial port " << FLAGS_receiver_serial_port
               << ", terminating.";
    return 1;
  }

  // Construct a Polaris client.
  if (FLAGS_polaris_api_key == "") {
    LOG(ERROR) << "You must supply a Polaris API key to connect to the server.";
    return 1;
  }

  PolarisClient polaris_client(FLAGS_polaris_api_key, FLAGS_polaris_unique_id);
  polaris_client.SetRTCMCallback([&](const uint8_t* buffer, size_t size_bytes) {
    serial_port_correction_forwarder.Write(buffer, size_bytes);
  });

  serial_port_correction_forwarder.SetCallback(
      std::bind(OnSerialData, std::placeholders::_1, std::placeholders::_2,
                &polaris_client));

  // Run the Polaris connection connection asynchronously.
  LOG(INFO) << "Connecting to Polaris...";
  polaris_client.RunAsync();

  // Now run the Boost IO loop to communicate with the serial port. This will
  // block forever.
  LOG(INFO) << "Listening for incoming serial data...";
  io_loop.run();

  return 0;
}
