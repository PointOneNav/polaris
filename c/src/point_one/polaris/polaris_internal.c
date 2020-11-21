/**************************************************************************/ /**
 * @brief Point One Navigation Polaris message definitions and support.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/polaris_internal.h"

#include "point_one/polaris/socket.h"

/******************************************************************************/
PolarisHeader_t* Polaris_PopulateHeader(void* buffer, uint8_t message_id,
                                        uint16_t payload_length) {
  PolarisHeader_t* header = (PolarisHeader_t*)buffer;
  header->start_byte0 = POLARIS_START_BYTE_0;
  header->start_byte1 = POLARIS_START_BYTE_1;
  header->message_class = POLARIS_CLASS_INTERNAL;
  header->message_id = message_id;
  header->payload_length = htole16(payload_length);
  return header;
}

/******************************************************************************/
size_t Polaris_PopulateChecksum(void* buffer_in) {
  uint8_t* buffer = (uint8_t*)buffer_in;
  PolarisHeader_t* header = (PolarisHeader_t*)buffer;
  size_t message_length = sizeof(PolarisHeader_t) + header->payload_length;
  PolarisChecksum_t* checksum = (PolarisChecksum_t*)(buffer + message_length);
  *checksum = htole16(Polaris_CalculateChecksum(buffer, message_length));
  return message_length + sizeof(PolarisChecksum_t);
}

/******************************************************************************/
PolarisChecksum_t Polaris_CalculateChecksum(const void* buffer, size_t length) {
  // Note: The Polaris checksum is calculated on the header, _not_ including the
  // start bytes, plus the payload. Hence i starts at 2.
  uint8_t ckA = 0;
  uint8_t ckB = 0;
  for (size_t i = 2; i < length; i++) {
    ckA = ckA + ((const uint8_t*)buffer)[i];
    ckB = ckB + ckA;
  }
  return (((PolarisChecksum_t)ckB) << 8) | ckA;
}
