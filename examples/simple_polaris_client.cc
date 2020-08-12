// Copyright (C) Point One Navigation - All Rights Reserved

#include <iomanip>
#include <iostream>

#include <boost/asio.hpp>

#include <gflags/gflags.h>

#include <point_one/polaris_asio_client.h>

// Polaris options:
DEFINE_string(polaris_api_key, "",
              "The service API key. Contact account administrator or "
              "sales@pointonenav.com if unknown.");

DEFINE_string(polaris_unique_id, "device12345",
              "The unique ID to assign to this Polaris connection.");

// Allows for prebuilt versions of gflags/google that don't have gflags/google
// namespace.
namespace gflags {}
namespace google {}
using namespace gflags;
using namespace google;

// Process receiver incoming messages.  This example code expects received data
// to be ascii nmea messages.
void ReceivedData(const void* data, size_t length) {
  LOG(INFO) << "Received " << length << " bytes.";
}

int main(int argc, char* argv[]) {
  // Parse commandline flags.
  ParseCommandLineFlags(&argc, &argv, true);

  // Setup logging interface.
  InitGoogleLogging(argv[0]);

  // Setup asio work loop.
  boost::asio::io_service io_loop;
  boost::asio::io_service::work work(io_loop);

  // Construct a Polaris client.
  if (FLAGS_polaris_api_key == "") {
    LOG(FATAL) << "You must supply a Polaris API key to connect to the server.";
    return 1;
  }

  auto connection_settings = point_one::polaris::DEFAULT_CONNECTION_SETTINGS;
  point_one::polaris::PolarisAsioClient polaris_client(
      io_loop, FLAGS_polaris_api_key, FLAGS_polaris_unique_id,
      connection_settings);
  polaris_client.SetPolarisBytesReceived(
      std::bind(&ReceivedData, std::placeholders::_1, std::placeholders::_2));
  // Application can set position at any time to change associated beacon(s) and
  // corrections. Example setting postion to SF in ECEF meters.
  // It is required to call SetPositionECEF at some cadence to assure the
  // corrections received are the best and relevant.
  polaris_client.SetPositionECEF(-2707071, -4260565, 3885644);
  polaris_client.Connect();

  // Run work loop.
  io_loop.run();

  return 0;
}
