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
  m_lastDeg = deg;
  g_servo.write(m_lastDeg);
}

int ServoDriver::getLastWrittenDeg() const {
  return m_lastDeg;
}
