/**************************************************************************/ /**
 * @brief Example Polaris client user.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <signal.h>

#include "point_one/polaris/polaris.h"
#include "point_one/polaris/portability.h"

static PolarisContext_t context;

void HandleData(const uint8_t* buffer, size_t size_bytes) {
  P1_printf("Recevied %u bytes.\n", (unsigned)size_bytes);
}

void HandleSignal(int sig) {
  signal(sig, SIG_DFL);

  P1_printf("Caught signal %s (%d). Closing Polaris connection.\n",
            sys_siglist[sig], sig);
  Polaris_Disconnect(&context);
}

int main(int argc, const char* argv[]) {
  if (argc != 3) {
    P1_fprintf(stderr, "Usage: %s API_KEY UNIQUE_ID\n", argv[0]);
    return 1;
  }

  const char* api_key = argv[1];
  const char* unique_id = argv[2];

  if (Polaris_Init(&context) != POLARIS_SUCCESS) {
    return 2;
  }

  P1_printf("Opened Polaris context. Authenticating...\n");

  if (Polaris_Authenticate(&context, api_key, unique_id) != POLARIS_SUCCESS) {
    return 3;
  }

  P1_printf("Authenticated. Connecting to Polaris...\n");

  Polaris_SetRTCMCallback(&context, HandleData);

  if (Polaris_Connect(&context) != POLARIS_SUCCESS) {
    return 3;
  }

  P1_printf("Connected to Polaris...\n");

  if (Polaris_SendECEFPosition(&context, -2707071.0, -4260565.0, 3885644.0) !=
      POLARIS_SUCCESS) {
    Polaris_Disconnect(&context);
    return 4;
  }

  P1_printf("Sent position. Listening for data...\n");

  signal(SIGINT, HandleSignal);
  signal(SIGTERM, HandleSignal);

  Polaris_Run(&context);

  P1_printf("Finished.\n");

  return 0;
}
