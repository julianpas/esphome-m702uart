#include "m702uart.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m702uart {

static const char *const TAG = "m702uart";

void M702UARTComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M702UART Air Quality Sensor...");
  
  // Initialize data structure
  memset(&current_data_, 0, sizeof(current_data_));
  reset_buffer_();
  
  // Set initial sensor status
  update_sensor_status_("Initializing");
  
  ESP_LOGCONFIG(TAG, "M702UART Air Quality Sensor setup complete");
}

void M702UARTComponent::update() {
  // This is called according to update_interval
  // Check if we've received data recently
  uint32_t now = millis();
  if (now - last_data_time_ > 60000) {  // 60 seconds timeout
    ESP_LOGW(TAG, "No data received for 60 seconds");
    update_sensor_status_("No Data");
    if (data_valid_binary_sensor_ != nullptr) {
      data_valid_binary_sensor_->publish_state(false);
    }
  }
}

void M702UARTComponent::loop() {
  // Read data from UART
  while (available()) {
    uint8_t incoming_byte = read();
    
    // Look for start of packet (Fixed bytes: 0x3C, 0x02)
    if (!packet_started_) {
      if (buffer_index_ == 0 && incoming_byte == 0x3C) {
        data_buffer_[buffer_index_++] = incoming_byte;
      } else if (buffer_index_ == 1 && incoming_byte == 0x02) {
        data_buffer_[buffer_index_++] = incoming_byte;
        packet_started_ = true;
      } else {
        reset_buffer_();  // Reset if we don't get the expected start sequence
      }
    } else {
      // Continue collecting packet data
      data_buffer_[buffer_index_++] = incoming_byte;
      
      // Check if we have a complete packet
      if (buffer_index_ >= 17) {
        if (parse_data_packet_()) {
          publish_sensors_();
          last_data_time_ = millis();
          update_sensor_status_("Online");
        } else {
          ESP_LOGW(TAG, "Failed to parse data packet");
          update_sensor_status_("Parse Error");
        }
        reset_buffer_();
      }
    }
  }
}

void M702UARTComponent::reset_buffer_() {
  buffer_index_ = 0;
  packet_started_ = false;
  memset(data_buffer_, 0, sizeof(data_buffer_));
}

bool M702UARTComponent::parse_data_packet_() {
  // Verify packet integrity (basic check)
  if (data_buffer_[0] != 0x3C || data_buffer_[1] != 0x02) {
    ESP_LOGW(TAG, "Invalid packet header: 0x%02X 0x%02X", data_buffer_[0], data_buffer_[1]);
    return false;
  }
  
  // Calculate checksum (simple sum of all data bytes)
  uint8_t calculated_checksum = 0;
  for (int i = 0; i < 16; i++) {
    calculated_checksum += data_buffer_[i];
  }
  
  uint8_t received_checksum = data_buffer_[16];
  
  // Parse sensor values
  current_data_.eco2_high = (data_buffer_[2] << 8) | data_buffer_[3];
  current_data_.ozone_high = (data_buffer_[4] << 8) | data_buffer_[5];
  current_data_.tvoc_high = (data_buffer_[6] << 8) | data_buffer_[7];
  current_data_.pm25_high = (data_buffer_[8] << 8) | data_buffer_[9];
  current_data_.pm10_high = (data_buffer_[10] << 8) | data_buffer_[11];
  
  // Temperature calculation (based on the specification)
  int temp_integer = data_buffer_[12];
  int temp_decimal = data_buffer_[13];
  
  // Check if temperature is negative (bit 7 of integer part)
  if (temp_integer & 0x80) {
    // Negative temperature
    temp_integer = temp_integer & 0x7F;  // Remove sign bit
    current_data_.temperature = -(float)temp_integer - ((float)temp_decimal / 100.0);
  } else {
    // Positive temperature
    current_data_.temperature = (float)temp_integer + ((float)temp_decimal / 100.0);
  }
  
  // Humidity calculation
  int hum_integer = data_buffer_[14];
  int hum_decimal = data_buffer_[15];
  current_data_.humidity = (float)hum_integer + ((float)hum_decimal / 100.0);
  
  // Mark data as valid (simplified validation)
  current_data_.is_valid = true;
  
  ESP_LOGD(TAG, "Parsed data - eCO2: %.0f ppm, O3: %.0f ppb, TVOC: %.0f ppb, PM2.5: %.0f µg/m³, PM10: %.0f µg/m³, Temp: %.2f°C, Humidity: %.2f%%",
           current_data_.eco2_high, current_data_.ozone_high, current_data_.tvoc_high,
           current_data_.pm25_high, current_data_.pm10_high,
           current_data_.temperature, current_data_.humidity);
  
  return true;
}

void M702UARTComponent::publish_sensors_() {
  if (!current_data_.is_valid) {
    return;
  }
  
  // Publish sensor values to Home Assistant
  if (eco2_sensor_ != nullptr) {
    eco2_sensor_->publish_state(current_data_.eco2_high);
  }
  
  if (ozone_sensor_ != nullptr) {
    ozone_sensor_->publish_state(current_data_.ozone_high);
  }
  
  if (tvoc_sensor_ != nullptr) {
    tvoc_sensor_->publish_state(current_data_.tvoc_high);
  }
  
  if (pm_2_5_sensor_ != nullptr) {
    pm_2_5_sensor_->publish_state(current_data_.pm25_high);
  }
  
  if (pm_10_sensor_ != nullptr) {
    pm_10_sensor_->publish_state(current_data_.pm10_high);
  }
  
  if (temperature_sensor_ != nullptr) {
    temperature_sensor_->publish_state(current_data_.temperature);
  }
  
  if (humidity_sensor_ != nullptr) {
    humidity_sensor_->publish_state(current_data_.humidity);
  }
  
  if (data_valid_binary_sensor_ != nullptr) {
    data_valid_binary_sensor_->publish_state(current_data_.is_valid);
  }
}

void M702UARTComponent::update_sensor_status_(const std::string &status) {
  if (sensor_status_text_sensor_ != nullptr) {
    sensor_status_text_sensor_->publish_state(status);
  }
}

}  // namespace m702uart
}  // namespace esphome
