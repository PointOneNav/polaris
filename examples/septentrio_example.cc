// Copyright (C) Point One Navigation - All Rights Reserved

#include <boost/asio.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <point_one/polaris/polarispp.h>

#include "septentrio/septentrio_service.h"

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

DEFINE_string(polaris_unique_id, "device12345",
              "The unique ID to assign to this Polaris connection.");

// Septentrio/output options:
DEFINE_string(device, "/dev/ttyACM0",
              "The serial device on which the Septentrio is connected.");

DEFINE_string(
    udp_host, "",
    "If set, forward RTCM corrections over UDP to the specified hostname/IP in "
    "addition to the Septentrio receiver (over serial).");

DEFINE_int32(
    udp_port, 0,
    "The UDP port to which RTCM corrections will be forwarded.");

#ifndef RAD2DEG
#define RAD2DEG (57.295779513082320876798154814105)  //!< 180.0/PI
#endif

// Simple callback to print position when received and update Polaris Client.
void PvtCallback(const PVTGeodetic_2_2_t& pvt, PolarisClient* polaris_client) {
  LOG_EVERY_N(INFO, 10) << "Week: " << pvt.WNc << " Tow: " << pvt.TOW / 1000.0
                        << " Solution Type: " << (int)pvt.Mode
                        << " Hacc: " << pvt.HAccuracy
                        << " Vacc: " << pvt.VAccuracy
                        << " Lat: " << pvt.Lat * RAD2DEG
                        << " Lon: " << pvt.Lon * RAD2DEG << " Alt: " << pvt.Alt;
  polaris_client->SendLLAPosition(pvt.Lat * RAD2DEG, pvt.Lon * RAD2DEG,
                                  pvt.Alt);
}

int main(int argc, char *argv[]) {
  ParseCommandLineFlags(&argc, &argv, true);
  InitGoogleLogging(argv[0]);

  boost::asio::io_service io_loop;
  boost::asio::io_service::work work(io_loop);

  bool udp_enabled = false;
  boost::asio::ip::udp::socket udp_socket(io_loop);
  boost::asio::ip::udp::endpoint udp_endpoint;

  // Validate arguments.
  if (FLAGS_polaris_api_key == "") {
    LOG(ERROR) << "You must supply a Polaris API key to connect to the server.";
    return 1;
  }

  if (!FLAGS_udp_host.empty()) {
    if (FLAGS_udp_port < 1 || FLAGS_udp_port > 65535) {
      LOG(ERROR)
          << "You must specify a valid UDP port when enabling UDP output.";
      return 1;
    }
    else {
      udp_enabled = true;
      udp_endpoint = boost::asio::ip::udp::endpoint(
          boost::asio::ip::address_v4::from_string(FLAGS_udp_host),
          static_cast<unsigned short>(FLAGS_udp_port));
    }
  }

  // Connect to the Septentrio receiver.
  point_one::gpsreceiver::SeptentrioService septentrio(FLAGS_device);
  LOG(INFO) << "Connecting to receiver...";
  if (!septentrio.Connect(io_loop)) {
    LOG(FATAL) << "Could not connect to the receiver at path: " << FLAGS_device;
  }

  // Create the Polaris client.
  PolarisClient polaris_client(FLAGS_polaris_api_key, FLAGS_polaris_unique_id);

  // This callback will forward RTCM correction bytes received from Polaris to
  // the Septentrio, as well as to an output UDP port if enabled.
  polaris_client.SetRTCMCallback([&](const uint8_t* buffer, size_t size_bytes) {
    septentrio.SendRtcm(buffer, size_bytes);
    if (udp_enabled) {
      udp_socket.send_to(boost::asio::buffer(buffer, size_bytes), udp_endpoint);
    }
  });

  // This callback will send position updates from the Septentrio to Polaris.
  // Polaris uses the positions to associate the connection with a corrections
  // stream.
  //
  // Position updates should be sent periodically to ensure the incoming
  // corrections are from the most appropriate stream.
  //
  // Note that positions are supplied by the Septentrio over serial. The UDP
  // interface does not support receiving positions.
  septentrio.SetPvtCallback(
      std::bind(PvtCallback, std::placeholders::_1, &polaris_client));

  // Run the Polaris connection connection asynchronously.
  LOG(INFO) << "Connecting to Polaris...";
  polaris_client.RunAsync();

  // Now run the Boost IO loop to communicate with the serial port. This will
  // block forever.
  LOG(INFO) << "Listening for incoming serial data...";
  io_loop.run();

  return 0;
}
