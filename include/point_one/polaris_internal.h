// Copyright (C) Point One Navigation - All Rights Reserved
// Written by Jason Moran <jason@pointonenav.com>, March 2019

// Encapsulation of binary protocol for connecting with the Point One Navigation
// Polaris Server.

#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace point_one {
namespace polaris {
namespace internal {

// Protocol bytes.
static constexpr uint8_t START_BYTE_0 = 0xb5;
static constexpr uint8_t START_BYTE_1 = 0x62;
static constexpr uint8_t SYSTEM_TYPE = 0xe0;
static constexpr uint8_t SYSTEM_AUTH = 0x01;
static constexpr uint8_t SYSTEM_ECEF = 0x03;
static constexpr uint8_t SYSTEM_LLA = 0x04;
static constexpr uint8_t SYSTEM_BEACON = 0x05;

#pragma pack(1)
struct PolarisHeader {
  PolarisHeader(uint8_t mtype, uint8_t id, uint16_t len)
      : message_type(mtype), id(id), length(len) {}
  uint8_t start_byte0 = START_BYTE_0;
  uint8_t start_byte1 = START_BYTE_1;
  uint8_t message_type;
  uint8_t id;
  uint16_t length;
};

constexpr size_t POLARIS_HEADER_SIZE = sizeof(PolarisHeader);
#pragma pack()

typedef uint16_t Checksum;
constexpr size_t POLARIS_CHECKSUM_SIZE = sizeof(Checksum);
static Checksum GetChecksum(const uint8_t bytes[], size_t length) {
  uint8_t ckA = 0;
  uint8_t ckB = 0;

  for (size_t i = 2; i < length; i++) {
    ckA = ckA + bytes[i];
    ckB = ckB + ckA;
  }
  return (static_cast<uint16_t>(ckB) << 8) | ckA;
}

// Gets a polaris message size including header and CRC.
static size_t GetMessageSize(size_t message_len) {
  return POLARIS_HEADER_SIZE + message_len + POLARIS_CHECKSUM_SIZE;
}

// Serializes Polaris message with header and CRC.
static void SerializeMessage(const PolarisHeader &header,
                             const uint8_t *message, uint8_t *buf) {
  std::memcpy(buf, &header, POLARIS_HEADER_SIZE);
  std::memcpy(buf + POLARIS_HEADER_SIZE, message, header.length);
  Checksum checksum = GetChecksum(buf, POLARIS_HEADER_SIZE + header.length);
  std::memcpy(buf + POLARIS_HEADER_SIZE + header.length, &checksum,
              POLARIS_CHECKSUM_SIZE);
}

}  // namespace internal
}  // namespace polaris.
}  // namespace point_one.
