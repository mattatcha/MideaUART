#pragma once
#include <Arduino.h>
#include "Frame/FrameData.h"

namespace dudanov {
namespace midea {
namespace ac {

/// Enum for all modes a Midea device can be in.
enum Mode : uint8_t {
  ///
  MODE_OFF = 0x00,
  /// The Midea device is set to automatically change the heating/cooling cycle
  MODE_AUTO = 0x80,
  /// The Midea device is manually set to cool mode (not in auto mode!)
  MODE_COOL = 0x88,
  /// The Midea device is manually set to dry mode
  MODE_DRY = 0x82,
  /// The Midea device is manually set to heat mode (not in auto mode!)
  MODE_HEAT = 0x84,
  /// The Midea device is manually set to fan only mode
  MODE_FAN_ONLY = 0x81,
};

/// Enum for all modes a Midea fan can be in
enum FanMode : uint8_t {
  /// The fan mode is set to Auto
  FAN_AUTO = 0x80,
  /// The fan mode is set to Silent
  FAN_SILENT = 20,
  /// The fan mode is set to Low
  FAN_LOW = 0x04,
  /// The fan mode is set to Medium
  FAN_MEDIUM = 0x02,
  /// The fan mode is set to High
  FAN_HIGH = 0x01,
  /// The fan mode is set to Turbo
  FAN_TURBO = 100,
};

/// Enum for all modes a Midea swing can be in
enum SwingMode : uint8_t {
  /// The sing mode is set to Off
  SWING_OFF = 0b0000,
  /// The fan mode is set to Both
  SWING_BOTH = 0b1111,
  /// The fan mode is set to Vertical
  SWING_VERTICAL = 0b1100,
  /// The fan mode is set to Horizontal
  SWING_HORIZONTAL = 0b0011,
};

/// Enum for all presets a Midea can be in
enum Preset : uint8_t {
  /// None preset
  PRESET_NONE,
  /// The SLEEP preset
  PRESET_SLEEP,
  /// The TURBO preset
  PRESET_TURBO,
  /// The ECO preset
  PRESET_ECO,
  /// The FREEZE_PROTECTION preset
  PRESET_FREEZE_PROTECTION,
};

class StatusData : public FrameData {
 public:
  StatusData()
      : FrameData({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}) {}
  StatusData(const FrameData &data) : FrameData(data) {}

  /// Copy status from another StatusData
  void copyStatus(const StatusData &p) { memcpy(this->m_data.data() + 1, p.data() + 1, 32); }

  /* TARGET TEMPERATURE */
  float getTargetTemp() const;
  void setTargetTemp(float temp);

  /* MODE */
  // Mode getRawMode() const { return static_cast<Mode>(this->m_getValue(8, 7, 5)); }
  Mode getRawMode() const { return static_cast<Mode>(this->m_getValue(8)); }
  Mode getMode() const;
  void setMode(Mode mode);

  /* FAN SPEED */
  FanMode getFanMode() const { return static_cast<FanMode>(this->m_getValue(9)); }
  void setFanMode(FanMode mode) { this->m_setValue(3, mode); };

  /* SWING MODE */
  SwingMode getSwingMode() const { return static_cast<SwingMode>(this->m_getValue(7, 15)); }
  void setSwingMode(SwingMode mode) { this->m_setValue(7, 0x30 | mode); }

  /* INDOOR TEMPERATURE */
  float getIndoorTemp() const;

  /* OUTDOOR TEMPERATURE */
  float getOutdoorTemp() const;

  /* HUMIDITY SETPOINT */
  float getHumiditySetpoint() const;

  /* PRESET */
  Preset getPreset() const;
  void setPreset(Preset preset);

  /* POWER USAGE */
  float getPowerUsage() const;

  void setBeeper(bool state) {
    this->m_setMask(1, true, 2);
    this->m_setMask(1, state, 64);
  }

 protected:
  /* POWER */
  // bool m_getPower() const { return this->m_getValue(1, 1); }
  bool m_getPower() const { return true; }
  void m_setPower(bool state) { this->m_setMask(1, state, 1); }
  /* ECO MODE */
  bool m_getEco() const { return this->m_getValue(9, 16); }
  void m_setEco(bool state) { this->m_setMask(9, state, 128); }
  /* TURBO MODE */
  bool m_getTurbo() const { return this->m_getValue(8, 32) || this->m_getValue(10, 2); }
  void m_setTurbo(bool state) {
    this->m_setMask(8, state, 32);
    this->m_setMask(10, state, 2);
  }
  /* FREEZE PROTECTION */
  bool m_getFreezeProtection() const { return this->m_getValue(21, 128); }
  void m_setFreezeProtection(bool state) { this->m_setMask(21, state, 128); }
  /* SLEEP MODE */
  bool m_getSleep() const { return this->m_getValue(10, 1); }
  void m_setSleep(bool state) { this->m_setMask(10, state, 1); }
};

// SentData[0] =  0XAA;
// SentData[1] =  MASTER_COMMAND_QERRY; 0xC0
// SentData[2] =  SlaveId; 0
// SentData[3] =  MasterId; 0
// SentData[4] =  FROM_MASTER; 0x80
// SentData[5] =  MasterId; 0
// SentData[6] =  0;
// SentData[7] =  0;
// SentData[8] =  0;
// SentData[9] =  0;
// SentData[10] =  0;
// SentData[11] =  0;
// SentData[12] =  0;
// SentData[13] =  0xFF-0xC0;
// SentData[15] =  PROLOGUE; 0x55
// SentData[14] = CalculateCRC(TRANSMIT_CRC);

class QueryStateData : public FrameData {
  // public:
  //   //  TX: AA 00 AC 00 00 00 00 00 88 03 41 81 00 FF 03 FF 00 02 00 00 00 00 00 00 00 00 00 00 00 00 03 92 9A 2D
  //   // QueryStateData() : FrameData({0x41, 0x81, 0x00, 0xFF, 0x03, 0xFF, 0x00, 0x02, 0x00, 0x00,
  //   //                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  //   //                               0x03, FrameData::m_getID()}) { this->appendCRC(); }
  //   QueryStateData() : FrameData({0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  //                                 0x00, 0x00}) {}
  // };
 public:
  //  TX: AA 00 AC 00 00 00 00 00 88 03 41 81 00 FF 03 FF 00 02 00 00 00 00 00 00 00 00 00 00 00 00 03 92 9A 2D
  // QueryStateData() : FrameData({0x41, 0x81, 0x00, 0xFF, 0x03, 0xFF, 0x00, 0x02, 0x00, 0x00,
  //                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  //                               0x03, FrameData::m_getID()}) { this->appendCRC(); }
  QueryStateData() : FrameData({0x80, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F}) {}
};

// AA C0 00 00 00 00 00 00 00 00 00 00 00 3F 01 55
class QueryPowerData : public FrameData {
 public:
  QueryPowerData()
      : FrameData({0x41,
                   0x21,
                   0x01,
                   0x44,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x00,
                   0x04,
                   FrameData::m_getID()}) {
    this->appendCRC();
  }
};

class DisplayToggleData : public FrameData {
 public:
  DisplayToggleData()
      : FrameData({0x41, 0x61, 0x00, 0xFF, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, FrameData::m_getRandom()}) {
    this->appendCRC();
  }
};

class GetCapabilitiesData : public FrameData {
 public:
  GetCapabilitiesData() : FrameData({0xB5, 0x01, 0x11}) { this->appendCRC(); }
};

class GetCapabilitiesSecondData : public FrameData {
 public:
  GetCapabilitiesSecondData() : FrameData({0xB5, 0x01, 0x01, 0x00}) { this->appendCRC(); }
};

}  // namespace ac
}  // namespace midea
}  // namespace dudanov
