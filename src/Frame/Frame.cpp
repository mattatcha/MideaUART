#include "Frame/Frame.h"
#include "Helpers/Log.h"

namespace dudanov {
namespace midea {

static const char *TAG = "Frame";

void Frame::setData(const FrameData &data) {
  this->m_trimData();
  this->m_appendData(data);
  // this->m_data[OFFSET_LENGTH] = this->m_data.size();
  // this->m_data[OFFSET_SYNC] = this->m_data[OFFSET_LENGTH] ^ this->m_data[OFFSET_APPTYPE];
  this->m_data.push_back(0x00);
  this->m_data.push_back(0x55);
  this->m_setCS();
}

// uint8_t Frame::m_calcCS() const
// {
//   if (this->m_data.size() <= OFFSET_LENGTH)
//   {
//     LOG_D(TAG, "Invalid frame length");
//     return -1;
//   }
//   uint8_t cs = 0;
//   for (auto it = this->m_data.begin(); it != this->m_data.end() - 3; ++it)
//   {
//     LOG_D(TAG, "it %02X", *it);
//     cs += *it;
//   }

//   LOG_D(TAG, "checksum %02X", cs);
//   LOG_D(TAG, "checksum %02X", 0xFF - (cs & 0xFF));
//   return 0xFF - (cs & 0xFF);
// }

uint8_t Frame::m_calcCS() const {
  if (this->m_data.size() <= OFFSET_LENGTH) {
    LOG_D(TAG, "Invalid frame length");
    return -1;
  }
  uint8_t cs = 0;
  uint8_t i = 0;
  uint32_t crc = 0;
  // for (auto it = this->m_data.begin(); it != this->m_data.end(); ++it)
  // {
  //   if (i == 30)
  //   {
  //     continue;
  //   }
  //   i++;
  //   cs += *it;
  //   crc += *it;
  // }
  uint8_t size = this->m_data.size();
  LOG_D(TAG, "size %d", size);
  for (i = 0; i < size; i++) {
    if ((size == RESPONSE_LENGTH && 30 != i) || (size == REQUEST_LENGTH && 14 != i)) {
      crc += this->m_data[i];
    }
  }

  LOG_D(TAG, "checksum %02X", 0xFF - (crc & 0xFF));
  return 0xFF - (crc & 0xFF);
}

static char u4hex(uint8_t num) { return num + ((num < 10) ? '0' : ('A' - 10)); }

String Frame::toString() const {
  String ret;
  char buf[4];
  buf[2] = ' ';
  buf[3] = '\0';
  ret.reserve(3 * this->size());
  for (const uint8_t data : this->m_data) {
    buf[0] = u4hex(data / 16);
    buf[1] = u4hex(data % 16);
    ret += buf;
  }
  return ret;
}

}  // namespace midea
}  // namespace dudanov
