/**************************************************************************/ /**
 * @brief Point One Navigation Polaris client support.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/polaris.h"

#include <errno.h>
#include <inttypes.h>  // For PRI*
#include <stdarg.h>    // For va_list support
#include <stdio.h>     // For sscanf() and snprintf()
#include <stdlib.h>    // For malloc()
#include <string.h>    // For memmove()

#ifndef P1_FREERTOS
#  include <fcntl.h>  // For fcntl()
#  include <time.h>   // For struct tm
#endif

#ifdef POLARIS_USE_TLS
#  include <openssl/err.h>
#  include <openssl/ssl.h>
#endif

#include "point_one/polaris/polaris_internal.h"
#include "point_one/polaris/portability.h"

#define POLARIS_NOT_AUTHENTICATED 0
#define POLARIS_AUTHENTICATED 1
#define POLARIS_AUTHENTICATION_SKIPPED 2

#define MAKE_STR(x) #x
#define STR(x) MAKE_STR(x)

#define P1_NOOP \
  do {          \
  } while (0)

#if P1_NO_PRINT
#  define P1_PrintMessage(level, x, ...) P1_NOOP
#  define P1_PrintErrno(x, ...) P1_NOOP

#  define P1_PrintData(buffer, length) P1_NOOP
#  if POLARIS_USE_TLS
#    define ShowCerts(ssl) \
      do {                 \
      } while (0)
#  endif
#else  // !P1_NO_PRINT
static int __log_level = POLARIS_LOG_LEVEL_INFO;

static PolarisPrintCallback_t __print_callback = NULL;

static int PrintTime(char* buffer, size_t capacity_bytes) {
  // Get the current _local_ time (not UTC).
  P1_TimeValue_t now;
  P1_GetCurrentTime(&now);
  uint64_t now_ms = P1_GetTimeMS(&now);
  now_ms += P1_GetUTCOffsetSec(&now) * 1000;

  unsigned sec_frac_ms = now_ms % 1000;
  now_ms /= 1000;
  unsigned sec = now_ms % 60;
  now_ms /= 60;
  unsigned min = now_ms % 60;
  now_ms /= 60;
  unsigned hour = now_ms % 24;

  // Where date is available, print the date in a format consistent with glog
  // used by the C++ client source code (YYYYMMDD).
  int length = 0;
#  ifndef P1_FREERTOS
  time_t now_sec = (time_t)now.tv_sec;
  struct tm local_time = *localtime(&now_sec);
  if (buffer) {
    length += snprintf(buffer, capacity_bytes, "%04u%02u%02u ",
                       1900 + local_time.tm_year, local_time.tm_mon + 1,
                       local_time.tm_mday);
    buffer += length;
    capacity_bytes -= length;
  } else {
    P1_fprintf(stderr, "%04u%02u%02u ", 1900 + local_time.tm_year,
               local_time.tm_mon + 1, local_time.tm_mday);
  }
#  endif

  // At a minimum, print HH:MM:SS.SSS. For FreeRTOS devices, this will likely be
  // elapsed time since boot.
  if (buffer) {
    length += snprintf(buffer, capacity_bytes, "%02u:%02u:%02u.%03u", hour, min,
                       sec, sec_frac_ms);
  } else {
    P1_fprintf(stderr, "%02u:%02u:%02u.%03u", hour, min, sec, sec_frac_ms);
  }

  return length;
}

static void P1_PrintToCallback(int line, int level, const char* format, ...) {
  char buffer[POLARIS_MAX_PRINT_LENGTH + 1];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);
  __print_callback("polaris.c", line, level, buffer);
}

#  define P1_DoPrint(line, level, x, ...)                       \
    if (__log_level >= level) {                                 \
      if (__print_callback) {                                   \
        P1_PrintToCallback(line, level, x, ##__VA_ARGS__);      \
      } else {                                                  \
        PrintTime(NULL, 0);                                     \
        P1_fprintf(stderr, " polaris.c:" STR(line) "] " x "\n", \
                   ##__VA_ARGS__);                              \
      }                                                         \
    }

#  define P1_PrintMessage(level, x, ...) \
    P1_DoPrint(__LINE__, level, x, ##__VA_ARGS__);

// The standard POSIX perror() does not include the numeric error code in the
// printout, which is often very useful, so we do not use it. We also can't use
// it anyway since we want to support using the print callback and perror()
// prints directly to stderr. We do try to make this macro at least similar to
// perror(), so it adds a trailing '.', etc.
//
// FreeRTOS does not use errno and does not support perror(). Instead, they
// include the error condition in the return code itself. To get around this,
// our macro also takes the function return code and prints it. For POSIX
// systems, the return code argument is ignored. While it theoretically could be
// omitted, it is required for FreeRTOS compilation.
#  ifdef P1_FREERTOS  // FreeRTOS
#    define P1_PrintErrnoLevel(level, x, ret) \
      P1_PrintMessage(level, x ". [error=%s (%d)]", strerror(-ret), ret)
#  else  // POSIX
#    define P1_PrintErrnoLevel(level, x, ret) \
      P1_PrintMessage(level, x ". [error=%s (%d)]", strerror(errno), errno)
#  endif  // End OS selection

#  define P1_PrintErrno(x, ret) \
    P1_PrintErrnoLevel(POLARIS_LOG_LEVEL_ERROR, x, ret)

static void P1_PrintData(const uint8_t* buffer, size_t length);
#  if POLARIS_USE_TLS
static void ShowCerts(SSL* ssl);
#  endif
#endif  // P1_NO_PRINT

#define P1_PrintError(x, ...) \
  P1_PrintMessage(POLARIS_LOG_LEVEL_ERROR, x, ##__VA_ARGS__)
#define P1_PrintWarning(x, ...) \
  P1_PrintMessage(POLARIS_LOG_LEVEL_WARNING, x, ##__VA_ARGS__)
#define P1_PrintInfo(x, ...) \
  P1_PrintMessage(POLARIS_LOG_LEVEL_INFO, x, ##__VA_ARGS__)
#define P1_PrintDebug(x, ...) \
  P1_PrintMessage(POLARIS_LOG_LEVEL_DEBUG, x, ##__VA_ARGS__)
#define P1_PrintTrace(x, ...) \
  P1_PrintMessage(POLARIS_LOG_LEVEL_TRACE, x, ##__VA_ARGS__)

#if POLARIS_NO_PRINT
#  define P1_PrintReadWriteError(context, x, ret) P1_NOOP
#  define P1_DebugPrintReadWriteError(context, x, ret) P1_NOOP
#  define P1_PrintSSLError(context, x, ret) P1_NOOP
#elif POLARIS_USE_TLS
static void __P1_PrintSSLError(int level, int line, PolarisContext_t* context,
                               const char* message, int ret) {
  SSL_load_error_strings();
  int ssl_error = SSL_get_error(context->ssl, ret);
  if (ssl_error == SSL_ERROR_SYSCALL) {
    P1_DoPrint(line, level, "%s. [error=%s (syscall: errno=%d; %s)]", message,
               strerror(errno), errno, ERR_error_string(ssl_error, NULL));
  } else {
    // Note: OpenSSL has a function sort of like strerror(), SSL_error_string(),
    // but in practice its output is less than helpful most of the time. We'll
    // still print it, but we'll also print out our own error name string:
    const char* error_name;
    switch (ssl_error) {
      case SSL_ERROR_NONE:
        error_name = "SSL_ERROR_NONE";
        break;
      case SSL_ERROR_SSL:
        error_name = "SSL_ERROR_SSL";
        break;
      case SSL_ERROR_WANT_READ:
        error_name = "SSL_ERROR_WANT_READ";
        break;
      case SSL_ERROR_WANT_WRITE:
        error_name = "SSL_ERROR_WANT_WRITE";
        break;
      case SSL_ERROR_WANT_X509_LOOKUP:
        error_name = "SSL_ERROR_WANT_X509_LOOKUP";
        break;
      // case SSL_ERROR_SYSCALL:
      //   error_name = "SSL_ERROR_SYSCALL";
      //   break;
      case SSL_ERROR_ZERO_RETURN:
        error_name = "SSL_ERROR_ZERO_RETURN";
        break;
      case SSL_ERROR_WANT_CONNECT:
        error_name = "SSL_ERROR_WANT_CONNECT";
        break;
      case SSL_ERROR_WANT_ACCEPT:
        error_name = "SSL_ERROR_WANT_ACCEPT";
        break;
      default:
        error_name = "<UNKNOWN>";
        break;
    }

    P1_DoPrint(line, level, "%s. [error=%s (%d; %s)]", message, error_name,
               ssl_error, ERR_error_string(ssl_error, NULL));
  }
}

#  define P1_PrintReadWriteError(context, x, ret) \
    __P1_PrintSSLError(POLARIS_LOG_LEVEL_ERROR, __LINE__, context, x, ret)
#  define P1_DebugPrintReadWriteError(context, x, ret)                        \
    if (__log_level >= POLARIS_LOG_LEVEL_DEBUG) {                             \
      __P1_PrintSSLError(POLARIS_LOG_LEVEL_DEBUG, __LINE__, context, x, ret); \
    }
#  define P1_PrintSSLError(context, x, ret) \
    __P1_PrintSSLError(POLARIS_LOG_LEVEL_ERROR, __LINE__, context, x, ret)
#else  // !POLARIS_NO_PRINT && !POLARIS_USE_TLS
#  define P1_PrintReadWriteError(context, x, ret) P1_PrintErrno(x, ret)
#  define P1_DebugPrintReadWriteError(context, x, ret)     \
    if (__log_level >= POLARIS_LOG_LEVEL_DEBUG) {          \
      P1_PrintErrnoLevel(POLARIS_LOG_LEVEL_DEBUG, x, ret); \
    }
#  define P1_PrintSSLError(context, x, ret) P1_NOOP
#endif  // POLARIS_NO_PRINT / POLARIS_USE_TLS

static int ValidateUniqueID(const char* unique_id);

static int OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port);

static int SendPOSTRequest(PolarisContext_t* context, const char* endpoint_url,
                           int endpoint_port, const char* address,
                           const void* content, size_t content_length);

static int GetHTTPResponse(PolarisContext_t* context);

static void CloseSocket(PolarisContext_t* context, int destroy_context);

/******************************************************************************/
int Polaris_Init(PolarisContext_t* context) {
  if (POLARIS_RECV_BUFFER_SIZE < POLARIS_MAX_HTTP_MESSAGE_SIZE) {
    P1_PrintWarning(
        "Warning: Receive buffer smaller than expected authentication "
        "response.");
  }

  if (POLARIS_SEND_BUFFER_SIZE < POLARIS_MAX_MESSAGE_SIZE) {
    P1_PrintWarning(
        "Warning: Send buffer smaller than max expected outbound packet.");
  }

  context->socket = P1_INVALID_SOCKET;
  context->auth_token[0] = '\0';
  context->authenticated = POLARIS_NOT_AUTHENTICATED;
  context->disconnected = 0;
  context->total_bytes_received = 0;
  context->data_request_sent = 0;
  context->rtcm_callback = NULL;
  context->rtcm_callback_info = NULL;

#ifdef POLARIS_USE_TLS
  context->ssl = NULL;
  context->ssl_ctx = NULL;

  SSL_library_init();
  OpenSSL_add_all_algorithms();
#endif

  return POLARIS_SUCCESS;
}

/******************************************************************************/
void Polaris_Free(PolarisContext_t* context) { CloseSocket(context, 1); }

/******************************************************************************/
void Polaris_SetLogLevel(int log_level) {
#if !POLARIS_NO_PRINT
  __log_level = log_level;
#endif
}

/******************************************************************************/
void Polaris_SetPrintCallback(PolarisPrintCallback_t callback) {
#if !POLARIS_NO_PRINT
  __print_callback = callback;
#endif
}

/******************************************************************************/
int Polaris_Authenticate(PolarisContext_t* context, const char* api_key,
                         const char* unique_id) {
  return Polaris_AuthenticateTo(context, api_key, unique_id, POLARIS_API_URL);
}

/******************************************************************************/
int Polaris_AuthenticateTo(PolarisContext_t* context, const char* api_key,
                           const char* unique_id, const char* api_url) {
  // Sanity check the inputs.
  if (strlen(api_key) == 0) {
    P1_PrintError("API key must not be empty.");
    return POLARIS_ERROR;
  }

  int ret = ValidateUniqueID(unique_id);
  if (ret != POLARIS_SUCCESS) {
    // ValidateUniqueID() will print an error.
    return ret;
  }

  // Send an auth request, then wait for the response containing the access
  // token.
  //
  // Note: We use the receive buffer to send the HTTP auth request since it's
  // much larger than typical packets that need to fit in the send buffer.
  static const char* AUTH_REQUEST_TEMPLATE =
      "{"
      "\"grant_type\": \"authorization_code\","
      "\"token_type\": \"bearer\","
      "\"authorization_code\": \"%s\","
      "\"unique_id\": \"%s\""
      "}";

  int content_size = snprintf((char*)context->recv_buffer,
                              POLARIS_RECV_BUFFER_SIZE, AUTH_REQUEST_TEMPLATE,
                              api_key, unique_id == NULL ? "" : unique_id);
  if (content_size < 0) {
    P1_PrintError("Error populating authentication request payload.");
    return POLARIS_NOT_ENOUGH_SPACE;
  }

  P1_PrintDebug("Sending auth request. [api_key=%.7s..., unique_id=%s, url=%s]",
                api_key, unique_id, api_url);
  context->auth_token[0] = '\0';
#ifdef POLARIS_USE_TLS
  int status_code = SendPOSTRequest(context, api_url, 443, "/api/v1/auth/token",
                                    context->recv_buffer, (size_t)content_size);
#else
  int status_code = SendPOSTRequest(context, api_url, 80, "/api/v1/auth/token",
                                    context->recv_buffer, (size_t)content_size);
#endif
  if (status_code < 0) {
    P1_PrintError("Error sending authentication request.");
    return status_code;
  }

  // Extract the auth token from the JSON response.
  if (status_code == 200) {
    const char* token_start =
        strstr((char*)context->recv_buffer, "\"access_token\":\"");
    if (token_start == NULL) {
      P1_PrintError("Authentication token not found in response.");
      return POLARIS_AUTH_ERROR;
    } else {
      token_start += 16;
      if (sscanf(token_start, "%" STR(POLARIS_MAX_TOKEN_SIZE) "[^\"]s",
                 context->auth_token) != 1) {
        P1_PrintError("Authentication token not found in response.");
        return POLARIS_AUTH_ERROR;
      } else {
        P1_PrintDebug("Received access token: %s", context->auth_token);
        return POLARIS_SUCCESS;
      }
    }
  } else if (status_code == 403) {
    P1_PrintError("Authentication failed. Please check your API key.");
    return POLARIS_FORBIDDEN;
  } else {
    P1_PrintError("Unexpected authentication response (%d).", status_code);
    return POLARIS_AUTH_ERROR;
  }
}

/******************************************************************************/
int Polaris_SetAuthToken(PolarisContext_t* context, const char* auth_token) {
  size_t length = strlen(auth_token);
  if (length == 0) {
    P1_PrintError("User-provided auth token must not be empty.");
    return POLARIS_ERROR;
  } else if (length > POLARIS_MAX_TOKEN_SIZE) {
    P1_PrintError("User-provided auth token is too long.");
    return POLARIS_NOT_ENOUGH_SPACE;
  } else {
    memcpy(context->auth_token, auth_token, length + 1);
    P1_PrintDebug("Using user-specified access token: %s", context->auth_token);
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_Connect(PolarisContext_t* context) {
#ifdef POLARIS_USE_TLS
  return Polaris_ConnectTo(context, POLARIS_ENDPOINT_URL,
                           POLARIS_ENDPOINT_TLS_PORT);
#else
  return Polaris_ConnectTo(context, POLARIS_ENDPOINT_URL,
                           POLARIS_ENDPOINT_PORT);
#endif
}

/******************************************************************************/
int Polaris_ConnectTo(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port) {
  if (context->auth_token[0] == '\0') {
    P1_PrintError("Error: Auth token not specified.");
    return POLARIS_AUTH_ERROR;
  }

  // Connect to the corrections endpoint.
  context->disconnected = 0;
  context->authenticated = POLARIS_NOT_AUTHENTICATED;
  context->total_bytes_received = 0;
  context->data_request_sent = 0;
  int ret = OpenSocket(context, endpoint_url, endpoint_port);
  if (ret != POLARIS_SUCCESS) {
    P1_PrintError("Error connecting to corrections endpoint: tcp://%s:%d.",
                  endpoint_url, endpoint_port);
    return ret;
  }

  // Send the auth token.
  //
  // Note: We use the receive buffer here to send the auth message since the
  // auth token is very large. We haven't authenticated yet, so no data will
  // be coming in and it should be fine to use the receive buffer.
  size_t token_length = strlen(context->auth_token);
  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->recv_buffer, POLARIS_ID_AUTH, token_length);
  memmove(header + 1, context->auth_token, token_length);
  size_t message_size = Polaris_PopulateChecksum(context->recv_buffer);

  P1_PrintDebug("Sending access token message. [size=%u B]",
                (unsigned)message_size);
#ifdef POLARIS_USE_TLS
  ret = SSL_write(context->ssl, context->recv_buffer, message_size);
#else
  ret = send(context->socket, context->recv_buffer, message_size, 0);
#endif
  if (ret != message_size) {
    P1_PrintReadWriteError(context, "Error sending authentication token", ret);
    CloseSocket(context, 1);
    return POLARIS_SEND_ERROR;
  }

  return POLARIS_SUCCESS;
}

/******************************************************************************/
int Polaris_ConnectWithoutAuth(PolarisContext_t* context,
                               const char* endpoint_url, int endpoint_port,
                               const char* unique_id) {
  // Sanity check the inputs.
  int ret = ValidateUniqueID(unique_id);
  if (ret != POLARIS_SUCCESS) {
    // ValidateUniqueID() will print an error.
    return ret;
  }

  // Connect to the corrections endpoint.
  context->disconnected = 0;
  context->authenticated = POLARIS_NOT_AUTHENTICATED;
  context->total_bytes_received = 0;
  context->data_request_sent = 0;
  ret = OpenSocket(context, endpoint_url, endpoint_port);
  if (ret != POLARIS_SUCCESS) {
    P1_PrintError("Error connecting to corrections endpoint: tcp://%s:%d.",
                  endpoint_url, endpoint_port);
    return ret;
  }

  // Send the unique ID.
  size_t id_length = unique_id == NULL ? 0 : strlen(unique_id);
  if (id_length != 0) {
    PolarisHeader_t* header = Polaris_PopulateHeader(
        context->send_buffer, POLARIS_ID_UNIQUE_ID, id_length);
    memmove(header + 1, unique_id, id_length);
    size_t message_size = Polaris_PopulateChecksum(context->send_buffer);

    P1_PrintDebug("Sending unique ID message. [size=%u B]",
                  (unsigned)message_size);
#ifdef POLARIS_USE_TLS
    ret = SSL_write(context->ssl, context->send_buffer, message_size);
#else
    ret = send(context->socket, context->send_buffer, message_size, 0);
#endif
    if (ret != message_size) {
      P1_PrintReadWriteError(context, "Error sending unique ID", ret);
      CloseSocket(context, 1);
      return POLARIS_SEND_ERROR;
    }
  }

  context->authenticated = POLARIS_AUTHENTICATION_SKIPPED;

  return POLARIS_SUCCESS;
}

/******************************************************************************/
void Polaris_Disconnect(PolarisContext_t* context) {
  if (context->socket != P1_INVALID_SOCKET) {
    P1_PrintDebug("Closing Polaris connection.");
    context->disconnected = 1;
#ifdef POLARIS_USE_TLS
    SSL_shutdown(context->ssl);
#endif
    shutdown(context->socket, SHUT_RDWR);
    // Note: We intentionally close the socket here but do _not_ destroy the SSL
    // context since Polaris_Work() may be suspended on it and will segfault if
    // the memory is freed. Polaris_Work() and Polaris_Run() will free it when
    // they return. If they are not currently being called, the user should call
    // Polaris_Free() to free the context.
    CloseSocket(context, 0);
  }
}

/******************************************************************************/
void Polaris_SetRTCMCallback(PolarisContext_t* context,
                             PolarisCallback_t callback, void* callback_info) {
  context->rtcm_callback = callback;
  context->rtcm_callback_info = callback_info;
}

/******************************************************************************/
int Polaris_SendECEFPosition(PolarisContext_t* context, double x_m, double y_m,
                             double z_m) {
  if (context->socket == P1_INVALID_SOCKET) {
    P1_PrintError("Error: Polaris connection not currently open.");
    return POLARIS_SOCKET_ERROR;
  }
#ifdef POLARIS_USE_TLS
  else if (context->ssl_ctx == NULL || context->ssl == NULL) {
    P1_PrintError("Error: Polaris SSL context not available.");
    return POLARIS_SOCKET_ERROR;
  }
#endif

  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->send_buffer, POLARIS_ID_ECEF, sizeof(PolarisECEFMessage_t));
  PolarisECEFMessage_t* payload = (PolarisECEFMessage_t*)(header + 1);
  payload->x_cm = htole32((int32_t)(x_m * 1e2));
  payload->y_cm = htole32((int32_t)(y_m * 1e2));
  payload->z_cm = htole32((int32_t)(z_m * 1e2));
  size_t message_size = Polaris_PopulateChecksum(context->send_buffer);

#ifdef P1_FREERTOS
  // Floating point printf() not available in FreeRTOS.
  P1_PrintDebug("Sending ECEF position. [size=%u B, position=[%d, %d, %d] cm]",
                (unsigned)message_size, le32toh(payload->x_cm),
                le32toh(payload->y_cm), le32toh(payload->z_cm));
#else
  P1_PrintDebug(
      "Sending ECEF position. [size=%u B, position=[%.2f, %.2f, %.2f]]",
      (unsigned)message_size, x_m, y_m, z_m);
#endif
  P1_PrintData(context->send_buffer, message_size);

#ifdef POLARIS_USE_TLS
  int ret = SSL_write(context->ssl, context->send_buffer, message_size);
#else
  int ret = send(context->socket, context->send_buffer, message_size, 0);
#endif

  if (ret != message_size) {
    P1_PrintReadWriteError(context, "Error sending ECEF position", ret);
    return POLARIS_SEND_ERROR;
  } else {
    context->data_request_sent = 1;
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_SendLLAPosition(PolarisContext_t* context, double latitude_deg,
                            double longitude_deg, double altitude_m) {
  if (context->socket == P1_INVALID_SOCKET) {
    P1_PrintError("Error: Polaris connection not currently open.");
    return POLARIS_SOCKET_ERROR;
  }
#ifdef POLARIS_USE_TLS
  else if (context->ssl_ctx == NULL || context->ssl == NULL) {
    P1_PrintError("Error: Polaris SSL context not available.");
    return POLARIS_SOCKET_ERROR;
  }
#endif

  // TODO Cache double buffer and flip flop, then send in Work()
  //  - If we do this can we take out the mutex in polaris_client.cc?

  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->send_buffer, POLARIS_ID_LLA, sizeof(PolarisLLAMessage_t));
  PolarisLLAMessage_t* payload = (PolarisLLAMessage_t*)(header + 1);
  payload->latitude_dege7 = htole32((int32_t)(latitude_deg * 1e7));
  payload->longitude_dege7 = htole32((int32_t)(longitude_deg * 1e7));
  payload->altitude_mm = htole32((int32_t)(altitude_m * 1e3));
  size_t message_size = Polaris_PopulateChecksum(context->send_buffer);

#ifdef P1_FREERTOS
  // Floating point printf() not available in FreeRTOS.
  P1_PrintDebug(
      "Sending LLA position. [size=%u B, position=[%d.0e-7, %d.0e-7, %d]]",
      (unsigned)message_size, le32toh(payload->latitude_dege7),
      le32toh(payload->longitude_dege7), le32toh(payload->altitude_mm));
#else
  P1_PrintDebug(
      "Sending LLA position. [size=%u B, position=[%.6f, %.6f, %.2f]]",
      (unsigned)message_size, latitude_deg, longitude_deg, altitude_m);
#endif
  P1_PrintData(context->send_buffer, message_size);

#ifdef POLARIS_USE_TLS
  int ret = SSL_write(context->ssl, context->send_buffer, message_size);
#else
  int ret = send(context->socket, context->send_buffer, message_size, 0);
#endif

  if (ret != message_size) {
    P1_PrintReadWriteError(context, "Error sending LLA position", ret);
    return POLARIS_SEND_ERROR;
  } else {
    context->data_request_sent = 1;
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_RequestBeacon(PolarisContext_t* context, const char* beacon_id) {
  if (context->socket == P1_INVALID_SOCKET) {
    P1_PrintError("Error: Polaris connection not currently open.");
    return POLARIS_SOCKET_ERROR;
  }
#ifdef POLARIS_USE_TLS
  else if (context->ssl_ctx == NULL || context->ssl == NULL) {
    P1_PrintError("Error: Polaris SSL context not available.");
    return POLARIS_SOCKET_ERROR;
  }
#endif

  size_t id_length = strlen(beacon_id);
  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->send_buffer, POLARIS_ID_BEACON, id_length);
  memmove(header + 1, beacon_id, id_length);
  size_t message_size = Polaris_PopulateChecksum(context->send_buffer);

  P1_PrintDebug("Sending beacon request. [size=%u B, beacon='%s']",
                (unsigned)message_size, beacon_id);
  P1_PrintData(context->send_buffer, message_size);

#ifdef POLARIS_USE_TLS
  int ret = SSL_write(context->ssl, context->send_buffer, message_size);
#else
  int ret = send(context->socket, context->send_buffer, message_size, 0);
#endif

  if (ret != message_size) {
    P1_PrintReadWriteError(context, "Error sending beacon request", ret);
    return POLARIS_SEND_ERROR;
  } else {
    context->data_request_sent = 1;
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_Work(PolarisContext_t* context) {
  // The following should be unlikely to happen, but we call CloseSocket() just
  // in case this function is called after Polaris_Disconnect() to clean up the
  // SSL session.
  if (context->disconnected) {
    P1_PrintDebug("Connection terminated by user request.");
    CloseSocket(context, 1);
    return POLARIS_CONNECTION_CLOSED;
  } else if (context->socket == P1_INVALID_SOCKET) {
    P1_PrintError("Error: Polaris connection not currently open.");
    CloseSocket(context, 1);
    return POLARIS_SOCKET_ERROR;
  }

  P1_PrintDebug("Listening for data block.");

#ifdef POLARIS_USE_TLS
  P1_RecvSize_t bytes_read =
      SSL_read(context->ssl, context->recv_buffer, POLARIS_RECV_BUFFER_SIZE);
#else
  P1_RecvSize_t bytes_read =
      recv(context->socket, context->recv_buffer, POLARIS_RECV_BUFFER_SIZE, 0);
#endif

#ifdef P1_FREERTOS
  // FreeRTOS does not use errno for recv() errors, but instead returns the
  // equivalent error value. On a socket timeout, it returns 0 instead of
  // ETIMEDOUT.
  //
  // We try to mimick the actual POSIX behavior to simplify the code below, so
  // we set bytes_read to -1 and set errno accordingly.
  //
  // Later, we will need to restore them both before calling
  // P1_PrintReadWriteError(). That function and P1_PrintErrno(), which it calls
  // if TLS is disabled, expect to be provided the return code, not errno.
  P1_RecvSize_t original_bytes_read = bytes_read;
  int original_errno = errno;
  if (bytes_read == 0) {
    bytes_read = -1;
    errno = ETIMEDOUT;
  } else if (bytes_read < 0) {
    bytes_read = -1;
    errno = (int)bytes_read;
  }
#endif

  // Did we hit a read timeout? This is normal behavior (e.g., brief loss of
  // cell service) and is not considered an error. Most receivers can handle
  // small gaps in corrections data. We do not close the socket here.
  //
  // Note that in this context the timeout is the socket receive timeout
  // (POLARIS_RECV_TIMEOUT_MS; typically a few seconds). This is not the same as
  // the longer connection timeout used by Polaris_Run() to decide if the
  // connection was lost upstream (typically 30 seconds or longer).
  if (bytes_read < 0) {
#ifdef POLARIS_USE_TLS
    int ssl_error = SSL_get_error(context->ssl, bytes_read);
    if (ssl_error == SSL_ERROR_WANT_READ || ssl_error == SSL_ERROR_WANT_WRITE ||
        ssl_error == SSL_ERROR_WANT_X509_LOOKUP ||
        (ssl_error == SSL_ERROR_SYSCALL &&
         (errno == EAGAIN || errno == EWOULDBLOCK || errno == ETIMEDOUT))) {
#else
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == ETIMEDOUT) {
#endif

#ifdef P1_FREERTOS
      bytes_read = original_bytes_read;
      errno = original_errno;
#endif
      if (context->disconnected) {
        P1_DebugPrintReadWriteError(context, "Socket read timed out",
                                    bytes_read);
      } else {
        P1_PrintReadWriteError(context, "Warning: Socket read timed out",
                               bytes_read);
      }

      return POLARIS_TIMED_OUT;
    }
  }

  // Check if we hit an error condition (<0) or a shutdown (0).
  //
  // If we hit an error, it will typically be ENOTCONN (i.e., socket closed) or
  // EINTR, but could be another error condition.
  //
  // If recv() returns 0, the socket had an "orderly shutdown", i.e., it was
  // either closed by user request or disconnected upstream by the Polaris
  // service, and it was not necessary to interrupt an existing recv() call.
  //
  if (bytes_read <= 0) {
    // Was the connection closed by the user?
    if (context->disconnected) {
      if (bytes_read == 0 || errno == EINTR || errno == ENOTCONN) {
        P1_PrintDebug("Connection terminated by user request.");
      } else {
#ifdef P1_FREERTOS
        bytes_read = original_bytes_read;
        errno = original_errno;
#endif
        P1_PrintReadWriteError(
            context,
            "Warning: Connection terminated by user request; unexpected error",
            bytes_read);
      }
    }
    // If the connection was closed by the server, print the reason.
    else {
      if (bytes_read == 0) {
        // Note that this is considered unexpected, even though the server
        // closed the connection without error. Once connected, we expect the
        // stream to stay open indefinitely under normal circumstances. The
        // server should not have a reason to terminate the connection.
        P1_PrintWarning("Warning: Connection terminated remotely.");
      } else {
#ifdef P1_FREERTOS
        bytes_read = original_bytes_read;
        errno = original_errno;
#endif
        P1_PrintReadWriteError(
            context, "Warning: Connection terminated remotely", bytes_read);
      }
    }

    // If we connected to the server but never got any data, it was likely:
    // 1. The user failed to send either a position (that is within range of a
    //    base station in the network) or a request for a specific base station
    // 2. The authentication token was rejected and socket was closed remotely
    //
    // Note that in general for case (2), we do expect the server to send an
    // RTCM 1029 message indicating the reason for failure. That would be
    // handled as "data received but not authenticated" (authenticated == 0),
    // followed by EINTR or a similar condition above. total_bytes_received will
    // not be 0.
    //
    // Similarly, if we received something from the server but no position or
    // beacon request has been sent, the data we received is likely an
    // authentication error. This too should be treated as "data received but
    // not authenticated" (authenticated == 0).
    int ret = POLARIS_CONNECTION_CLOSED;
    if (context->authenticated == POLARIS_AUTHENTICATED) {
      // Note that currently, we only declare authenticated == 1 after we
      // receive enough data to confirm we did not get an error 1029 response,
      // so we do not expect to get here if the user did not send either a
      // position or base station request.
      //
      // No warning needed. If there was a socket error, we'll have printed a
      // warning above.
    }
    // Authentication skipped and data received.
    else if (context->total_bytes_received > 0 &&
             context->authenticated == POLARIS_AUTHENTICATION_SKIPPED) {
      // No warning necessary.
    }
    // Not authenticated, but response received: likely server indicated an
    // error.
    else if (context->total_bytes_received > 0) {
      P1_PrintWarning(
          "Warning: Polaris connection closed with an error response. Is "
          "your authentication token valid?");
      ret = POLARIS_FORBIDDEN;
    }
    // Data request sent but no response received: authentication (presumably)
    // accepted but position likely out of network.
    else if (context->data_request_sent) {
      P1_PrintWarning(
          "Warning: Polaris connection closed and no response received "
          "from server.");
    }
    // Data request not sent and no response received: authentication
    // (presumably) accepted.
    else {
      P1_PrintWarning(
          "Warning: Polaris connection closed and no position or beacon "
          "request issued.");
    }

    CloseSocket(context, 1);
    return ret;
  } else {
    context->total_bytes_received += bytes_read;
    P1_PrintDebug("Received %u bytes. [%" PRIu64 " bytes total]",
                  (unsigned)bytes_read,
                  (uint64_t)context->total_bytes_received);
    P1_PrintData(context->recv_buffer, bytes_read);

    // We do not consider the connection authenticated (auth token valid and
    // accepted by the network) until after we begin receiving data. It the auth
    // token is rejected, the network may respond with an RTCM 1029 text message
    // indicating the reason. However, we do not currently parse the incoming
    // RTCM stream, so instead we simply wait until we've received more than the
    // maximum 1029 message size.
    //
    // If the user never sends a position or beacon request to the network, no
    // data will be received. That does not necessarily imply an authentication
    // failure.
    if (!context->authenticated && context->total_bytes_received > 270) {
      P1_PrintDebug("Sufficient data received. Authentication token accepted.");
      context->authenticated = POLARIS_AUTHENTICATED;
    }

    // We don't interpret the incoming RTCM data, so there's no need to buffer
    // it up to a complete RTCM frame. We'll just forward what we got along.
    if (context->rtcm_callback) {
      context->rtcm_callback(context->rtcm_callback_info, context,
                             context->recv_buffer, bytes_read);
    }

    if (context->disconnected) {
      P1_PrintDebug("Connection terminated by user.");
      return POLARIS_SUCCESS;
    } else {
      return (int)bytes_read;
    }
  }
}

/******************************************************************************/
int Polaris_Run(PolarisContext_t* context, int connection_timeout_ms) {
  // The following should be unlikely to happen, but we call CloseSocket() just
  // in case this function is called after Polaris_Disconnect() to clean up the
  // SSL session.
  if (context->disconnected) {
    P1_PrintDebug("Connection terminated by user request.");
    CloseSocket(context, 1);
    return POLARIS_SUCCESS;
  } else if (context->socket == P1_INVALID_SOCKET) {
    P1_PrintError("Error: Polaris connection not currently open.");
    CloseSocket(context, 1);
    return POLARIS_SOCKET_ERROR;
  }

  P1_PrintDebug("Listening for data.");

  P1_TimeValue_t last_read_time;
  P1_GetCurrentTime(&last_read_time);

  int ret = POLARIS_ERROR;
  while (1) {
    // Read the next data block.
    ret = Polaris_Work(context);

    // Check if the read timed out. This can happen if the client briefly loses
    // cell coverage, etc., so we do not consider short gaps an error. See if
    // we've hit the longer connection timeout and, if so, close the connection.
    // Otherwise, try again.
    //
    // We treat 0 bytes as a read timeout condition just to be safe, but in
    // practice Polaris_Work() should handle all timeout, error, and connection
    // closed conditions and this should not happen.
    if (ret == POLARIS_TIMED_OUT || ret == 0) {
      P1_TimeValue_t current_time;
      P1_GetCurrentTime(&current_time);
      int elapsed_ms = P1_GetElapsedMS(&last_read_time, &current_time);
      P1_PrintDebug("%d ms elapsed since last data arrived.", elapsed_ms);
      if (elapsed_ms >= connection_timeout_ms) {
        P1_PrintWarning("Warning: Connection timed out after %d ms.",
                        elapsed_ms);
        CloseSocket(context, 1);
        ret = POLARIS_TIMED_OUT;
        break;
      }
    }
    // If an error occurred or the connection was terminated, break. The socket
    // will have already been closed and a debug message printed by
    // Polaris_Work().
    else if (ret < 0) {
      break;
    }
    // Data received and dispatched to the callback.
    else {
      P1_GetCurrentTime(&last_read_time);

      if (context->disconnected) {
        P1_PrintDebug("Connection terminated by user.");
        CloseSocket(context, 1);
        ret = POLARIS_SUCCESS;
        break;
      }
    }
  }

  P1_PrintDebug("Received %" PRIu64 " total bytes.",
                (uint64_t)context->total_bytes_received);
  if (ret == POLARIS_CONNECTION_CLOSED && context->disconnected) {
    return POLARIS_SUCCESS;
  } else {
    return ret;
  }
}

/******************************************************************************/
static int ValidateUniqueID(const char* unique_id) {
  size_t length = unique_id == NULL ? 0 : strlen(unique_id);
  if (length == 0) {
    // Unique IDs are optional (but recommended). If unspecified, an ID will be
    // autogenerated on the back end.
    return POLARIS_SUCCESS;
  } else if (length > POLARIS_MAX_UNIQUE_ID_SIZE) {
    P1_PrintError("Unique ID must be a maximum of %d characters. [id='%s']",
                  POLARIS_MAX_UNIQUE_ID_SIZE, unique_id);
    return POLARIS_ERROR;
  } else {
    for (const char* ptr = unique_id; *ptr != '\0'; ++ptr) {
      char c = *ptr;
      if (c != '-' && c != '_' && (c < 'A' || c > 'Z') &&
          (c < 'a' || c > 'z') && (c < '0' || c > '9')) {
        P1_PrintError("Invalid unique ID specified. [id='%s']", unique_id);
        return POLARIS_ERROR;
      }
    }

    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
#ifdef P1_FREERTOS
static inline int P1_SetAddress(const char* hostname, int port,
                                P1_SocketAddrV4_t* result) {
  uint32_t ip = FreeRTOS_gethostbyname(hostname);
  if (ip == 0) {
    return -1;
  } else {
    result->sin_family = AF_INET;
    result->sin_port = FreeRTOS_htons(port);
    result->sin_addr = ip;
    return 0;
  }
}
#else
static inline int P1_SetAddress(const char* hostname, int port,
                                P1_SocketAddrV4_t* result) {
  struct hostent* host_info = gethostbyname(hostname);
  if (host_info == NULL) {
    P1_PrintError("Unable to resolve \"%s\": %s", hostname, hstrerror(h_errno));
    return -1;
  }
  // IPv6 not currently supported by the API.
  else if (host_info->h_addrtype != AF_INET) {
    P1_PrintWarning(
        "Warning: DNS lookup for \"%s\" returned an IPv6 address (not "
        "supported).",
        hostname);
    // Explicitly set h_errno to _something_ (gethostbyname() doesn't touch
    // h_errno on success).
    h_errno = NO_RECOVERY;
    return -2;
  } else {
    result->sin_family = AF_INET;
    result->sin_port = htons(port);
    memcpy(&result->sin_addr, host_info->h_addr_list[0], host_info->h_length);
    return 0;
  }
}
#endif

/******************************************************************************/
static int OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port) {
  // Is the connection already open?
  if (context->socket != P1_INVALID_SOCKET) {
    P1_PrintError("Error: socket already open.");
    return POLARIS_ERROR;
  }
#ifdef POLARIS_USE_TLS
  else if (context->ssl_ctx != NULL || context->ssl != NULL) {
    P1_PrintError("Error: SSL context not freed.");
    return POLARIS_ERROR;
  }
#endif

#ifdef POLARIS_USE_TLS
  // Configure TLS.
  P1_PrintDebug("Configuring TLS context.");
#  if OPENSSL_VERSION_NUMBER < 0x10100000L
  context->ssl_ctx = SSL_CTX_new(TLSv1_2_client_method());
#  else
  context->ssl_ctx = SSL_CTX_new(TLS_client_method());
#  endif
  // we specifically disable older insecure protocols
  SSL_CTX_set_options(context->ssl_ctx, SSL_OP_NO_SSLv2);
  SSL_CTX_set_options(context->ssl_ctx, SSL_OP_NO_SSLv3);
  SSL_CTX_set_options(context->ssl_ctx, SSL_OP_NO_TLSv1);

  if (context->ssl_ctx == NULL) {
    P1_PrintError("SSL context failed to initialize.");
    return POLARIS_ERROR;
  }
#endif

  // Open a socket.
  context->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (context->socket < 0) {
    P1_PrintErrno("Error creating socket", context->socket);
    CloseSocket(context, 1);
    return POLARIS_SOCKET_ERROR;
  }

  P1_PrintDebug(
      "Configuring socket. [socket=%d, read_timeout=%d ms, send_timeout=%d "
      "ms]",
      context->socket, POLARIS_RECV_TIMEOUT_MS, POLARIS_SEND_TIMEOUT_MS);

  // Set send/receive timeouts.
  P1_TimeValue_t timeout;
  P1_SetTimeMS(POLARIS_RECV_TIMEOUT_MS, &timeout);
  setsockopt(context->socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
             sizeof(timeout));
  P1_SetTimeMS(POLARIS_SEND_TIMEOUT_MS, &timeout);
  setsockopt(context->socket, SOL_SOCKET, SO_SNDTIMEO, &timeout,
             sizeof(timeout));

#ifndef P1_FREERTOS
  int flags = fcntl(context->socket, F_GETFL);
  P1_PrintDebug("Socket flags: 0x%08x", flags);
#endif  // P1_FREERTOS

  // Lookup the IP of the endpoint used for auth requests.
  P1_PrintDebug("Performing DNS lookup for '%s'.", endpoint_url);
  P1_SocketAddrV4_t address;
  if (P1_SetAddress(endpoint_url, endpoint_port, &address) < 0) {
#ifdef P1_FREERTOS
    P1_PrintError("Error locating address '%s'.", endpoint_url);
#else
    P1_PrintError("Error locating address '%s'. [error=%s (%d)]", endpoint_url,
                  hstrerror(h_errno), h_errno);
#endif
    CloseSocket(context, 1);
    return POLARIS_SOCKET_ERROR;
  }

  // Connect to the server.
#ifdef P1_FREERTOS
  uint32_t ip_host_endian = ntohl(address.sin_addr);
#else
  uint32_t ip_host_endian = ntohl(address.sin_addr.s_addr);
#endif
  P1_PrintDebug("Connecting to 'tcp://%d.%d.%d.%d:%d'.",
                (ip_host_endian >> 24) & 0xFF, (ip_host_endian >> 16) & 0xFF,
                (ip_host_endian >> 8) & 0xFF, ip_host_endian & 0xFF,
                endpoint_port);
  int ret =
      connect(context->socket, (P1_SocketAddr_t*)&address, sizeof(address));
  if (ret < 0) {
    P1_PrintErrno("Error connecting to endpoint", ret);
    CloseSocket(context, 1);
    return POLARIS_SOCKET_ERROR;
  }
  P1_PrintDebug("Connected successfully.");

#ifdef POLARIS_USE_TLS
  // Create new SSL connection state and attach the socket.
  P1_PrintDebug("Establishing TLS connection.");
  context->ssl = SSL_new(context->ssl_ctx);
  SSL_set_fd(context->ssl, context->socket);

  // Enable the TLS extension for servers that use SNI, explicitly telling it
  // the hostname of the remote server.
  SSL_set_tlsext_host_name(context->ssl, endpoint_url);

  // Perform SSL handhshake.
  ret = SSL_connect(context->ssl);
  if (ret != 1) {
#  if !P1_NO_PRINT
    // Note: We intentionally reuse the receive buffer to store the error
    // message to be displayed to avoid requiring additional stack here. At this
    // point we're trying to open the socket, so there should be nobody actively
    // using the receive buffer.
    snprintf((char*)context->recv_buffer, POLARIS_RECV_BUFFER_SIZE,
             "TLS handshake failed for tcp://%s:%d", endpoint_url,
             endpoint_port);
    P1_PrintSSLError(context, (char*)context->recv_buffer, ret);
#  endif  // !P1_NO_PRINT
    CloseSocket(context, 1);
    return POLARIS_SOCKET_ERROR;
  }

  const SSL_CIPHER* c = SSL_get_current_cipher(context->ssl);
  if (c == NULL) {
    P1_PrintError(
        "Server failed to negotiate encryption cipher. Verify that endpoint "
        "tcp://%s:%d supports TLS 1.2 or better.",
        endpoint_url, endpoint_port);
    CloseSocket(context, 1);
    return POLARIS_ERROR;
  }

  P1_PrintDebug("Connected with %s encryption.", SSL_get_cipher(context->ssl));
  ShowCerts(context->ssl);
#endif

  return POLARIS_SUCCESS;
}

/******************************************************************************/
void CloseSocket(PolarisContext_t* context, int destroy_context) {
#ifdef POLARIS_USE_TLS
  if (destroy_context && context->ssl != NULL) {
    P1_PrintDebug("Shutting down TLS context.");
    if (SSL_get_shutdown(context->ssl) == 0) {
      SSL_shutdown(context->ssl);
    }

    SSL_free(context->ssl);
    context->ssl = NULL;
  }
#endif

  if (context->socket != P1_INVALID_SOCKET) {
    P1_PrintDebug("Closing socket.");
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
  }

#ifdef POLARIS_USE_TLS
  if (destroy_context && context->ssl_ctx != NULL) {
    P1_PrintDebug("Freeing TLS context.");
    SSL_CTX_free(context->ssl_ctx);
    context->ssl_ctx = NULL;
  }
#endif

  // Note: We do not clear any of the authenticated, disconnected, etc. flags
  // here since it is used to determine the return value in Polaris_Run()
  // _after_ Polaris_Work() has returned and may have closed the socket.
}

/******************************************************************************/
static int SendPOSTRequest(PolarisContext_t* context, const char* endpoint_url,
                           int endpoint_port, const char* address,
                           const void* content, size_t content_length) {
  // Calculate the expected header length.
  static const char* HEADER_TEMPLATE =
      "POST %s HTTP/1.1\r\n"
      "Host: %s:%s\r\n"
      "Content-Type: application/json; charset=utf-8\r\n"
      "Content-Length: %s\r\n"
      "Connection: Close\r\n"
      "\r\n";
  static size_t HEADER_TEMPLATE_SIZE = 0;
  if (HEADER_TEMPLATE_SIZE == 0) {
    HEADER_TEMPLATE_SIZE = strlen(HEADER_TEMPLATE) - (4 * 2);
  }

  size_t address_size = strlen(address);

  size_t url_size = strlen(endpoint_url);

  char port_str[6] = {0};
  size_t port_str_size =
      (size_t)snprintf(port_str, sizeof(port_str), "%d", endpoint_port);

  char content_length_str[6] = {0};
  size_t content_length_str_size =
      (size_t)snprintf(content_length_str, sizeof(content_length_str), "%d",
                       (int)content_length);

  int header_size = (int)(HEADER_TEMPLATE_SIZE + address_size + url_size +
                          port_str_size + content_length_str_size);

  // Copy the payload before building the header. That way we don't accidentally
  // overwrite the payload if it is stored inline in the output buffer.
  //
  // Note that we use the receive buffer to send HTTP requests since it is
  // larger than the send buffer. We currently only send HTTP requests during
  // authentication, before data is coming in.
  if (POLARIS_RECV_BUFFER_SIZE < header_size + content_length + 1) {
    P1_PrintError("Error populating POST request: buffer too small.");
    CloseSocket(context, 1);
    return POLARIS_NOT_ENOUGH_SPACE;
  }

  uint8_t first_content_byte =
      (content_length == 0 ? '\0' : ((const uint8_t*)content)[0]);
  memmove(context->recv_buffer + header_size, content, content_length);
  context->recv_buffer[header_size + content_length] = '\0';

  // Now populate the header.
  header_size =
      snprintf((char*)context->recv_buffer, header_size + 1, HEADER_TEMPLATE,
               address, endpoint_url, port_str, content_length_str);
  if (header_size < 0) {
    // This shouldn't happen.
    P1_PrintError("Error populating POST request.");
    CloseSocket(context, 1);
    return POLARIS_ERROR;
  }

  context->recv_buffer[header_size] = first_content_byte;

  size_t message_size = header_size + content_length;

  // Send the request.
  int ret = OpenSocket(context, endpoint_url, endpoint_port);
  if (ret != POLARIS_SUCCESS) {
    return ret;
  }

  P1_PrintDebug("Sending POST request. [size=%u B]", (unsigned)message_size);
#ifdef POLARIS_USE_TLS
  ret = SSL_write(context->ssl, context->recv_buffer, message_size);
#else
  ret = send(context->socket, context->recv_buffer, message_size, 0);
#endif

  if (ret != message_size) {
    P1_PrintReadWriteError(context, "Error sending POST request", ret);
    CloseSocket(context, 1);
    return POLARIS_SEND_ERROR;
  }

  // Wait for a response.
  return GetHTTPResponse(context);
}

/******************************************************************************/
static int GetHTTPResponse(PolarisContext_t* context) {
  // Read until the connection is closed.
  size_t total_bytes = 0;
  int bytes_read;

#ifdef POLARIS_USE_TLS
  while (
      (bytes_read = SSL_read(context->ssl, context->recv_buffer + total_bytes,
                             POLARIS_RECV_BUFFER_SIZE - total_bytes - 1)) > 0) {
#else
  while ((bytes_read = recv(context->socket, context->recv_buffer + total_bytes,
                            POLARIS_RECV_BUFFER_SIZE - total_bytes - 1, 0)) >
         0) {
#endif
    total_bytes += (size_t)bytes_read;
    if (total_bytes == POLARIS_RECV_BUFFER_SIZE - 1) {
      break;
    }
  }

#ifdef P1_FREERTOS
  // Unlike POSIX recv(), which returns <0 and sets ETIMEDOUT on a socket read
  // timeout, FreeRTOS returns 0.
  if (bytes_read == 0) {
    P1_PrintWarning("Socket timed out waiting for HTTP response.");
    CloseSocket(context, 1);
    return POLARIS_SEND_ERROR;
  }
  // Similarly, FreeRTOS returns -pdFREERTOS_ERRNO_ENOTCONN on an orderly socket
  // shutdown rather than 0. Any other <0 return is considered an error.
  else if (bytes_read != -pdFREERTOS_ERRNO_ENOTCONN) {
    P1_PrintReadWriteError(context,
                           "Unexpected error while waiting for HTTP response",
                           bytes_read);
    CloseSocket(context, 1);
    return POLARIS_SEND_ERROR;
  }
#else
  // Check for socket read errors. Under normal circumstances, recv() should
  // return 0 when finished, indicating the HTTP server closed the connection.
  if (bytes_read < 0) {
    P1_PrintReadWriteError(context,
                           "Unexpected error while waiting for HTTP response",
                           bytes_read);
    CloseSocket(context, 1);
    return POLARIS_SEND_ERROR;
  }
#endif

  // Note: Ideally we'd do this just once up above before any of the error
  // checks instead of having to do it explicitly in all of the if statements.
  // However, when TLS is enabled, P1_PrintReadWriteError() needs to access
  // context->ssl, which is freed by CloseSocket().
  CloseSocket(context, 1);

  P1_PrintDebug("Received HTTP response. [size=%u B]", (unsigned)total_bytes);

  // Append a null terminator to the response.
  context->recv_buffer[total_bytes++] = '\0';

  // Extract the status code.
  int status_code;
  if (sscanf((char*)context->recv_buffer, "HTTP/1.1 %d", &status_code) != 1) {
    P1_PrintError("Invalid HTTP response:\n\n%s", context->recv_buffer);
    return POLARIS_SEND_ERROR;
  }

  // Find the content, then move the response content to the front of the
  // recv_buffer. We don't care about the HTTP headers.
  char* content_start = strstr((char*)context->recv_buffer, "\r\n\r\n");
  if (content_start != NULL) {
    content_start += 4;
    size_t content_length =
        total_bytes - (content_start - (char*)context->recv_buffer);
    memmove(context->recv_buffer, content_start, content_length);
    P1_PrintDebug("Response content:\n%s", context->recv_buffer);
  } else {
    // No content in response.
    P1_PrintDebug("No content in response.");
    context->recv_buffer[0] = '\0';
  }

  return status_code;
}

/******************************************************************************/
#if !P1_NO_PRINT
void P1_PrintData(const uint8_t* buffer, size_t length) {
  if (__log_level < POLARIS_LOG_LEVEL_TRACE) {
    return;
  }

  char str[64] = {'\0'};
  int str_length = 0;
  for (size_t i = 0; i < length; ++i) {
    if (i % 16 != 0) {
      str[str_length++] = ' ';
    }

    str_length +=
        snprintf(str + str_length, sizeof(str - str_length), "%02x", buffer[i]);

    if (i % 16 == 15) {
      str[str_length++] = '\0';
      if (__print_callback) {
        __print_callback("polaris.c", __LINE__, POLARIS_LOG_LEVEL_TRACE, str);
      } else {
        P1_fprintf(stderr, "%s", str);
      }
      str_length = 0;
    }
  }

  if (str_length != 0) {
    str[str_length++] = '\0';
    if (__print_callback) {
      __print_callback("polaris.c", __LINE__, POLARIS_LOG_LEVEL_TRACE, str);
    } else {
      P1_fprintf(stderr, "%s\n", str);
    }
  }
}
#endif

/******************************************************************************/
#if !P1_NO_PRINT && POLARIS_USE_TLS
void ShowCerts(SSL* ssl) {
  if (__log_level < POLARIS_LOG_LEVEL_DEBUG) {
    return;
  }

  X509* cert = SSL_get_peer_certificate(ssl);
  if (cert != NULL) {
    char line[256];
    P1_PrintDebug("Server certificates:");
    X509_NAME_oneline(X509_get_subject_name(cert), line, sizeof(line));
    P1_PrintDebug("  Subject: %s", line);
    X509_NAME_oneline(X509_get_issuer_name(cert), line, sizeof(line));
    P1_PrintDebug("  Issuer: %s", line);
  } else {
    P1_PrintDebug("No client certificates configured.");
  }
}
#endif
