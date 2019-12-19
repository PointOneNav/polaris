// Copyright (C) Point One Navigation - All Rights Reserved
// Written by Jason Moran <jason@pointonenav.com>, March 2019

// Encapsulation of binary protocol for connecting with the Point One Navigation
// Polaris Server.

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include "polaris_internal.h"

namespace point_one {
namespace polaris {

namespace {
// Converts SI meters to centimeters.
static constexpr double METERS_TO_CENTIMETERS = 100.0;

// Converts SI meters to millimeters.
static constexpr double METERS_TO_MILLIMETERS = 1000.0;
}  // namespace

// Polaris Service connection defaults.
static const std::string DEFAULT_POLARIS_URL = "polaris.pointonenav.com";
static constexpr int DEFAULT_POLARIS_PORT = 8088;

// Point One api for requesting access token.
static const std::string DEFAULT_POINTONE_API_URL = "api.pointonenav.com";

// How frequently to send position to polaris backend.
static constexpr int DEFAULT_POSITION_SEND_INTERVAL_MSECS = 3000;

struct PolarisConnectionSettings {
  PolarisConnectionSettings()
      : host(DEFAULT_POLARIS_URL),
        port(DEFAULT_POLARIS_PORT),
        api_host(DEFAULT_POINTONE_API_URL),
        interval_ms(DEFAULT_POSITION_SEND_INTERVAL_MSECS) {}
  std::string host;
  int port;
  std::string api_host;
  int interval_ms;
};

static const PolarisConnectionSettings DEFAULT_CONNECTION_SETTINGS;

struct PolarisAuthToken {
  std::string access_token;
  double issued_at;
  double expires_in;
};

#pragma pack(1)

// ECEF position in degrees.

struct PositionEcef {
  PositionEcef() {}
  PositionEcef(const double pos_meters[3]) {
    std::copy(pos_meters, pos_meters + 3, pos);
  }
  PositionEcef(double x_meters, double y_meters, double z_meters) {
    pos[0] = x_meters;
    pos[1] = y_meters;
    pos[2] = z_meters;
  }
  double pos[3];
};

struct PolarisPositionEcef {
  PolarisPositionEcef(const PositionEcef &ecef) {
    x_cm =
        static_cast<int32_t>(std::round(ecef.pos[0] * METERS_TO_CENTIMETERS));
    y_cm =
        static_cast<int32_t>(std::round(ecef.pos[1] * METERS_TO_CENTIMETERS));
    z_cm =
        static_cast<int32_t>(std::round(ecef.pos[2] * METERS_TO_CENTIMETERS));
  }

  int32_t x_cm;
  int32_t y_cm;
  int32_t z_cm;
};

// Latitide, Longitude (decimal degrees x 10^7).
// Altitude (cm)
struct PositionLla {
  PositionLla() {}
  PositionLla(const double pos_lla[3]) { std::copy(pos_lla, pos_lla + 3, pos); }
  PositionLla(double lat_deg, double lon_deg, double alt_m) {
    pos[0] = lat_deg;
    pos[1] = lon_deg;
    pos[2] = alt_m;
  }
  double pos[3];
};

struct PolarisPositionLla {
  PolarisPositionLla(const PositionLla &lla) {
    lat_deg_ = static_cast<int32_t>(std::round(lla.pos[0] * 1e7));
    lon_deg_ = static_cast<int32_t>(std::round(lla.pos[1] * 1e7));
    alt_mm_ =
        static_cast<int32_t>(std::round(lla.pos[2] * METERS_TO_MILLIMETERS));
  }
  int32_t lat_deg_;
  int32_t lon_deg_;
  int32_t alt_mm_;
};
#pragma pack()

// Request abstract class/
class PolarisRequest {
 public:
  virtual ~PolarisRequest() {}

  virtual size_t GetSize() = 0;

  virtual size_t Serialize(uint8_t *) = 0;
};

// Class for storing a request to authenticate with Polaris Service.
// It can be serialized to a buffer with Serialize().
// Auth_token can be an device api key or temporary auth token.
// Contact sales@pointonenav.com if you do not have a device api key.
class AuthRequest : public PolarisRequest {
 public:
  explicit AuthRequest(const std::string &auth_token)
      : auth_token_(auth_token) {}

  ~AuthRequest() override {}

  size_t GetSize() override {
    return internal::GetMessageSize(auth_token_.size());
  }

  size_t Serialize(uint8_t *buf) override {
    internal::PolarisHeader header(internal::SYSTEM_TYPE, internal::SYSTEM_AUTH,
                                   auth_token_.size());
    internal::SerializeMessage(
        header, reinterpret_cast<const uint8_t *>(auth_token_.c_str()), buf);
    return GetSize();
  }

 private:
  std::string auth_token_;
};

// A request that gets the list of available beacons.
class BeaconRequest : public PolarisRequest {
 public:
  explicit BeaconRequest(const std::string &beacon_id)
      : beacon_id_(beacon_id) {}

  ~BeaconRequest() override {}

  size_t GetSize() override {
    return internal::GetMessageSize(beacon_id_.size());
  }

  size_t Serialize(uint8_t *buf) override {
    internal::PolarisHeader header(internal::SYSTEM_TYPE,
                                   internal::SYSTEM_BEACON, beacon_id_.size());
    internal::SerializeMessage(
        header, reinterpret_cast<const uint8_t *>(beacon_id_.c_str()), buf);
    return GetSize();
  }

 private:
  std::string beacon_id_;
};

// Provides Polaris Service with ECEF Position allowing the service to
// generated and respond with a correction stream for the provided position.
template <typename T, uint8_t ID>
class PositionRequest : public PolarisRequest {
 public:
  explicit PositionRequest(const T &position) : pos_(position) {}

  ~PositionRequest() override {}

  size_t GetSize() override { return internal::GetMessageSize(sizeof(T)); }

  size_t Serialize(uint8_t *buf) override {
    internal::PolarisHeader header(internal::SYSTEM_TYPE, ID, sizeof(T));
    internal::SerializeMessage(header, reinterpret_cast<uint8_t *>(&pos_), buf);
    return GetSize();
  }

 private:
  T pos_;
};
// Request for sending ECEF postion.
typedef PositionRequest<PolarisPositionEcef, internal::SYSTEM_ECEF>
    PositionEcefRequest;

// Request for sending LLA position.
typedef PositionRequest<PolarisPositionLla, internal::SYSTEM_LLA>
    PositionLlaRequest;

}  // namespace polaris.
}  // namespace point_one.
