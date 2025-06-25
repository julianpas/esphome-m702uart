#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace m702uart {

class M702UARTComponent : public PollingComponent, public uart::UARTDevice {
 public:
  void set_eco2_sensor(sensor::Sensor *eco2_sensor) { eco2_sensor_ = eco2_sensor; }
  void set_ozone_sensor(sensor::Sensor *ozone_sensor) { ozone_sensor_ = ozone_sensor; }
  void set_tvoc_sensor(sensor::Sensor *tvoc_sensor) { tvoc_sensor_ = tvoc_sensor; }
  void set_pm_2_5_sensor(sensor::Sensor *pm_2_5_sensor) { pm_2_5_sensor_ = pm_2_5_sensor; }
  void set_pm_10_sensor(sensor::Sensor *pm_10_sensor) { pm_10_sensor_ = pm_10_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }
  void set_data_valid_binary_sensor(binary_sensor::BinarySensor *data_valid_binary_sensor) {
    data_valid_binary_sensor_ = data_valid_binary_sensor;
  }
  void set_sensor_status_text_sensor(text_sensor::TextSensor *sensor_status_text_sensor) {
    sensor_status_text_sensor_ = sensor_status_text_sensor;
  }

  void setup() override;
  void update() override;
  void loop() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  sensor::Sensor *eco2_sensor_{nullptr};
  sensor::Sensor *ozone_sensor_{nullptr};
  sensor::Sensor *tvoc_sensor_{nullptr};
  sensor::Sensor *pm_2_5_sensor_{nullptr};
  sensor::Sensor *pm_10_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  binary_sensor::BinarySensor *data_valid_binary_sensor_{nullptr};
  text_sensor::TextSensor *sensor_status_text_sensor_{nullptr};

  struct AirQualityData {
    float eco2_high;
    float ozone_high;
    float tvoc_high;
    float pm25_high;
    float pm10_high;
    float temperature;
    float humidity;
    bool is_valid;
  };

  AirQualityData current_data_{};
  uint8_t data_buffer_[17];
  int buffer_index_{0};
  bool packet_started_{false};
  uint32_t last_data_time_{0};
  
  void reset_buffer_();
  bool parse_data_packet_();
  void publish_sensors_();
  void update_sensor_status_(const std::string &status);
};

}  // namespace m702uart
}  // namespace esphome
