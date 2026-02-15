#ifndef KEYPAD_CONTROLLER_H
#define KEYPAD_CONTROLLER_H

#include <Arduino.h>

// class to control 4x4 keypad
class KeypadController {
public:
  // make a new Keypad Controller with pins for rows and columns
  KeypadController(int row1, int row2, int row3, int row4, int col1, int col2, int col3, int col4);

  // setup the keypad pins
  void setup();

  // get the pressed key (returns 0 if no key is pressed)
  char getKey();

  // get the entire code sequence (waits for # to confirm entry)
  String getCode();

private:
  int m_rowPins[4]; // row pins
  int m_colPins[4]; // column pins
  char m_keymap[4][4]; // key mapping
  
  // initialize keymap
  void initKeymap();
  
  // scan the keypad matrix
  char scanKeypad();
};

#endif