#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

// class to control led
class LedController {
public:
  // make a new Led Controller on a pin
  LedController(int pin);

  // setup the led pin as output
  void setup();

  // turn led on
  void turnOn();

  // turn led on
  void turnOff();

  // toggle led state
  void toggle();

  // check if led is on
  bool isOn() const;

private:
  int m_pin; // led pin
  bool m_isOn; // led state
};

#endif