#pragma once

#include <Arduino.h>
#include <stdint.h>

class Dht11Driver {
public:
  struct Config {
    uint8_t dataPin;
  };

  Dht11Driver() = default;

  void setup(const Config& cfg);

  // Returns true when a valid humidity sample was read.
  bool readHumidityPercent(float& outHumidityPercent);

private:
  uint8_t m_dataPin = 255;
};
