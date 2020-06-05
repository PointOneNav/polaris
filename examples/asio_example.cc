// Copyright (C) Point One Navigation - All Rights Reserved
// Written by Jason Moran <jason@pointonenav.com>, March 2019

#include <iomanip>
#include <iostream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/asio.hpp>

#include <gflags/gflags.h>

#include <point_one/polaris_asio_client.h>
#include "simple_asio_serial_port.h"

// Options for connecting to Polaris Server:
DEFINE_string(
    polaris_host, point_one::polaris::DEFAULT_POLARIS_URL,
    "The Point One Navigation Polaris server tcp URI to which to connect");

DEFINE_int32(polaris_port, point_one::polaris::DEFAULT_POLARIS_PORT,
             "The tcp port to which to connect");

DEFINE_string(polaris_api_key, "",
              "The service API key. Contact account administrator or "
              "sales@pointonenav.com if unknown.");

// Serial port forwarding options.
DEFINE_string(receiver_serial_port, "/dev/ttyACM0",
              "The path to the serial port for which to forward corrections.");

DEFINE_int32(receiver_serial_baud, 115200, "The baud rate of the serial port.");

// Allows for prebuilt versions of gflags/google that don't have gflags/google
// namespace.
namespace gflags {}
namespace google {}
using namespace gflags;
using namespace google;

namespace {
// Max size of string buffer to hold before clearing nmea data. Should be larger
// than max expected frame size.
constexpr double MAX_NMEA_SENTENCE_LENGTH = 1024;

// Variable to hold nmea data for this example.  If using in production
// you should use a production grade nmea parser.
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
void OnNmea(const std::string& nmea_str,
            point_one::polaris::PolarisAsioClient* polaris_client) {
  // Some receivers put out INGGA messages as opposed to GPGGA.
  if (!(boost::istarts_with(nmea_str, "$GPGGA") ||
        boost::istarts_with(nmea_str, "$INGGA"))) {
    LOG(INFO) << nmea_str;
    return;
  }
  VLOG(4) << "Got GGA: " << nmea_str;
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
    double alt = std::stod(result[8], &sz);
    VLOG(3) << "Setting position: lat: " << lat << " lon: " << lon
              << " alt: " << alt;
    polaris_client->SetPositionLLA(lat, lon, alt);
  } catch (const std::exception&) {
    LOG(ERROR) << "Bad parse of received NMEA string: " << nmea_str;
    LOG(WARNING) << "Could not send position to server for bad NMEA string.";
    return;
  }
}

// Process receiver incomming messages.  This example code expects received data
// to be ascii nmea messages.
void OnSerialData(const void* data, size_t length,
                  point_one::polaris::PolarisAsioClient* polaris_client) {
  std::string nmea_data((const char*)data, length);
  for (const char& c : nmea_data) {
    if (c == '$') {
      OnNmea(nmea_sentence_buffer, polaris_client);
      nmea_sentence_buffer.clear();
    }
    nmea_sentence_buffer.push_back(c);
  }

  if (nmea_sentence_buffer.size() > MAX_NMEA_SENTENCE_LENGTH) {
    LOG(WARNING) << "Clearing nmea buffer.  Are you sending NMEA ascii data?";
  }
}

int main(int argc, char* argv[]) {
  // Parse commandline flags.
  ParseCommandLineFlags(&argc, &argv, true);

  // Setup logging interface.
  InitGoogleLogging(argv[0]);

  // Setup asio work loop.
  boost::asio::io_service io_loop;
  boost::asio::io_service::work work(io_loop);

  // Create connection to receiver to forward correction data received from
  // Polaris Server.
  point_one::utils::SimpleAsioSerialPort serial_port_correction_forwarder(
      io_loop);
  if (!serial_port_correction_forwarder.Open(FLAGS_receiver_serial_port,
                                             FLAGS_receiver_serial_baud)) {
    LOG(FATAL) << "Could not open serial port" << FLAGS_receiver_serial_port
               << ", terminating.";
    return 1;
  }

  // Construct a Polaris client.
  if (FLAGS_polaris_api_key == "") {
    LOG(FATAL) << "You must supply a Polaris API key to connect to the server.";
    return 1;
  }

  point_one::polaris::PolarisConnectionSettings settings;
  settings.host = FLAGS_polaris_host;
  settings.port = FLAGS_polaris_port;
  point_one::polaris::PolarisAsioClient polaris_client(
      io_loop, FLAGS_polaris_api_key, "ntrip-device12345", settings);
  polaris_client.SetPolarisBytesReceived(
      std::bind(&point_one::utils::SimpleAsioSerialPort::Write,
                &serial_port_correction_forwarder, std::placeholders::_1,
                std::placeholders::_2));
  // Application can set position at any time to change associated beacon(s) and
  // corrections. Example setting postion to SF in ECEF meters.
  // It is required to call SetPositionECEF at some cadence to assure the
  // corrections received are the best and relevant.
  polaris_client.SetPositionECEF(-2707071, -4260565, 3885644);
  polaris_client.Connect();

  serial_port_correction_forwarder.SetCallback(
      std::bind(OnSerialData, std::placeholders::_1, std::placeholders::_2,
                &polaris_client));

  // Run work loop.
  io_loop.run();

  return 0;
}
