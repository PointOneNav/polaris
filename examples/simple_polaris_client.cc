/**************************************************************************/ /**
 * @brief Simple example of connecting to the Polaris service to receive
 *        corrections.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <iomanip>
#include <iostream>
#include <signal.h>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <point_one/polaris/polaris_client.h>

// Allows for prebuilt versions of gflags/google that don't have gflags/google
// namespace.
namespace gflags {}
namespace google {}
using namespace gflags;
using namespace google;

using namespace point_one::polaris;

// Polaris options:
DEFINE_string(polaris_api_key, "",
              "The polaris API key. Sign up at app.pointonenav.com.");

DEFINE_string(polaris_unique_id, "device12345",
              "The unique ID to assign to this Polaris connection.");

PolarisClient* polaris_client = nullptr;

// Process receiver incoming messages.  This example code expects received data
// to be ascii nmea messages.
void ReceivedData(const uint8_t* data, size_t length) {
  LOG(INFO) << "Application received " << length << " bytes.";
}

void HandleSignal(int sig) {
  signal(sig, SIG_DFL);

  LOG(INFO) << "Caught signal " << strsignal(sig) << " (" << sig
            << "). Closing Polaris connection.";
  polaris_client->Disconnect();
}

int main(int argc, char* argv[]) {
  // Parse commandline flags.
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;
  ParseCommandLineFlags(&argc, &argv, true);

  // Setup logging interface.
  InitGoogleLogging(argv[0]);

  // Construct a Polaris client.
  if (FLAGS_polaris_api_key.empty()) {
    LOG(ERROR) << "You must supply a Polaris API key to connect to the server.";
    return 1;
  }
  else if (FLAGS_polaris_unique_id.empty()) {
    LOG(ERROR) << "You must supply a unique ID to connect to the server.";
    return 1;
  }

  polaris_client =
      new PolarisClient(FLAGS_polaris_api_key, FLAGS_polaris_unique_id);
  polaris_client->SetRTCMCallback(
      std::bind(&ReceivedData, std::placeholders::_1, std::placeholders::_2));

  // Send the receiver's position to Polaris (this example is a position in San
  // Francisco). Sending your position allows Polaris to associate you with an
  // appropriate corrections stream. You must send a position at least once
  // before any data will be sent back. You should periodically update your
  // position as you move to make sure you are receiving the best set of
  // corrections.
  LOG(INFO) << "Setting initial position.";
  polaris_client->SendECEFPosition(-2707071.0, -4260565.0, 3885644.0);

  // Run the client, receiving data until the user presses Ctrl-C.
  signal(SIGINT, HandleSignal);
  signal(SIGTERM, HandleSignal);

  LOG(INFO) << "Connecting to Polaris and listening for data...";
  polaris_client->Run();

  LOG(INFO) << "Finished running. Cleaning up.";
  delete polaris_client;

  LOG(INFO) << "Exiting.";
  return 0;
}
