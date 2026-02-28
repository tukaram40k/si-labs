#include "ButtonController.h"

ButtonController::ButtonController(int pin, bool activeLow) : m_pin(pin), m_activeLow(activeLow) {

}

void ButtonController::setup() {
  pinMode(m_pin, INPUT_PULLUP);
}

bool ButtonController::isPressed() const {
  bool state = digitalRead(m_pin);
  return m_activeLow ? (state == LOW) : (state == HIGH);
}

bool ButtonController::read() const {
  return digitalRead(m_pin);
}
