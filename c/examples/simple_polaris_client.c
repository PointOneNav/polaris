/**************************************************************************/ /**
 * @brief Simple Polaris client example.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <signal.h>
#include <stdlib.h> // For atoi()

#include "point_one/polaris/polaris.h"
#include "point_one/polaris/portability.h"

static PolarisContext_t context;

void HandleData(void* info, PolarisContext_t* polaris_context,
                const uint8_t* buffer, size_t size_bytes) {
  P1_printf("Application received %u bytes.\n", (unsigned)size_bytes);
}

void HandleSignal(int sig) {
  signal(sig, SIG_DFL);

  P1_printf("Caught signal %s (%d). Closing Polaris connection.\n",
            strsignal(sig), sig);
  Polaris_Disconnect(&context);
}

int main(int argc, const char* argv[]) {
  if (argc < 2 || argc > 4) {
    P1_fprintf(stderr,
               "Usage: %s API_KEY UNIQUE_ID [LOG_LEVEL (1=debug, 2=trace)]\n",
               argv[0]);
    return 1;
  }

  const char* api_key = argv[1];
  if (strlen(api_key) == 0) {
    P1_fprintf(stderr,
               "You must supply a Polaris API key to connect to the server.\n");
    return 1;
  }

  const char* unique_id = argv[2];
  if (strlen(unique_id) == 0) {
    P1_fprintf(stderr, "You must supply a unique ID for this connection.\n");
    return 1;
  }

  int log_level = argc > 3 ? atoi(argv[3]) : POLARIS_LOG_LEVEL_INFO;
  Polaris_SetLogLevel(log_level);

  // Retrieve an access token using the specified API key.
  if (Polaris_Init(&context) != POLARIS_SUCCESS) {
    return 2;
  }

  P1_printf("Opened Polaris context. Authenticating...\n");

  if (Polaris_Authenticate(&context, api_key, unique_id) != POLARIS_SUCCESS) {
    Polaris_Free(&context);
    return 3;
  }

  // We now have a valid access token. Connect to the corrections service.
  P1_printf("Authenticated. Connecting to Polaris...\n");

  Polaris_SetRTCMCallback(&context, HandleData, NULL);

  if (Polaris_Connect(&context) != POLARIS_SUCCESS) {
    Polaris_Free(&context);
    return 3;
  }

  P1_printf("Connected to Polaris...\n");

  // Send a position update to Polaris. Position updates are used to select an
  // appropriate corrections stream, and should be updated periodically as the
  // receiver moves.
  P1_printf("Setting initial position.\n");
  if (Polaris_SendLLAPosition(&context, 37.773971, -122.430996, -0.02) !=
      POLARIS_SUCCESS) {
    Polaris_Disconnect(&context);
    Polaris_Free(&context);
    return 4;
  }

  // Receive incoming RTCM data until the application exits.
  P1_printf("Sent position. Listening for data...\n");

  signal(SIGINT, HandleSignal);
  signal(SIGTERM, HandleSignal);

  Polaris_Run(&context, 30000);

  P1_printf("Finished running. Cleaning up.\n");
  Polaris_Free(&context);

  P1_printf("Exiting.\n");
  return 0;
}
