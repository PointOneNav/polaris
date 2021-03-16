/**************************************************************************/ /**
 * @brief Point One Navigation Polaris client support.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/polaris.h"

#include <stdio.h>   // For sscanf() and snprintf()
#include <stdlib.h>  // For malloc()
#include <string.h>  // For memmove()

#include "point_one/polaris/polaris_internal.h"
#include "point_one/polaris/portability.h"

#define MAKE_STR(x) #x
#define STR(x) MAKE_STR(x)

#define Print(x, ...) \
  P1_fprintf(stderr, "polaris.c:" STR(__LINE__) "] " x, ##__VA_ARGS__)
#define PrintError(x, ...) \
  P1_perror("polaris.c:" STR(__LINE__) "] " x, ##__VA_ARGS__)

#if defined(POLARIS_DEBUG) || defined(POLARIS_TRACE)
# define DebugPrint(x, ...) Print(x, ##__VA_ARGS__)
#else
# define DebugPrint(x, ...) do {} while(0)
#endif

#if defined(POLARIS_TRACE)
void PrintData(const uint8_t* buffer, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    if (i % 16 != 0) {
      DebugPrint(" ");
    }

    DebugPrint("%02x", buffer[i]);

    if (i % 16 == 15) {
      DebugPrint("\n");
    }
  }
  DebugPrint("\n");
}
#else
# define PrintData(buffer, length) do {} while(0)
#endif

static int OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port);

static int SendPOSTRequest(PolarisContext_t* context, const char* endpoint_url,
                           int endpoint_port, const char* address,
                           const void* content, size_t content_length);

static int GetHTTPResponse(PolarisContext_t* context);

/******************************************************************************/
int Polaris_Init(PolarisContext_t* context) {
  if (POLARIS_RECV_BUFFER_SIZE < POLARIS_MAX_HTTP_MESSAGE_SIZE) {
    Print(
        "Warning: Receive buffer smaller than expected authentication "
        "response.\n");
  }

  if (POLARIS_SEND_BUFFER_SIZE < POLARIS_MAX_MESSAGE_SIZE) {
    Print("Warning: Send buffer smaller than max expected outbound packet.\n");
  }

  context->socket = P1_INVALID_SOCKET;
  context->auth_token[0] = '\0';
  context->authenticated = 0;
  context->disconnected = 0;
  context->rtcm_callback = NULL;
  context->rtcm_callback_info = NULL;
  return POLARIS_SUCCESS;
}

/******************************************************************************/
int Polaris_Authenticate(PolarisContext_t* context, const char* api_key,
                         const char* unique_id) {
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

  int content_size =
      snprintf((char*)context->recv_buffer, POLARIS_RECV_BUFFER_SIZE,
               AUTH_REQUEST_TEMPLATE, api_key, unique_id);
  if (content_size < 0) {
    Print("Error populating authentication request payload.\n");
    return POLARIS_NOT_ENOUGH_SPACE;
  }

  DebugPrint("Sending auth request. [api_key=%s, unique_id=%s]\n", api_key,
             unique_id);
  int status_code =
      SendPOSTRequest(context, POLARIS_API_URL, 80, "/api/v1/auth/token",
                      context->recv_buffer, (size_t)content_size);
  if (status_code < 0) {
    Print("Error sending authentication request.\n");
    return status_code;
  }

  // Extract the auth token from the JSON response.
  if (status_code == 200) {
    const char* token_start =
        strstr((char*)context->recv_buffer, "\"access_token\":\"");
    if (token_start == NULL) {
      Print("Authentication token not found in response.\n");
      return POLARIS_AUTH_ERROR;
    } else {
      token_start += 16;
      if (sscanf(token_start, "%" STR(POLARIS_MAX_TOKEN_SIZE) "[^\"]s",
                 context->auth_token) != 1) {
        Print("Authentication token not found in response.\n");
        return POLARIS_AUTH_ERROR;
      } else {
        DebugPrint("Received access token: %s\n", context->auth_token);
        return POLARIS_SUCCESS;
      }
    }
  } else if (status_code == 403) {
    Print("Authentication failed. Please check your API key.\n");
    return POLARIS_FORBIDDEN;
  } else {
    Print("Unexpected authentication response (%d).\n", status_code);
    return POLARIS_AUTH_ERROR;
  }
}

/******************************************************************************/
int Polaris_SetAuthToken(PolarisContext_t* context, const char* auth_token) {
  size_t length = strlen(auth_token);
  if (length > POLARIS_MAX_TOKEN_SIZE) {
    Print("User-provided auth token is too long.\n");
    return POLARIS_NOT_ENOUGH_SPACE;
  } else {
    memcpy(context->auth_token, auth_token, length + 1);
    DebugPrint("Using user-specified access token: %s\n", context->auth_token);
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_Connect(PolarisContext_t* context) {
  return Polaris_ConnectTo(context, POLARIS_ENDPOINT_URL,
                           POLARIS_ENDPOINT_PORT);
}

/******************************************************************************/
int Polaris_ConnectTo(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port) {
  if (context->auth_token[0] == '\0') {
    Print("Error: Auth token not specified.\n");
    return POLARIS_AUTH_ERROR;
  }

  // Connect to the corrections endpoint.
  context->disconnected = 0;
  int ret = OpenSocket(context, endpoint_url, endpoint_port);
  if (ret != POLARIS_SUCCESS) {
    Print("Error connecting to corrections endpoint: tcp://%s:%d.\n",
          endpoint_url, endpoint_port);
    return ret;
  }

  // Send the auth token.
  //
  // Note: We use the receive buffer here to send the auth message since the
  // auth token is very large. We haven't authenticated yet, so no data will be
  // coming in and it should be fine to use the receive buffer.
  size_t token_length = strlen(context->auth_token);
  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->recv_buffer, POLARIS_ID_AUTH, token_length);
  memmove(header + 1, context->auth_token, token_length);
  size_t message_size = Polaris_PopulateChecksum(context->recv_buffer);

  DebugPrint("Sending access token message. [size=%u B]\n",
              (unsigned)message_size);
  ret = send(context->socket, context->recv_buffer, message_size, 0);
  if (ret != message_size) {
    PrintError("Error sending authentication token", ret);
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_SEND_ERROR;
  }

  return POLARIS_SUCCESS;
}

/******************************************************************************/
void Polaris_Disconnect(PolarisContext_t* context) {
  if (context->socket != P1_INVALID_SOCKET) {
    DebugPrint("Closing Polaris connection.\n");
    context->disconnected = 1;
    shutdown(context->socket, SHUT_RDWR);
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
  }
}

/******************************************************************************/
void Polaris_SetRTCMCallback(PolarisContext_t* context,
                             PolarisCallback_t callback,
                             void* callback_info) {
  context->rtcm_callback = callback;
  context->rtcm_callback_info = callback_info;
}

/******************************************************************************/
int Polaris_SendECEFPosition(PolarisContext_t* context, double x_m, double y_m,
                             double z_m) {
  if (context->socket == P1_INVALID_SOCKET) {
    Print("Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->send_buffer, POLARIS_ID_ECEF, sizeof(PolarisECEFMessage_t));
  PolarisECEFMessage_t* payload = (PolarisECEFMessage_t*)(header + 1);
  payload->x_cm = htole32((int32_t)(x_m * 1e2));
  payload->y_cm = htole32((int32_t)(y_m * 1e2));
  payload->z_cm = htole32((int32_t)(z_m * 1e2));
  size_t message_size = Polaris_PopulateChecksum(context->send_buffer);

#ifdef P1_FREERTOS
  // Floating point printf() not available in FreeRTOS.
  DebugPrint("Sending ECEF position. [size=%u B, position=[%d, %d, %d] cm]\n",
              (unsigned)message_size, le32toh(payload->x_cm),
              le32toh(payload->y_cm), le32toh(payload->z_cm));
#else
  DebugPrint(
      "Sending ECEF position. [size=%u B, position=[%.2f, %.2f, %.2f]]\n",
      (unsigned)message_size, x_m, y_m, z_m);
#endif
  PrintData(context->send_buffer, message_size);

  int ret = send(context->socket, context->send_buffer, message_size, 0);
  if (ret != message_size) {
    PrintError("Error sending ECEF position", ret);
    return POLARIS_SEND_ERROR;
  } else {
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_SendLLAPosition(PolarisContext_t* context, double latitude_deg,
                            double longitude_deg, double altitude_m) {
  if (context->socket == P1_INVALID_SOCKET) {
    Print("Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->send_buffer, POLARIS_ID_LLA, sizeof(PolarisLLAMessage_t));
  PolarisLLAMessage_t* payload = (PolarisLLAMessage_t*)(header + 1);
  payload->latitude_dege7 = htole32((int32_t)(latitude_deg * 1e7));
  payload->longitude_dege7 = htole32((int32_t)(longitude_deg * 1e7));
  payload->altitude_mm = htole32((int32_t)(altitude_m * 1e3));
  size_t message_size = Polaris_PopulateChecksum(context->send_buffer);

#ifdef P1_FREERTOS
  // Floating point printf() not available in FreeRTOS.
  DebugPrint(
      "Sending LLA position. [size=%u B, position=[%d.0e-7, %d.0e-7, %d]]\n",
      (unsigned)message_size, le32toh(payload->latitude_dege7),
      le32toh(payload->longitude_dege7), le32toh(payload->altitude_mm));
#else
  DebugPrint(
      "Sending LLA position. [size=%u B, position=[%.6f, %.6f, %.2f]]\n",
      (unsigned)message_size, latitude_deg, longitude_deg, altitude_m);
#endif
  PrintData(context->send_buffer, message_size);

  int ret = send(context->socket, context->send_buffer, message_size, 0);
  if (ret != message_size) {
    PrintError("Error sending LLA position", ret);
    return POLARIS_SEND_ERROR;
  } else {
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_RequestBeacon(PolarisContext_t* context, const char* beacon_id) {
  if (context->socket == P1_INVALID_SOCKET) {
    Print("Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  size_t id_length = strlen(beacon_id);
  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->send_buffer, POLARIS_ID_BEACON, id_length);
  memmove(header + 1, beacon_id, id_length);
  size_t message_size = Polaris_PopulateChecksum(context->send_buffer);

  DebugPrint("Sending beacon request. [size=%u B, beacon='%s']\n",
              (unsigned)message_size, beacon_id);
  PrintData(context->send_buffer, message_size);

  int ret = send(context->socket, context->send_buffer, message_size, 0);
  if (ret != message_size) {
    PrintError("Error sending beacon request", ret);
    return POLARIS_SEND_ERROR;
  } else {
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_Work(PolarisContext_t* context) {
  if (context->disconnected) {
    return 0;
  } else if (context->socket == P1_INVALID_SOCKET) {
    Print("Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  DebugPrint("Listening for data block.\n");
  P1_RecvSize_t bytes_read =
      recv(context->socket, context->recv_buffer, POLARIS_RECV_BUFFER_SIZE, 0);
  DebugPrint("recv returned with %d\n", bytes_read);
  if (bytes_read < 0) {
    DebugPrint("Connection terminated. [ret=%d, disconnected=%d]\n",
                (int)bytes_read, context->disconnected);
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    if (context->disconnected) {
      return 0;
    } else {
      return POLARIS_CONNECTION_CLOSED;
    }
  } else if (bytes_read == 0) {
    // If recv() times out before we've gotten anything, the socket was probably
    // closed on the other end due to an auth failure.
    if (!context->authenticated) {
      Print(
          "Warning: Polaris connection closed and no data received. Is your "
          "authentication token valid? Did you send a position?\n");
      close(context->socket);
      context->socket = P1_INVALID_SOCKET;
      return POLARIS_FORBIDDEN;
    }
    // Otherwise, there may just not be new data available (e.g., user hasn't
    // sent a position yet, network connection temporarily broken, etc.).
    else {
      DebugPrint("Received 0 bytes/socket timed out.\n");
      return 0;
    }
  } else {
    DebugPrint("Received %u bytes.\n", (unsigned)bytes_read);
    context->authenticated = 1;

    // We don't interpret the incoming RTCM data, so there's no need to buffer
    // it up to a complete RTCM frame. We'll just forward what we got along.
    if (context->rtcm_callback) {
      context->rtcm_callback(context->rtcm_callback_info, context,
                             context->recv_buffer, bytes_read);
    }

    return (int)bytes_read;
  }
}

/******************************************************************************/
int Polaris_Run(PolarisContext_t* context, int connection_timeout_ms) {
  if (context->socket == P1_INVALID_SOCKET) {
    Print("Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  DebugPrint("Listening for data.\n");

  P1_TimeValue_t last_read_time;
  P1_GetCurrentTime(&last_read_time);

  size_t total_bytes = 0;
  int ret = POLARIS_ERROR;
  while (!context->disconnected) {
    // Read the next data block.
    ret = Polaris_Work(context);

    if (ret < 0) {
      // Connection closed remotely or another error occurred.
      break;
    } else if (ret == 0) {
      // Did the user call disconnect?
      if (context->disconnected) {
        ret = POLARIS_SUCCESS;
        break;
      }
      // Read timed out - see if we've hit the connection timeout. Otherwise,
      // try again.
      else {
        P1_TimeValue_t current_time;
        P1_GetCurrentTime(&current_time);
        int elapsed_ms = P1_GetElapsedMS(&last_read_time, &current_time);
        if (elapsed_ms >= connection_timeout_ms) {
          Print("Warning: Connection timed out after %d ms.\n", elapsed_ms);
          close(context->socket);
          context->socket = P1_INVALID_SOCKET;
          ret = POLARIS_TIMED_OUT;
          break;
        }
      }
    } else {
      // Data received and dispatched to the callback.
      total_bytes += (size_t)ret;
      P1_GetCurrentTime(&last_read_time);
    }
  }

  DebugPrint("Received %u total bytes.\n", (unsigned)total_bytes);
  return context->disconnected ? POLARIS_SUCCESS : ret;
}

/******************************************************************************/
static int OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port) {
  // Is the connection already open?
  if (context->socket != P1_INVALID_SOCKET) {
    Print("Error socket already open.\n");
    return POLARIS_ERROR;
  }

  // Open a socket.
  context->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (context->socket < 0) {
    Print("Error opening socket.\n");
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_SOCKET_ERROR;
  }

  // Set send/receive timeouts.
  P1_TimeValue_t timeout;
  P1_SetTimeMS(POLARIS_RECV_TIMEOUT_MS, &timeout);
  setsockopt(context->socket, 0, SO_RCVTIMEO, &timeout, sizeof(timeout));
  P1_SetTimeMS(POLARIS_SEND_TIMEOUT_MS, &timeout);
  setsockopt(context->socket, 0, SO_SNDTIMEO, &timeout, sizeof(timeout));

  // Lookup the IP of the API endpoint used for auth requests.
  P1_SocketAddrV4_t address;
  if (P1_SetAddress(endpoint_url, endpoint_port, &address) < 0) {
    Print("Error locating address '%s'.\n", endpoint_url);
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_SOCKET_ERROR;
  }

  // Connect to the API server.
  DebugPrint("Connecting to 'tcp://%s:%d'.\n", endpoint_url, endpoint_port);
  int ret =
      connect(context->socket, (P1_SocketAddr_t*)&address, sizeof(address));
  if (ret < 0) {
    PrintError("Error connecting to endpoint", ret);
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_SOCKET_ERROR;
  }

  return POLARIS_SUCCESS;
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
    Print("Error populating POST request: buffer too small.\n");
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
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
    Print("Error populating POST request.\n");
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_ERROR;
  }

  context->recv_buffer[header_size] = first_content_byte;

  size_t message_size = header_size + content_length;

  // Send the request.
  int ret = OpenSocket(context, endpoint_url, endpoint_port);
  if (ret != POLARIS_SUCCESS) {
    return ret;
  }

  DebugPrint("Sending POST request. [size=%u B]\n", (unsigned)message_size);
  ret = send(context->socket, context->recv_buffer, message_size, 0);
  if (ret != message_size) {
    PrintError("Error sending POST request", ret);
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
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
  while ((bytes_read = recv(context->socket, context->recv_buffer + total_bytes,
                            POLARIS_RECV_BUFFER_SIZE - total_bytes - 1, 0)) >
         0) {
    total_bytes += (size_t)bytes_read;
    if (total_bytes == POLARIS_RECV_BUFFER_SIZE - 1) {
      break;
    }
  }

  close(context->socket);
  context->socket = P1_INVALID_SOCKET;

  DebugPrint("Received HTTP request. [size=%u B]\n", (unsigned)total_bytes);

  // Append a null terminator to the response.
  context->recv_buffer[total_bytes++] = '\0';

  // Extract the status code.
  int status_code;
  if (sscanf((char*)context->recv_buffer, "HTTP/1.1 %d", &status_code) != 1) {
    Print("Invalid HTTP response:\n\n%s", context->recv_buffer);
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
    DebugPrint("Response content:\n%s\n", context->recv_buffer);
  } else {
    // No content in response.
    DebugPrint("No content in response.\n");
    context->recv_buffer[0] = '\0';
  }

  return status_code;
}
