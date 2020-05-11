// Copyright (C) Point One Navigation - All Rights Reserved

#include <iomanip>
#include <iostream>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <point_one/polaris_asio_client.h>
#include "septentrio_service.h"

// Options for connecting to Polaris Server:
DEFINE_string(
    polaris_host, point_one::polaris::DEFAULT_POLARIS_URL,
    "The Point One Navigation Polaris server tcp URI to which to connect");

DEFINE_int32(polaris_port, point_one::polaris::DEFAULT_POLARIS_PORT,
             "The tcp port to which to connect");

DEFINE_string(polaris_api_key, "",
              "The service API key. Contact account administrator or "
              "sales@pointonenav.com if unknown.");

// Serial options.
DEFINE_string(device, "/dev/ttyACM0", "The reciver com port file handle");

#ifndef RAD2DEG
#define RAD2DEG (57.295779513082320876798154814105)  //!< 180.0/PI
#endif

// Simple callback to print position when received and update Polaris Client.
void PvtCallback(const PVTGeodetic_2_2_t &pvt,
                 point_one::polaris::PolarisAsioClient *polaris_client) {
  LOG_EVERY_N(INFO, 10) << "Week: " << pvt.WNc << " Tow: " << pvt.TOW / 1000.0
                        << " Solution Type: " << (int)pvt.Mode
                        << " Hacc: " << pvt.HAccuracy
                        << " Vacc: " << pvt.VAccuracy
                        << " Lat: " << pvt.Lat * RAD2DEG
                        << " Lon: " << pvt.Lon * RAD2DEG << " Alt: " << pvt.Alt;
  polaris_client->SetPositionLLA(pvt.Lat * RAD2DEG, pvt.Lon * RAD2DEG, pvt.Alt);
}

// Allows for prebuilt versions of gflags/google that don't have gflags/google
// namespace.
namespace gflags {}
namespace google {}
using namespace gflags;
using namespace google;

int main(int argc, char *argv[], char *envp[]) {
  ParseCommandLineFlags(&argc, &argv, true);
  InitGoogleLogging(argv[0]);

  boost::asio::io_service io_loop;
  boost::asio::io_service::work work(io_loop);

  // Create connection to receiver to forward correction data received from
  // Polaris Server.

  // Construct a Polaris client.
  if (FLAGS_polaris_api_key == "") {
    LOG(FATAL) << "You must supply a Polaris API key to connect to the server.";
    return 1;
  }

  point_one::gpsreceiver::SeptentrioService septentrio(FLAGS_device);
  LOG(INFO) << "Connecting to receiver...";
  if (!septentrio.Connect(io_loop)) {
    LOG(FATAL) << "Could not connect to the receiver at path: " << FLAGS_device;
  }

  // Create the Polaris client.
  point_one::polaris::PolarisConnectionSettings settings;
  settings.host = FLAGS_polaris_host;
  settings.port = FLAGS_polaris_port;
  point_one::polaris::PolarisAsioClient polaris_client(
      io_loop, FLAGS_polaris_api_key, "septentrio12345", settings);

  // This callback will forward RTCM correction bytes received from the server
  // to the septentrio.
  polaris_client.SetPolarisBytesReceived(
      std::bind(&point_one::gpsreceiver::SeptentrioService::SendRtcm,
                &septentrio, std::placeholders::_1, std::placeholders::_2));

  // Add callback for when we get a position updated.
  septentrio.SetPvtCallback(
      std::bind(PvtCallback, std::placeholders::_1, &polaris_client));

  // Application can set position at any time to change associated beacon(s) and
  // corrections. Example setting postion to SF in ECEF meters.
  // It is required to call SetPositionECEF at some cadence to assure the
  // corrections received are the best and relevant.
  // In this example the position will be overwritten upon a successful
  // connection to the receiver.
  polaris_client.SetPositionECEF(-2707071, -4260565, 3885644);
  polaris_client.Connect();

  LOG(INFO) << "Starting services...";
  io_loop.run();
  return 0;
}