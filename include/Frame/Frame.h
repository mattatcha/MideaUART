#pragma once
#include <Arduino.h>
#include <vector>
#include "Frame/FrameData.h"
#include "Helpers/Helpers.h"

namespace dudanov {
namespace midea {

class Frame {
 public:
  Frame() = default;
  // AA C4 00 00 00 00 00 00 00 00 00 00 00 3B 01
  // AA C0 00 00 00 00 00 00 00 80 00 00 00 00 00 00 00 00 00 00 67 AE
  Frame(uint8_t type, const FrameData &data) : m_data({START_BYTE, type, 0x02, 0x03}) { this->setData(data); }
  // Frame(uint8_t appliance, uint8_t protocol, uint8_t type, const FrameData &data)
  //     : m_data({START_BYTE, 0x00, appliance, 0x00, 0x00, 0x00, 0x00, 0x00, protocol, type})
  // {
  //   this->setData(data);
  // }
  FrameData getData() const { return FrameData(this->m_data.data(), REQUEST_LENGTH); }
  // FrameData getData() const { return FrameData(this->m_data.data() + OFFSET_DATA, REQUEST_LENGTH); }
  void setData(const FrameData &data);
  bool isValid() const { return this->m_calcCS() == this->m_data[0x1E]; }

  const uint8_t *data() const { return this->m_data.data(); }
  uint8_t size() const { return this->m_data.size(); }
  void setType(uint8_t value) { this->m_data[OFFSET_TYPE] = value; }
  bool hasType(uint8_t value) const { return this->m_data[OFFSET_TYPE] == value; }
  void setProtocol(uint8_t value) { this->m_data[OFFSET_PROTOCOL] = value; }
  uint8_t getProtocol() const { return this->m_data[OFFSET_PROTOCOL]; }
  String toString() const;

 protected:
  std::vector<uint8_t> m_data;
  void m_trimData() { this->m_data.erase(this->m_data.begin() + OFFSET_DATA, this->m_data.end()); }
  void m_appendData(const FrameData &data) {
    std::copy(data.data(), data.data() + data.size(), std::back_inserter(this->m_data));
  }
  // uint8_t m_len() const { return this->m_data[OFFSET_LENGTH]; }
  void m_setCS() { this->m_data[REQUEST_LENGTH - 2] = this->m_calcCS(); }
  // void m_appendCS() { this->m_data.push_back(this->m_calcCS()); }
  uint8_t m_calcCS() const;
  static const uint8_t START_BYTE = 0xAA;
  static const uint8_t REQUEST_LENGTH = 16;
  static const uint8_t RESPONSE_LENGTH = 32;
  static const uint8_t END_BYTE = 0x55;
  static const uint8_t OFFSET_TYPE = 1;
  static const uint8_t OFFSET_DATA = 4;

  static const uint8_t OFFSET_START = 0;
  static const uint8_t OFFSET_LENGTH = 1;
  static const uint8_t OFFSET_APPTYPE = 2;
  static const uint8_t OFFSET_SYNC = 3;
  static const uint8_t OFFSET_PROTOCOL = 8;
  // static const uint8_t OFFSET_TYPE = 9;
};

}  // namespace midea
}  // namespace dudanov
