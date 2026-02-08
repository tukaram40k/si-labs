#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

/**
 * @brief Class to control an LED connected to a specific pin
 */
class LedController {
public:
  /**
   * @brief Construct a new Led Controller object
   * @param pin The digital pin to which the LED is connected
   */
  LedController(int pin);

  /**
   * @brief Initialize the LED pin as output
   */
  void setup();

  /**
   * @brief Turn the LED on
   */
  void turnOn();

  /**
   * @brief Turn the LED off
   */
  void turnOff();

  /**
   * @brief Toggle the LED state
   */
  void toggle();

  /**
   * @brief Check if the LED is currently on
   * @return true if the LED is on, false otherwise
   */
  bool isOn() const;

private:
  int m_pin;    // Pin to which the LED is connected
  bool m_isOn;    // Current state of the LED
};

#endif // LED_CONTROLLER_H