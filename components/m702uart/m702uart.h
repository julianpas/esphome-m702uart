#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace n702b {

class N702BComponent : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Sensor setters
  void set_eco2_sensor(sensor::Sensor *eco2_sensor) { eco2_sensor_ = eco2_sensor; }
  void set_o3_sensor(sensor::Sensor *o3_sensor) { o3_sensor_ = o3_sensor; }
  void set_tvoc_sensor(sensor::Sensor *tvoc_sensor) { tvoc_sensor_ = tvoc_sensor; }
  void set_pm25_sensor(sensor::Sensor *pm25_sensor) { pm25_sensor_ = pm25_sensor; }
  void set_pm10_sensor(sensor::Sensor *pm10_sensor) { pm10_sensor_ = pm10_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }
  void set_health_risk_score_sensor(sensor::Sensor *health_risk_score_sensor) { health_risk_score_sensor_ = health_risk_score_sensor; }

  // Text sensor setters
  void set_air_quality_index_sensor(text_sensor::TextSensor *aqi_sensor) { aqi_sensor_ = aqi_sensor; }
  void set_co2_status_sensor(text_sensor::TextSensor *co2_status_sensor) { co2_status_sensor_ = co2_status_sensor; }
  void set_o3_status_sensor(text_sensor::TextSensor *o3_status_sensor) { o3_status_sensor_ = o3_status_sensor; }
  void set_tvoc_status_sensor(text_sensor::TextSensor *tvoc_status_sensor) { tvoc_status_sensor_ = tvoc_status_sensor; }
  void set_temperature_status_sensor(text_sensor::TextSensor *temp_status_sensor) { temp_status_sensor_ = temp_status_sensor; }
  void set_humidity_status_sensor(text_sensor::TextSensor *humidity_status_sensor) { humidity_status_sensor_ = humidity_status_sensor; }
  void set_health_risk_level_sensor(text_sensor::TextSensor *health_risk_level_sensor) { health_risk_level_sensor_ = health_risk_level_sensor; }
  void set_mold_risk_sensor(text_sensor::TextSensor *mold_risk_sensor) { mold_risk_sensor_ = mold_risk_sensor; }
  void set_primary_recommendation_sensor(text_sensor::TextSensor *primary_recommendation_sensor) { primary_recommendation_sensor_ = primary_recommendation_sensor; }

 protected:
  // Air quality data structure
  struct AirQualityData {
    float eco2_high{0};
    float eco2_low{0};
    float o3_high{0};
    float o3_low{0};
    float tvoc_high{0};
    float tvoc_low{0};
    float pm25_high{0};
    float pm25_low{0};
    float pm10_high{0};
    float pm10_low{0};
    float temperature{0};
    float humidity{0};
    bool is_valid{false};
  };

  // Data processing methods
  void parse_and_process_data_();
  void reset_buffer_();
  void publish_sensor_data_();
  void publish_text_sensor_data_();
  
  // Analysis methods
  int calculate_health_risk_score_();
  std::string get_health_risk_level_(int score);
  std::string get_air_quality_index_(float pm25, float pm10);
  std::string analyze_co2_level_(float co2);
  std::string analyze_ozone_level_(float o3);
  std::string analyze_tvoc_level_(float tvoc);
  std::string analyze_temperature_(float temp);
  std::string analyze_humidity_(float humidity);
  std::string assess_mold_risk_();
  std::string get_primary_recommendation_();
  
  // Short status methods for display
  std::string get_co2_status_short_();
  std::string get_pm_status_short_();
  std::string get_temp_status_short_();
  std::string get_humidity_status_short_();
  std::string get_aqi_short_(float pm25, float pm10);

  // Data members
  AirQualityData current_data_{};
  uint8_t data_buffer_[17]{};
  size_t buffer_index_{0};
  bool packet_started_{false};
  uint32_t last_valid_data_{0};
  
  // Sensor pointers
  sensor::Sensor *eco2_sensor_{nullptr};
  sensor::Sensor *o3_sensor_{nullptr};
  sensor::Sensor *tvoc_sensor_{nullptr};
  sensor::Sensor *pm25_sensor_{nullptr};
  sensor::Sensor *pm10_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  sensor::Sensor *health_risk_score_sensor_{nullptr};

  // Text sensor pointers
  text_sensor::TextSensor *aqi_sensor_{nullptr};
  text_sensor::TextSensor *co2_status_sensor_{nullptr};
  text_sensor::TextSensor *o3_status_sensor_{nullptr};
  text_sensor::TextSensor *tvoc_status_sensor_{nullptr};
  text_sensor::TextSensor *temp_status_sensor_{nullptr};
  text_sensor::TextSensor *humidity_status_sensor_{nullptr};
  text_sensor::TextSensor *health_risk_level_sensor_{nullptr};
  text_sensor::TextSensor *mold_risk_sensor_{nullptr};
  text_sensor::TextSensor *primary_recommendation_sensor_{nullptr};

  // Constants
  static const uint8_t PACKET_START_BYTE_1 = 0x3C;
  static const uint8_t PACKET_START_BYTE_2 = 0x02;
  static const size_t PACKET_SIZE = 17;
  static const uint32_t DATA_TIMEOUT_MS = 30000; // 30 seconds timeout for data validity
};

}  // namespace n702b
}  // namespace esphome
