#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace tfmini {

// Define constants for TFmini communication based on datasheet
const uint8_t TFMINI_HEADER = 0x59;
const size_t TFMINI_FRAME_LENGTH = 9; // Standard data frame length

class TFminiSensor : public sensor::Sensor, public PollingComponent, public uart::UARTDevice {
 public:
  // Constructor - gets UART configuration
  TFminiSensor() = default;

  // Standard ESPHome component methods
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

 protected:
  // Helper method to parse incoming data frame
  bool parse_tfmini_data_(const std::vector<uint8_t>& data);

  // Internal buffer to store incoming UART data
  std::vector<uint8_t> buffer_;
};

}  // namespace tfmini
}  // namespace esphome

