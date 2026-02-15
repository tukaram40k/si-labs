#ifndef IO_H
#define IO_H

#include <Arduino.h>
#include <stdio.h>
#include "../LCDDriver/LCDController.h"
#include "../KeypadDriver/KeypadController.h"

namespace IO {
  // helper for std output
  int serial_putchar(char c, FILE *stream);
  
  // helper for LCD output
  int lcd_putchar(char c, FILE *stream);

  // helper for keypad input
  int keypad_getchar(FILE *stream);

  // helper for std input
  int serial_getchar(FILE *stream);

  // setup stream redirection
  void setup(LCDController* lcd, KeypadController* keypad);

  // get reference to LCD controller
  LCDController* getLCD();

  // get reference to Keypad controller
  KeypadController* getKeypad();
}

#endif