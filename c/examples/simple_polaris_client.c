/**************************************************************************/ /**
 * @brief Example Polaris client user.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <signal.h>
#include <stdio.h>

#include "point_one/polaris/polaris.h"

static PolarisContext_t context;

void HandleData(const uint8_t* buffer, size_t size_bytes) {
  printf("Recevied %zu bytes.\n", size_bytes);
}

void HandleSignal(int sig) {
  signal(sig, SIG_DFL);

  printf("Caught signal %s (%d). Closing Polaris connection.\n",
         sys_siglist[sig], sig);
  Polaris_Close(&context);
}

int main(int argc, const char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s API_KEY UNIQUE_ID\n", argv[0]);
    return 1;
  }

  const char* api_key = argv[1];
  const char* unique_id = argv[2];

  if (Polaris_Open(&context) != POLARIS_SUCCESS) {
    return 2;
  }

  printf("Opened Polaris context...\n");

  Polaris_SetCallback(&context, HandleData);

  if (Polaris_Authenticate(&context, api_key, unique_id) != POLARIS_SUCCESS) {
    Polaris_Close(&context);
    return 3;
  }

  printf("Connected to Polaris...\n");

  if (Polaris_SendECEFPosition(&context, -2707071.0, -4260565.0, 3885644.0) !=
      POLARIS_SUCCESS) {
    Polaris_Close(&context);
    return 4;
  }

  printf("Sent position. Listening for data...\n");

  signal(SIGINT, HandleSignal);
  signal(SIGTERM, HandleSignal);

  Polaris_Run(&context);

  printf("Finished.\n");

  return 0;
}
