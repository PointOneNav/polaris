// Copyright (C) Point One Navigation - All Rights Reserved

#include <iomanip>
#include <iostream>

#include <boost/asio.hpp>

#include <gflags/gflags.h>

#include <point_one/polaris_asio_embedded_client.h>

// Polaris options:
DEFINE_string(polaris_signing_secret, "bRb1q3k4yiX4VbZxztx",
              "The signing secret.");
DEFINE_string(company_id, "co994711e0d78611e69dd502fd95286f33",
              "Company ID issued by Point One.");
DEFINE_string(unique_id, "123456789",
              "Unique identifier of the device (serial number).");
DEFINE_string(api_server, "api.pointonenav.com", "Address of the API server");
DEFINE_string(polaris_server, "polaris.pointonenav.com",
              "Address of the Polaris server");

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

  auto connection_settings = point_one::polaris::DEFAULT_CONNECTION_SETTINGS;
  connection_settings.api_host = FLAGS_api_server;
  connection_settings.host = FLAGS_polaris_server;

  point_one::polaris::PolarisAsioEmbeddedClient polaris_client(
      io_loop, FLAGS_polaris_signing_secret, FLAGS_unique_id, FLAGS_company_id,
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
