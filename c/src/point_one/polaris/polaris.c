/**************************************************************************/ /**
 * @brief Point One Navigation Polaris client support.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/polaris.h"

#include <stdio.h>   // For *printf()
#include <stdlib.h>  // For malloc()
#include <string.h>  // For memmove()

#include "point_one/polaris/polaris_internal.h"

static int OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port);

static int SendPOSTRequest(PolarisContext_t* context, const char* endpoint_url,
                           int endpoint_port, const char* address,
                           const void* content, size_t content_length);

static int GetHTTPResponse(PolarisContext_t* context);

/******************************************************************************/
int Polaris_Open(PolarisContext_t* context) {
  uint8_t* buffer = malloc(POLARIS_DEFAULT_BUFFER_SIZE);
  if (buffer == NULL) {
    fprintf(stderr, "Error: Failed to allocate data buffer.\n");
    return POLARIS_NOT_ENOUGH_SPACE;
  }

  int ret =
      Polaris_OpenWithBuffer(context, buffer, POLARIS_DEFAULT_BUFFER_SIZE);
  context->buffer_managed = 1;

  if (ret != POLARIS_SUCCESS) {
    // Note: We explicitly free the buffer here and clear context->buffer,
    // rather than letting Polaris_Close() do it, just in case open failed
    // before it set context->buffer (e.g., buffer too small).
    free(buffer);
    context->buffer = NULL;

    Polaris_Close(context);
  }

  return ret;
}

/******************************************************************************/
int Polaris_OpenWithBuffer(PolarisContext_t* context, uint8_t* buffer,
                           size_t buffer_size) {
  if (buffer_size < POLARIS_MIN_BUFFER_SIZE) {
    fprintf(stderr,
            "Error: Provided buffer is too small. Must be at least %d bytes.\n",
            (int)POLARIS_MIN_BUFFER_SIZE);
    return POLARIS_ERROR;
  }

  context->socket = P1_INVALID_SOCKET;
  context->rtcm_callback = NULL;

  context->auth_token = malloc(POLARIS_AUTH_TOKEN_MAX_LENGTH);
  if (context->auth_token == NULL) {
    fprintf(stderr, "Error: Failed to allocate auth token.\n");
    return POLARIS_NOT_ENOUGH_SPACE;
  }

  context->auth_token[0] = '\0';

  context->buffer = buffer;
  context->buffer_size = buffer_size;
  context->buffer_managed = 0;

  // Note: We don't actually open the socket here - that's done after
  // authentication.
  return POLARIS_SUCCESS;
}

/******************************************************************************/
void Polaris_Close(PolarisContext_t* context) {
  if (context->socket != P1_INVALID_SOCKET) {
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
  }

  if (context->auth_token != NULL) {
    free(context->auth_token);
    context->auth_token = NULL;
  }

  if (context->buffer_managed && context->buffer != NULL) {
    free(context->buffer);
  }
  context->buffer = NULL;
}

/******************************************************************************/
int Polaris_Authenticate(PolarisContext_t* context, const char* api_key,
                         const char* unique_id) {
  return Polaris_AuthenticateWith(context, api_key, unique_id,
                                  POLARIS_ENDPOINT_URL, POLARIS_ENDPOINT_PORT);
}

/******************************************************************************/
int Polaris_AuthenticateWith(PolarisContext_t* context, const char* api_key,
                             const char* unique_id, const char* endpoint_url,
                             int endpoint_port) {
  // Send an auth request, then wait for the response containing the access
  // token.
  static const char* AUTH_REQUEST_TEMPLATE =
      "{"
      "\"grant_type\": \"authorization_code\","
      "\"token_type\": \"bearer\","
      "\"authorization_code\": \"%s\","
      "\"unique_id\": \"%s\""
      "}";

  int content_size = snprintf(context->buffer, context->buffer_size,
                              AUTH_REQUEST_TEMPLATE, api_key, unique_id);
  if (content_size < 0) {
    fprintf(stderr, "Error populating authentication request payload.\n");
    return POLARIS_NOT_ENOUGH_SPACE;
  }

  int status_code =
      SendPOSTRequest(context, POLARIS_API_URL, 80, "/api/v1/auth/token",
                      context->buffer, (size_t)content_size);
  if (status_code < 0) {
    fprintf(stderr, "Error sending authentication request.\n");
    return status_code;
  }

  // Extract the auth token from the JSON response.
  if (status_code == 200) {
    if (sscanf(context->buffer, "\"access_token\": \"%32[0-9a-f]s\"",
               context->auth_token) != 1) {
      fprintf(stderr, "Authentication token not found in response.\n");
      return POLARIS_AUTH_ERROR;
    }
  } else if (status_code == 403) {
    fprintf(stderr, "Authentication failed. Please check your API key.\n");
    return POLARIS_FORBIDDEN;
  } else {
    fprintf(stderr, "Unexpected authentication response (%d).\n", status_code);
    return POLARIS_AUTH_ERROR;
  }

  // If we got this far, we now have an auth token. Connect to the corrections
  // URL and send an auth message.
  int ret = OpenSocket(context, endpoint_url, endpoint_port);
  if (ret != POLARIS_SUCCESS) {
    fprintf(stderr, "Error connecting to corrections endpoint: tcp://%s:%d.\n",
            endpoint_url, endpoint_port);
    return ret;
  }

  size_t token_length = strlen(context->auth_token);
  PolarisHeader_t* header =
      Polaris_PopulateHeader(context->buffer, POLARIS_ID_AUTH, token_length);
  memmove(header + 1, context->auth_token, token_length);
  size_t message_size = Polaris_PopulateChecksum(context->buffer);

  if (send(context->socket, context->buffer, message_size, 0) != message_size) {
    perror("Error sending authentication token");
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_SEND_ERROR;
  }

  return POLARIS_SUCCESS;
}

/******************************************************************************/
void Polaris_SetRTCMCallback(PolarisContext_t* context,
                             PolarisCallback_t callback) {
  context->rtcm_callback = callback;
}

/******************************************************************************/
int Polaris_SendECEFPosition(PolarisContext_t* context, double x_m, double y_m,
                             double z_m) {
  if (context->socket == P1_INVALID_SOCKET) {
    fprintf(stderr, "Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->buffer, POLARIS_ID_ECEF, sizeof(PolarisECEFMessage_t));
  PolarisECEFMessage_t* payload = (PolarisECEFMessage_t*)(header + 1);
  payload->x_cm = htole32((uint32_t)(x_m * 1e2));
  payload->y_cm = htole32((uint32_t)(y_m * 1e2));
  payload->z_cm = htole32((uint32_t)(z_m * 1e2));
  size_t message_size = Polaris_PopulateChecksum(context->buffer);

  if (send(context->socket, context->buffer, message_size, 0) != message_size) {
    perror("Error sending ECEF position");
    return POLARIS_SEND_ERROR;
  }
  else {
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_SendLLAPosition(PolarisContext_t* context, double latitude_deg,
                            double longitude_deg, double altitude_m) {
  if (context->socket == P1_INVALID_SOCKET) {
    fprintf(stderr, "Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  PolarisHeader_t* header = Polaris_PopulateHeader(
      context->buffer, POLARIS_ID_LLA, sizeof(PolarisLLAMessage_t));
  PolarisLLAMessage_t* payload = (PolarisLLAMessage_t*)(header + 1);
  payload->latitude_dege7 = htole32((uint32_t)(latitude_deg * 1e7));
  payload->longitude_dege7 = htole32((uint32_t)(longitude_deg * 1e7));
  payload->altitude_mm = htole32((uint32_t)(altitude_m * 1e3));
  size_t message_size = Polaris_PopulateChecksum(context->buffer);

  if (send(context->socket, context->buffer, message_size, 0) != message_size) {
    perror("Error sending LLA position");
    return POLARIS_SEND_ERROR;
  }
  else {
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
int Polaris_RequestBeacon(PolarisContext_t* context, const char* beacon_id) {
  if (context->socket == P1_INVALID_SOCKET) {
    fprintf(stderr, "Error: Polaris connection not currently open.\n");
    return POLARIS_SOCKET_ERROR;
  }

  size_t id_length = strlen(beacon_id);
  PolarisHeader_t* header =
      Polaris_PopulateHeader(context->buffer, POLARIS_ID_BEACON, id_length);
  memmove(header + 1, beacon_id, id_length);
  size_t message_size = Polaris_PopulateChecksum(context->buffer);

  if (send(context->socket, context->buffer, message_size, 0) != message_size) {
    perror("Error sending beacon request");
    return POLARIS_SEND_ERROR;
  }
  else {
    return POLARIS_SUCCESS;
  }
}

/******************************************************************************/
void Polaris_Run(PolarisContext_t* context) {
  while (1) {
    // Read some data.
    P1_RecvSize_t bytes_read =
        recv(context->socket, context->buffer, context->buffer_size, 0);
    if (bytes_read < 0) {
      break;
    } else if (bytes_read == 0) {
      continue;
    }

    // We don't interpret the incoming RTCM data, so there's no need to buffer
    // it up to a complete RTCM frame. We'll just forward what we got along.
    if (context->rtcm_callback) {
      context->rtcm_callback(context->buffer, bytes_read);
    }
  }
}

/******************************************************************************/
static int OpenSocket(PolarisContext_t* context, const char* endpoint_url,
                      int endpoint_port) {
  // Is the connection already open?
  if (context->socket != P1_INVALID_SOCKET) {
    fprintf(stderr, "Error socket already open.\n");
    return POLARIS_ERROR;
  }

  // Open a socket.
  context->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (context->socket < 0) {
    fprintf(stderr, "Error opening socket.\n");
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_SOCKET_ERROR;
  }

  // Set send/receive timeouts.
  P1_TimeValue_t timeout;
  P1_SetTime(2000, &timeout);
  setsockopt(context->socket, 0, SO_RCVTIMEO, &timeout, sizeof(timeout));
  setsockopt(context->socket, 0, SO_SNDTIMEO, &timeout, sizeof(timeout));

  // Lookup the IP of the API endpoint used for auth requests.
  P1_SocketAddrV4_t address;
  if (P1_SetAddress(endpoint_url, endpoint_port, &address) < 0) {
    fprintf(stderr, "Error locating address '%s'.\n", endpoint_url);
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_SOCKET_ERROR;
  }

  // Connect to the API server.
  if (connect(context->socket, (P1_SocketAddr_t*)&address, sizeof(address)) <
      0) {
    perror("Error connecting to endpoint");
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
  if (context->buffer_size < header_size + content_length + 1) {
    fprintf(stderr, "Error populating POST request: buffer too small.\n");
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_NOT_ENOUGH_SPACE;
  }

  uint8_t first_content_byte =
      (content_length == 0 ? '\0' : ((const uint8_t*)content)[0]);
  memmove(context->buffer + header_size, content, content_length);
  context->buffer[header_size + content_length] = '\0';

  // Now populate the header.
  header_size = snprintf(context->buffer, header_size + 1, HEADER_TEMPLATE,
                         address, endpoint_url, port_str, content_length_str);
  if (header_size < 0) {
    // This shouldn't happen.
    fprintf(stderr, "Error populating POST request.\n");
    close(context->socket);
    context->socket = P1_INVALID_SOCKET;
    return POLARIS_ERROR;
  }

  context->buffer[header_size] = first_content_byte;

  size_t message_size = header_size + content_length;

  // Send the request.
  int ret = OpenSocket(context, endpoint_url, endpoint_port);
  if (ret != POLARIS_SUCCESS) {
    return ret;
  }

  if (send(context->socket, context->buffer, message_size, 0) != message_size) {
    perror("Error sending POST request");
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
  int total_bytes = 0;
  int bytes_read;
  while ((bytes_read = recv(context->socket, context->buffer + total_bytes,
                            context->buffer_size - total_bytes - 1, 0)) > 0) {
    total_bytes += bytes_read;
    if (total_bytes == context->buffer_size - 1) {
      break;
    }
  }

  close(context->socket);
  context->socket = P1_INVALID_SOCKET;

  // Append a null terminator to the response.
  context->buffer[total_bytes++] = '\0';

  // Extract the status code.
  int status_code;
  if (sscanf(context->buffer, "HTTP/1.1 %d", &status_code) != 1) {
    fprintf(stderr, "Invalid HTTP response:\n\n%s", context->buffer);
    return POLARIS_SEND_ERROR;
  }

  // Find the content, then move the response content to the front of the
  // buffer. We don't care about the HTTP headers.
  int content_length;
  char* content_start = strstr(context->buffer, "\r\n\r\n");
  if (content_start != NULL) {
    content_start += 4;
    size_t content_length =
        ((size_t)total_bytes) - (content_start - (char*)context->buffer);
    memmove(context->buffer, content_start, content_length);
  } else {
    // No content in response.
    context->buffer[0] = '\0';
  }

  return status_code;
}
