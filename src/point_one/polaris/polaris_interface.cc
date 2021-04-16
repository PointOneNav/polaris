/**************************************************************************/ /**
 * @brief Polaris client C++ wrapper class.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include "point_one/polaris/polaris_interface.h"

using namespace point_one::polaris;

/******************************************************************************/
PolarisInterface::PolarisInterface() {
  Polaris_Init(&context_);
  Polaris_SetRTCMCallback(&context_, &PolarisInterface::HandleRTCMData, this);
}

/******************************************************************************/
PolarisInterface::~PolarisInterface() {
  Disconnect();
  Polaris_Free(&context_);
}

/******************************************************************************/
int PolarisInterface::Authenticate(const std::string& api_key,
                                   const std::string& unique_id) {
  return Polaris_Authenticate(&context_, api_key.c_str(), unique_id.c_str());
}

/******************************************************************************/
int PolarisInterface::SetAuthToken(const std::string& auth_token) {
  return Polaris_SetAuthToken(&context_, auth_token.c_str());
}

/******************************************************************************/
int PolarisInterface::Connect() {
  return Polaris_Connect(&context_);
}

/******************************************************************************/
int PolarisInterface::ConnectTo(const std::string& endpoint_url,
                                int endpoint_port) {
  return Polaris_ConnectTo(&context_, endpoint_url.c_str(), endpoint_port);
}

/******************************************************************************/
void PolarisInterface::Disconnect() {
  Polaris_Disconnect(&context_);
}

/******************************************************************************/
void PolarisInterface::SetRTCMCallback(
    std::function<void(const uint8_t* buffer, size_t size_bytes)> callback) {
  callback_ = callback;
}

/******************************************************************************/
int PolarisInterface::SendECEFPosition(double x_m, double y_m, double z_m) {
  return Polaris_SendECEFPosition(&context_, x_m, y_m, z_m);
}

/******************************************************************************/
int PolarisInterface::SendLLAPosition(double latitude_deg, double longitude_deg,
                                      double altitude_m) {
  return Polaris_SendLLAPosition(&context_, latitude_deg, longitude_deg,
                                 altitude_m);
}

/******************************************************************************/
int PolarisInterface::RequestBeacon(const std::string& beacon_id) {
  return Polaris_RequestBeacon(&context_, beacon_id.c_str());
}

/******************************************************************************/
int PolarisInterface::Work() {
  return Polaris_Work(&context_);
}

/******************************************************************************/
int PolarisInterface::Run(int connection_timeout_ms) {
  return Polaris_Run(&context_, connection_timeout_ms);
}

/******************************************************************************/
const uint8_t* PolarisInterface::GetRecvBuffer() const {
  return context_.recv_buffer;
}

/******************************************************************************/
void PolarisInterface::HandleRTCMData(void* ptr, PolarisContext_t* context,
                                      const uint8_t* buffer,
                                      size_t size_bytes) {
  auto interface = static_cast<PolarisInterface*>(ptr);
  if (interface->callback_) {
    interface->callback_(buffer, size_bytes);
  }
}
