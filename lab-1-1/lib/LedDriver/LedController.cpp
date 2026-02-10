#include "LedController.h"

LedController::LedController(int pin) : m_pin(pin), m_isOn(false) {

}

void LedController::setup() {
  pinMode(m_pin, OUTPUT);
  digitalWrite(m_pin, LOW);
  m_isOn = false;
}

void LedController::turnOn() {
  digitalWrite(m_pin, HIGH);
  m_isOn = true;
}

void LedController::turnOff() {
  digitalWrite(m_pin, LOW);
  m_isOn = false;
}

void LedController::toggle() {
  if (m_isOn) {
    turnOff();
  } else {
    turnOn();
  }
}

bool LedController::isOn() const {
  return m_isOn;
}