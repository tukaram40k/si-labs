#pragma once

#include <Arduino.h>

// Thin wrapper around the Arduino Servo library.
// Keeps actuator logic (task scheduling, rate limiting) separate from hardware I/O.
class ServoDriver {
public:
  struct Config {
    uint8_t pin;
  };

  ServoDriver() = default;

  void setup(const Config& cfg);

  // Writes an angle in degrees (typical 0..180).
  void writeDeg(int deg);

  int getLastWrittenDeg() const;

private:
  uint8_t m_pin = 255;
  int m_lastDeg = 0;

  // PIMPL-ish: keep Servo include out of the header if needed later.
  // For this small project we keep it simple and store it in the .cpp.
};
