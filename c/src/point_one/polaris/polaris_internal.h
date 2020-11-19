/**************************************************************************/ /**
 * @brief Point One Navigation Polaris message definitions and support.
 *
 * @note
 * Polaris messages are little endian, not network (big) endian.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#pragma once

#include <stddef.h> // For size_t
#include <stdint.h>

#define POLARIS_START_BYTE_0 0xB5
#define POLARIS_START_BYTE_1 0x62

#define POLARIS_CLASS_INTERNAL 0xE0

#define POLARIS_ID_AUTH 0x01
#define POLARIS_ID_ECEF 0x03
#define POLARIS_ID_LLA 0x04
#define POLARIS_ID_BEACON 0x05

#define POLARIS_API_URL "api.pointonenav.com"

#pragma pack(push, 1)
typedef struct {
  uint8_t start_byte0;
  uint8_t start_byte1;
  uint8_t message_class;
  uint8_t message_id;
  uint16_t payload_length;
} PolarisHeader_t;

typedef struct {
  uint32_t x_cm;
  uint32_t y_cm;
  uint32_t z_cm;
} PolarisECEFMessage_t;

typedef struct {
  uint32_t latitude_dege7;
  uint32_t longitude_dege7;
  uint32_t altitude_mm;
} PolarisLLAMessage_t;
#pragma pack(pop)

typedef uint16_t PolarisChecksum_t;

#define POLARIS_MAX_HTTP_HEADER_SIZE 256
#define POLARIS_MAX_HTTP_MESSAGE_SIZE \
  (POLARIS_MAX_HTTP_HEADER_SIZE + POLARIS_AUTH_TOKEN_MAX_LENGTH)

#define POLARIS_MAX_MESSAGE_SIZE                              \
  (sizeof(PolarisHeader_t) + POLARIS_AUTH_TOKEN_MAX_LENGTH + \
   sizeof(PolarisChecksum_t))

#define POLARIS_MIN_BUFFER_SIZE POLARIS_MAX_HTTP_HEADER_SIZE

PolarisHeader_t* Polaris_PopulateHeader(void* buffer, uint8_t message_id,
                                        uint16_t payload_length);

size_t Polaris_PopulateChecksum(void* buffer);

PolarisChecksum_t Polaris_CalculateChecksum(const void* buffer, size_t length);
