// Example ntrip client using boost asio, based off of boost http server example
// See http://www.boost.org/LICENSE_1_0.txt

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <string>
#include "ntrip_server.h"
#include <point_one/polaris_asio_client.h>

#include "gflags/gflags.h"
#include "glog/logging.h"

// Options for connecting to Polaris Server:
DEFINE_string(
    polaris_host, point_one::polaris::DEFAULT_POLARIS_URL,
    "The Point One Navigation Polaris server tcp URI to which to connect");

DEFINE_int32(polaris_port, point_one::polaris::DEFAULT_POLARIS_PORT,
             "The tcp port to which to connect");

DEFINE_string(polaris_api_key, "",
              "The service API key. Contact account administrator or "
              "sales@pointonenav.com if unknown.");

namespace gflags {}
namespace google {}
using namespace gflags;
using namespace google;

double ConvertGGADegrees(double gga_degrees) {
  double degrees = std::floor(gga_degrees/100.0);
  degrees += (gga_degrees - degrees * 100) / 60.0;
  return degrees;
}

void OnGpgga(const std::string &gpgga,
                 point_one::polaris::PolarisAsioClient *polaris_client) {
  LOG(INFO) << "Got gpgga: " << gpgga;
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
    
    LOG(INFO) << "Setting position: lat: " << lat << " lon: " << lon << " alt: " << alt; 
    polaris_client->SetPositionLLA(lat, lon, alt);
  }
  catch ( std::exception){ 
    LOG(ERROR) << "Bad parse of string " << gpgga;
    return;
  }
}


int main(int argc, char* argv[]) {
  ParseCommandLineFlags(&argc, &argv, true);
  InitGoogleLogging(argv[0]);
  try {
    // Check command line arguments.
    if (argc != 4) {
      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }

    boost::asio::io_service io_loop;
    boost::asio::io_service::work work(io_loop);

    // Create connection to receiver to forward correction data received from
    // Polaris Server.

    // Construct a Polaris client.
    if (FLAGS_polaris_api_key == "") {
      LOG(FATAL)
          << "You must supply a Polaris API key to connect to the server.";
      return 1;
    }

    // Create the Polaris client.
    point_one::polaris::PolarisConnectionSettings settings;
    settings.host = FLAGS_polaris_host;
    settings.port = FLAGS_polaris_port;
    point_one::polaris::PolarisAsioClient polaris_client(
        io_loop, FLAGS_polaris_api_key, "ntrip_example", settings);

    // Initialise the server.
    ntrip::server ntrip_server(io_loop, argv[1], argv[2], argv[3]);

    // This callback will forward RTCM correction bytes received from the server
    // to the septentrio.
    polaris_client.SetPolarisBytesReceived(
        std::bind(&ntrip::server::broadcast,
                  &ntrip_server, "/Polaris", std::placeholders::_1, std::placeholders::_2));

    // TODO: Add callback for when we get a position updated.
    ntrip_server.SetGpggaCallback(
      std::bind(OnGpgga, std::placeholders::_1, &polaris_client));

    // Application can set position at any time to change associated beacon(s)
    // and corrections. Example setting postion to SF in ECEF meters. It is
    // required to call SetPositionECEF at some cadence to assure the
    // corrections received are the best and relevant.
    // In this example the position will be overwritten upon a successful
    // connection to the receiver.
    polaris_client.SetPositionECEF(-2707071, -4260565, 3885644);
    polaris_client.Connect();

    // Run the server until stopped.
    io_loop.run();
  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
