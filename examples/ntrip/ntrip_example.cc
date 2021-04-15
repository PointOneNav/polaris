/**************************************************************************/ /**
 * @brief Example of relaying Polaris corrections as an NTRIP server.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <point_one/polaris/polaris_client.h>

#include "ntrip_server.h"

// Allows for prebuilt versions of gflags/google that don't have gflags/google
// namespace.
namespace gflags {}
namespace google {}
using namespace gflags;
using namespace google;

using namespace point_one::polaris;

// Options for connecting to Polaris Server:
DEFINE_string(
    polaris_host, "",
    "Specify an alternate Polaris corrections endpoint URL to be used.");

DEFINE_int32(polaris_port, 0,
             "Specify an alternate TCP port for the Polaris corrections "
             "endpoint.");

DEFINE_string(polaris_api_key, "",
              "The service API key. Contact account administrator or "
              "sales@pointonenav.com if unknown.");

DEFINE_string(polaris_unique_id, "ntrip-device12345",
              "The unique ID to assign to this Polaris connection.");

double ConvertGGADegrees(double gga_degrees) {
  double degrees = std::floor(gga_degrees/100.0);
  degrees += (gga_degrees - degrees * 100) / 60.0;
  return degrees;
}

void OnGpgga(const std::string& gpgga, PolarisClient* polaris_client) {
  LOG_FIRST_N(INFO, 1) << "Got first receiver GPGGA: " << gpgga;
  VLOG(1) << "Got receiver GPGGA: " << gpgga;
  std::stringstream ss(gpgga);
  std::vector<std::string> result;

  while( ss.good() )
  {
      std::string substr;
      std::getline( ss, substr, ',' );
      result.push_back( substr );
  }
  std::string::size_type sz;     // alias of size_t

  try {
    // TODO: This is not exactly correct but should not really matter because its just beacon association.
    double lat = ConvertGGADegrees(std::stod(result[2], &sz)) * (result[3] == "N" ? 1 : -1);
    double lon = ConvertGGADegrees(std::stod(result[4], &sz)) * (result[5] == "E" ? 1 : -1);
    double alt = std::stod(result[8], &sz);

    VLOG(2) << "Setting position: lat: " << lat << " lon: " << lon << " alt: " << alt;
    polaris_client->SendLLAPosition(lat, lon, alt);
  }
  catch (const std::exception&){
    LOG(WARNING) << "GPGGA Bad parse of string " << gpgga;
    return;
  }
}


int main(int argc, char* argv[]) {
  // Parse commandline flags.
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;
  ParseCommandLineFlags(&argc, &argv, true);

  // Setup logging interface.
  InitGoogleLogging(argv[0]);

  try {
    // Check command line arguments.
    if (argc != 4) {
      LOG(INFO) << "Usage: ntrip_example --polaris_api_key=KEY <address> <port> <doc_root>\n";
      LOG(INFO) << "For IPv4, try: --polaris_api_key=KEY 0.0.0.0 2101 examples/ntrip\n";
      return 1;
    }

    // Create connection to receiver to forward correction data received from
    // Polaris Server.

    // Construct a Polaris client.
    if (FLAGS_polaris_api_key == "") {
      LOG(ERROR)
          << "You must supply a Polaris API key to connect to the server.";
      return 1;
    }

    PolarisClient polaris_client(FLAGS_polaris_api_key,
                                 FLAGS_polaris_unique_id);
    polaris_client.SetPolarisEndpoint(FLAGS_polaris_host, FLAGS_polaris_port);

    // Setup the NTRIP server.
    std::string ntrip_host = argv[1];
    std::string ntrip_port = argv[2];
    std::string ntrip_root = argv[3];
    LOG(INFO) << "Starting NTRIP server on " << ntrip_host << ":" << ntrip_port
              << ".";
    boost::asio::io_service io_loop;
    boost::asio::io_service::work work(io_loop);
    ntrip::server ntrip_server(io_loop, ntrip_host, ntrip_port, ntrip_root);

    polaris_client.SetRTCMCallback(
        std::bind(&ntrip::server::broadcast, &ntrip_server, "/Polaris",
                  std::placeholders::_1, std::placeholders::_2));

    ntrip_server.SetGpggaCallback(
        std::bind(OnGpgga, std::placeholders::_1, &polaris_client));

    // Run the Polaris connection connection asynchronously.
    LOG(INFO) << "Connecting to Polaris...";
    polaris_client.RunAsync();

    // Now run the Boost IO loop to communicate with the serial port. This will
    // block forever.
    LOG(INFO) << "Running NTRIP server...";
    io_loop.run();
  } catch (std::exception& e) {
    LOG(ERROR) << "Caught exception: " << e.what();
  }

  return 0;
}
