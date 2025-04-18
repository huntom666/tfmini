#include "tfmini.h"
#include "esphome/core/log.h"
#include <vector>

namespace esphome {
namespace tfmini {

static const char *const TAG = "tfmini.sensor";

void TFminiSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TFmini Sensor...");
  // Initialization code here, if needed (e.g., sending config commands)
  // Note: UART device setup (like baud rate) is handled by the framework
  // based on the YAML configuration passed via sensor.py
}

void TFminiSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "TFmini Sensor:");
  LOG_SENSOR("  ", "Distance", this);
  LOG_UPDATE_INTERVAL(this);
  this->check_uart_settings(115200); // Log expected baud rate
}

float TFminiSensor::get_setup_priority() const {
  // Set component setup priority (can be adjusted if needed)
  return setup_priority::DATA;
}

void TFminiSensor::update() {
  // This method is called based on update_interval

  // Read all available bytes from UART
  uint8_t byte;
  while (this->available()) {
    if (this->read_byte(&byte)) {
      this->buffer_.push_back(byte);
    }
  }

  // Check if buffer has potential frame start
  // Basic frame finding logic - looks for two headers
  // More robust logic might be needed depending on sensor behavior
  while (this->buffer_.size() >= 2) {
    if (this->buffer_[0] == TFMINI_HEADER && this->buffer_[1] == TFMINI_HEADER) {
      // Found potential frame start
      if (this->buffer_.size() >= TFMINI_FRAME_LENGTH) {
        // Have enough bytes for a full frame
        std::vector<uint8_t> frame(this->buffer_.begin(), this->buffer_.begin() + TFMINI_FRAME_LENGTH);

        // Attempt to parse the frame
        if (parse_tfmini_data_(frame)) {
          // Successfully parsed, remove frame from buffer
          this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + TFMINI_FRAME_LENGTH);
          // Optional: break if you only want one reading per update cycle
          // break;
        } else {
          // Parsing failed (e.g., bad checksum), remove the header bytes and retry
          ESP_LOGW(TAG, "TFmini frame checksum failed or invalid data.");
          this->buffer_.erase(this->buffer_.begin(), this->buffer_.begin() + 2); // Remove the two header bytes
        }
      } else {
        // Not enough bytes for a full frame yet, wait for more data
        break;
      }
    } else {
      // First byte isn't a header, discard it and continue searching
      this->buffer_.erase(this->buffer_.begin());
    }
  }

  // Limit buffer size to prevent excessive memory usage if frames are never found
  if (this->buffer_.size() > TFMINI_FRAME_LENGTH * 3) {
      ESP_LOGW(TAG, "Clearing excessive data from TFmini buffer (%d bytes)", this->buffer_.size());
      this->buffer_.clear();
  }
}

bool TFminiSensor::parse_tfmini_data_(const std::vector<uint8_t>& data) {
  // *** IMPORTANT: Implement the actual data parsing logic here ***
  // Based on the TFmini Plus datasheet communication protocol (UART).
  // A standard frame looks like:
  // 0x59 0x59 Dist_L Dist_H Strength_L Strength_H Reserved_L Reserved_H Checksum

  if (data.size() < TFMINI_FRAME_LENGTH) {
    return false; // Not enough data
  }

  // 1. Verify Checksum
  uint8_t checksum = 0;
  for (size_t i = 0; i < TFMINI_FRAME_LENGTH - 1; ++i) {
    checksum += data[i];
  }

  if (checksum != data[TFMINI_FRAME_LENGTH - 1]) {
    ESP_LOGW(TAG, "TFmini checksum mismatch: calculated=0x%02X, received=0x%02X", checksum, data[TFMINI_FRAME_LENGTH - 1]);
    // You might want to log the frame data here for debugging
    // ESP_LOGD(TAG, "Frame: %02X %02X %02X %02X %02X %02X %02X %02X %02X", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
    return false; // Checksum failed
  }

  // 2. Extract Distance (Dist_L and Dist_H)
  uint16_t distance_cm = (uint16_t(data[3]) << 8) | data[2];

  // 3. Convert to meters (since default unit in sensor.py is meters)
  float distance_m = distance_cm / 100.0f;

  // 4. Extract Strength (optional, could be another sensor)
  // uint16_t strength = (uint16_t(data[5]) << 8) | data[4];
  // ESP_LOGD(TAG, "TFmini Strength: %u", strength);

  // 5. Publish the distance value
  ESP_LOGD(TAG, "TFmini Distance: %.2f m (%u cm)", distance_m, distance_cm);
  this->publish_state(distance_m);

  return true; // Successfully parsed
}

}  // namespace tfmini
}  // namespace esphome
