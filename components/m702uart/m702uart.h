#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace n702b {

class N702BComponent : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_eco2_sensor(sensor::Sensor *eco2_sensor) { eco2_sensor_ = eco2_sensor; }
  void set_o3_sensor(sensor::Sensor *o3_sensor) { o3_sensor_ = o3_sensor; }
  void set_tvoc_sensor(sensor::Sensor *tvoc_sensor) { tvoc_sensor_ = tvoc_sensor; }
  void set_pm25_sensor(sensor::Sensor *pm25_sensor) { pm25_sensor_ = pm25_sensor; }
  void set_pm10_sensor(sensor::Sensor *pm10_sensor) { pm10_sensor_ = pm10_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }

 protected:
  struct AirQualityData {
    float eco2_high;
    float eco
