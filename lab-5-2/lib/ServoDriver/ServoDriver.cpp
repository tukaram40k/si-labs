#include "ServoDriver.h"

#include <Servo.h>

namespace {
  // One servo instance for this lab.
  Servo g_servo;
  bool g_attached = false;
}

void ServoDriver::setup(const Config& cfg) {
  m_pin = cfg.pin;
  if (!g_attached) {
    g_servo.attach(m_pin);
    g_attached = true;
  }
  m_lastDeg = 0;
  g_servo.write(m_lastDeg);
}

void ServoDriver::writeDeg(int deg) {
  if (deg < FLIP_MIN_DEG) {
    deg = FLIP_MIN_DEG;
  } else if (deg > 180) {
    deg = 180;
  }

  m_lastDeg = deg;
  g_servo.write(m_lastDeg);
}

void ServoDriver::start_flipping() {
  if (!m_flipping) {
    m_flipping = true;
    m_direction = 1;
    m_lastStepMs = millis();
    writeDeg(FLIP_MIN_DEG);
  }
}

void ServoDriver::stop_flipping() {
  m_flipping = false;
  m_direction = 1;
  writeDeg(FLIP_MIN_DEG);
}

void ServoDriver::setFlipStepPeriodMs(uint16_t periodMs) {
  if (periodMs == 0) {
    periodMs = 1;
  }
  m_stepPeriodMs = periodMs;
}

void ServoDriver::tick() {
  if (!m_flipping) {
    return;
  }

  const uint32_t now = millis();
  if ((uint32_t)(now - m_lastStepMs) < m_stepPeriodMs) {
    return;
  }
  m_lastStepMs = now;

  int nextDeg = m_lastDeg + (m_direction * FLIP_STEP_DEG);
  if (nextDeg >= FLIP_MAX_DEG) {
    nextDeg = FLIP_MAX_DEG;
    m_direction = -1;
  } else if (nextDeg <= FLIP_MIN_DEG) {
    nextDeg = FLIP_MIN_DEG;
    m_direction = 1;
  }

  writeDeg(nextDeg);
}

int ServoDriver::getLastWrittenDeg() const {
  return m_lastDeg;
}
