// Copyright (C) Point One Navigation - All Rights Reserved

#pragma once


#include <stdint.h>
#include <functional>

#include "glog/logging.h"


namespace point_one {
namespace gpsreceiver {


#pragma pack(1)
struct SbfHeader
{
  uint8_t sync1;
  uint8_t sync2;
  uint16_t crc;
  uint16_t id;
  uint16_t length;
};
#pragma pack()

class SBFFramer {
 public:

  SBFFramer() {
    this->parser_state = STATE_SYNC1;

    this->cur_packet[MAX_SBF_MSG_LEN] = 0x00;
    this->cur_packet_index = 0;
    this->cur_msg_len = 0;
    this->bad_sync_counter = 0;

    this->callbackSBFFrame = NULL;
    this->callbackResponse = NULL;
  }

  void SetCallbackSBFFrame(std::function<void(uint16_t, uint8_t *)> callback) {
    this->callbackSBFFrame = callback;
    LOG(INFO)<<"Set Septentrio Parser SBF Callback";
  }

  void SetCallbackResponse(std::function<void(char *)> callback) {
    this->callbackResponse = callback;
    LOG(INFO)<<"Set Septentrio Parser Response Callback";
  }

  void OnByte(uint8_t b) {
    //	debugPrintf("byte: %c\n", b);
    //	debugPrintf("Parser State: %d\n", parser_state);
    switch (parser_state) {
      case STATE_SYNC1: {
        cur_packet_index = 0;
        if (b == '$') {
          cur_packet[cur_packet_index++] = b;
          parser_state = STATE_SYNC2;
        } else {
          bad_sync_counter++;
        }
        break;
      }
      case STATE_SYNC2: {
        cur_packet[cur_packet_index++] = b;
        if (b == '@') {
          parser_state = STATE_HEADER;
        } else if (b == 'R') {
          parser_state = STATE_CMD_RESPONSE;
        } else {
          parser_state = STATE_SYNC1; //reset state machine
        }
        break;
      }
      case STATE_HEADER: {
        //Get the remaining bytes that complete the header
        cur_packet[cur_packet_index++] = b;
        if (cur_packet_index == sizeof(SbfHeader)) {
          SbfHeader *header = (SbfHeader *) cur_packet;
          this->cur_msg_len = header->length; //lenght is the total packet length, including header
          if (this->cur_msg_len >= MAX_SBF_MSG_LEN) {
            LOG(WARNING) <<"Too big a message?";
            parser_state = STATE_SYNC1;
            break;
          }

          if (header->length <= sizeof(SbfHeader)) {
            LOG(WARNING) <<"Something Broke, start over!";
            parser_state = STATE_SYNC1;
          } else {
            parser_state = STATE_DATA;
          }
        }
        break;
      }
      case STATE_DATA: {
        cur_packet[cur_packet_index++] = b;
        if (cur_packet_index >= this->cur_msg_len) {
          //We have our Packet now verify it
          SbfHeader *header = (SbfHeader *)cur_packet;
          if (this->calculate_crc_16_ccitt(cur_packet + CRC_START_OFFSET,
                                           header->length - CRC_START_OFFSET) ==
              header->crc) {
            if (this->callbackSBFFrame != NULL)
              this->callbackSBFFrame(header->length, cur_packet);
          }
          parser_state = STATE_SYNC1;
        }
        break;
      }

      case STATE_CMD_RESPONSE: {
        cur_packet[cur_packet_index++] = b;
        if (this->cur_msg_len >= MAX_SBF_MSG_LEN) {
          LOG(WARNING) <<"Too big a message?";
          parser_state = STATE_SYNC1;
          break;
        }
          //We have our Packet now verify it
        if (cur_packet[cur_packet_index - 2] == '\r' && cur_packet[cur_packet_index - 1] == '\n') {
          cur_packet[cur_packet_index] = 0x00;
          if (this->callbackResponse != NULL)
            this->callbackResponse((char *) cur_packet);
          parser_state = STATE_SYNC1;
        }
        break;
      }

    }
  }

 private:

  std::function<void(uint16_t, uint8_t *)> callbackSBFFrame;
  std::function<void(char *)> callbackResponse;

  enum MsgState {
    STATE_DONE = 1,
    STATE_SYNC1 = 2,
    STATE_SYNC2 = 3,
    STATE_HEADER = 4,
    STATE_DATA = 5,
    STATE_CMD_RESPONSE = 6,
  };

  uint8_t parser_state;



  uint16_t cur_packet_index;
  uint16_t cur_msg_len;

  uint32_t bad_sync_counter;

  static constexpr uint16_t MAX_SBF_MSG_LEN = 4096;
  uint8_t cur_packet[MAX_SBF_MSG_LEN + 1];
  //CRC calc starts with the ID, so skip 2 Sync bytes and 2 CRC btyes in header.
  const uint8_t CRC_START_OFFSET = 4;

  uint16_t crc_ccitt_update(uint8_t x, uint16_t current_crc) {
    uint16_t crc_new = (uint8_t) (current_crc >> 8) | (current_crc << 8);
    crc_new ^= x;
    crc_new ^= (uint8_t) (crc_new & 0xff) >> 4;
    crc_new ^= crc_new << 12;
    crc_new ^= (crc_new & 0xff) << 5;
    return crc_new;
  }

  uint16_t calculate_crc_16_ccitt(uint8_t *data, uint16_t len) {
    uint16_t crc_new = 0;
    for (int i = 0; i < len; i++) {
      crc_new = crc_ccitt_update(data[i], crc_new);
    }
    return crc_new;
  }

};
}
}