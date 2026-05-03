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

  void start_flipping();
  void stop_flipping();
  void setFlipStepPeriodMs(uint16_t periodMs);
  void tick();

  int getLastWrittenDeg() const;

private:
  uint8_t m_pin = 255;
  int m_lastDeg = 0;
  bool m_flipping = false;
  int m_direction = 1;
  uint32_t m_lastStepMs = 0;
  uint16_t m_stepPeriodMs = FLIP_STEP_PERIOD_MS;

  static constexpr int FLIP_MIN_DEG = 0;
  static constexpr int FLIP_MAX_DEG = 90;
  static constexpr int FLIP_STEP_DEG = 5;
  static constexpr uint16_t FLIP_STEP_PERIOD_MS = 10;
};
