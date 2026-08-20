/**************************************************************************/ /**
 * @brief Example Polaris client with reconnect/reauthentication support.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <signal.h>
#include <stdlib.h> // For atoi()
#include <unistd.h> // For sleep()

#include "point_one/polaris/polaris.h"
#include "point_one/polaris/portability.h"

// The number of consecutive connection failures after which the access token is
// assumed to have expired and authentication is retried.
#define MAX_RECONNECTS 2

// The delay before the first reconnect attempt, and the longest delay to wait
// between attempts (in seconds).
#define MIN_RETRY_DELAY_SEC 1
#define MAX_RETRY_DELAY_SEC 30

// The Polaris network may respond to a rejected request (including a rejected
// access token) with a single RTCM 1029 text message and then close the
// connection. We wait until more than the maximum size of one such message has
// arrived before considering a connection healthy.
#define MIN_HEALTHY_BYTES 270

static PolarisContext_t context;

static volatile sig_atomic_t running = 1;

static int retry_delay_sec = 0;
static size_t bytes_received = 0;
static int reconnect_count = 0;

/******************************************************************************/
void HandleData(void* info, PolarisContext_t* polaris_context,
                const uint8_t* buffer, size_t size_bytes) {
  P1_printf("Application received %u bytes.\n", (unsigned)size_bytes);

  // Corrections are flowing, so forget about any earlier connection trouble.
  bytes_received += size_bytes;
  if (bytes_received > MIN_HEALTHY_BYTES) {
    retry_delay_sec = 0;
    reconnect_count = 0;
  }
}

/******************************************************************************/
void HandleSignal(int sig) {
  signal(sig, SIG_DFL);

  P1_printf("Caught signal %s (%d). Closing Polaris connection.\n",
            strsignal(sig), sig);
  running = 0;
  Polaris_Disconnect(&context);
}

/******************************************************************************/
/**
 * @brief Wait for the current backoff delay to elapse before reconnecting.
 *
 * The delay doubles on each successive failure, up to @ref MAX_RETRY_DELAY_SEC,
 * so that a persistent failure (no network connection, for instance) is not
 * retried in a tight loop.
 *
 * @return 1 if the caller should attempt to reconnect, or 0 if the user asked
 *         us to exit.
 */
int WaitForRetry(void) {
  if (retry_delay_sec == 0) {
    retry_delay_sec = MIN_RETRY_DELAY_SEC;
  } else if (retry_delay_sec < MAX_RETRY_DELAY_SEC) {
    retry_delay_sec *= 2;
    if (retry_delay_sec > MAX_RETRY_DELAY_SEC) {
      retry_delay_sec = MAX_RETRY_DELAY_SEC;
    }
  }

  P1_printf("Waiting %d seconds before reconnecting.\n", retry_delay_sec);
  for (int i = 0; i < retry_delay_sec && running; ++i) {
    sleep(1);
  }

  return running;
}

/******************************************************************************/
int main(int argc, const char* argv[]) {
  if (argc < 2 || argc > 4) {
    P1_fprintf(stderr,
               "Usage: %s API_KEY [UNIQUE_ID] [LOG_LEVEL (1=debug, 2=trace)]\n",
               argv[0]);
    return 1;
  }

  const char* api_key = argv[1];
  const char* unique_id = argc > 2 ? argv[2] : "device12345";

  int log_level = argc > 3 ? atoi(argv[3]) : POLARIS_LOG_LEVEL_INFO;
  Polaris_SetLogLevel(log_level);

  if (Polaris_Init(&context) != POLARIS_SUCCESS) {
    P1_printf("Error initializing context.\n");
    return 2;
  }

  Polaris_SetRTCMCallback(&context, HandleData, NULL);

  // Catch termination requests up front so the user can interrupt us at any
  // point, including while we are waiting to retry a failed connection.
  signal(SIGINT, HandleSignal);
  signal(SIGTERM, HandleSignal);

  int exit_code = 0;
  int auth_valid = 0;
  int first_attempt = 1;
  while (running) {
    if (!first_attempt && !WaitForRetry()) {
      break;
    }
    first_attempt = 0;

    // Retrieve an access token using the specified API key.
    if (!auth_valid) {
      P1_printf("Authenticating...\n");

      int ret = Polaris_Authenticate(&context, api_key, unique_id);
      if (ret == POLARIS_FORBIDDEN) {
        P1_printf("Authentication rejected. Is your API key valid?\n");
        exit_code = 3;
        break;
      } else if (ret == POLARIS_NO_NETWORK) {
        P1_printf("No network connection available. Retrying.\n");
        continue;
      } else if (ret != POLARIS_SUCCESS) {
        P1_printf("Authentication failed. Retrying.\n");
        continue;
      }

      auth_valid = 1;
    }

    // We now have a valid access token. Connect to the corrections service.
    //
    // In the calls below, if the connection times out or the access token is
    // rejected, try to connect again. If it fails too many times, the access
    // token may be expired - try reauthenticating.
    //
    // Note that we do not count a failure to establish the connection against
    // the reconnect limit. We never got far enough to learn anything about the
    // access token, so clearing it would not help.
    P1_printf("Authenticated. Connecting to Polaris...\n");

    int ret = Polaris_Connect(&context);
    if (ret == POLARIS_NO_NETWORK) {
      P1_printf("No network connection available. Retrying.\n");
      continue;
    } else if (ret != POLARIS_SUCCESS) {
      P1_printf("Error connecting to Polaris corrections stream. Retrying.\n");
      continue;
    }

    P1_printf("Connected to Polaris...\n");
    bytes_received = 0;

    // Send a position update to Polaris. Position updates are used to select an
    // appropriate corrections stream, and should be updated periodically as the
    // receiver moves.
    if (Polaris_SendLLAPosition(&context, 37.773971, -122.430996, -0.02) !=
        POLARIS_SUCCESS) {
      Polaris_Disconnect(&context);
      continue;
    }

    // Receive incoming RTCM data until the application exits.
    P1_printf("Sent position. Listening for data...\n");

    ret = Polaris_Run(&context, 30000);

    // The connection was closed by a call to Polaris_Disconnect() from our
    // signal handler, so we are done.
    if (ret == POLARIS_SUCCESS) {
      break;
    } else if (ret == POLARIS_CONNECTION_CLOSED) {
      P1_printf("Connection terminated remotely. Reconnecting.\n");
    } else if (ret == POLARIS_TIMED_OUT) {
      P1_printf("Connection timed out. Reconnecting.\n");
    } else {
      P1_printf("Unexpected error (%d). Reconnecting.\n", ret);
    }

    // The connection was established but then dropped, which may mean the
    // access token has expired. Reauthenticate if it keeps happening.
    if (++reconnect_count >= MAX_RECONNECTS) {
      P1_printf(
          "Max reconnects exceeded. Clearing access token and retrying "
          "authentication.\n");
      auth_valid = 0;
      reconnect_count = 0;
    }
  }

  Polaris_Free(&context);

  P1_printf("Finished.\n");

  return exit_code;
}
