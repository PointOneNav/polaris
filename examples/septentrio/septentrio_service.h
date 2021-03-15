// Copyright (C) Point One Navigation - All Rights Reserved

#pragma once

#include <functional>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <septentrio/measepoch.h>
#include <septentrio/sbfdef.h>
#include <septentrio/sbfread.h>

#include "septentrio_interface.h"

namespace point_one {
namespace gpsreceiver {

class SeptentrioService {
 public:
  SeptentrioService(std::string device_path) : device_path_(device_path) {}

  void SendRtcm(const void *buf, size_t len) {
    if (!sp_) {
      return;
    }
    sp_->Send(buf, len);
  }

  void SetPvtCallback(
      const std::function<void(const PVTGeodetic_2_2_t &data)> &callback) {
    pvt_callback_ = callback;
  }

  bool Connect(boost::asio::io_service &io_service) {
    sp_.reset(new point_one::gpsreceiver::SeptentrioSerialReceiver(
        io_service, device_path_));

    sp_->SetCallbackSBF(std::bind(&SeptentrioService::OnSBF, this,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
    return sp_->Connect();
  }

 private:
  std::string device_path_;
  std::unique_ptr<point_one::gpsreceiver::SeptentrioSerialReceiver> sp_;
  std::function<void(const PVTGeodetic_2_2_t &data)> pvt_callback_;

  void OnSBF(size_t len, const void *data) {
    const BlockHeader_t *blockHeader = (const BlockHeader_t *)data;

    // Top 3 bits represent the version of the message.
    uint16_t msg_id = blockHeader->ID & 0x1FFF;
    // Get the msg id version.
    uint8_t msg_id_version = (blockHeader->ID >> 13) & 0x07;

    if (msg_id == sbfnr_PVTGeodetic_2 && msg_id_version >= 2) {
      const PVTGeodetic_2_2_t *PVT = (const PVTGeodetic_2_2_t *)data;
      if (pvt_callback_ != nullptr) pvt_callback_(*PVT);
    }
  }
};

}  // namespace gpsreceiver
}  // namespace point_one
