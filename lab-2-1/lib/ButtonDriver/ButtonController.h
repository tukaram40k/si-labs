#ifndef BUTTON_CONTROLLER_H
#define BUTTON_CONTROLLER_H

#include <Arduino.h>

// class to read button state
class ButtonController {
public:
  // make a new Button Controller on a pin
  ButtonController(int pin, bool activeLow = true);

  // setup the button pin as input with pull-up
  void setup();

  // read button state (returns true when pressed)
  bool isPressed() const;

  // read raw pin state
  bool read() const;

private:
  int m_pin;       // button pin
  bool m_activeLow; // button active state (true = active low)
};

#endif
