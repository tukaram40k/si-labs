#include "KeypadController.h"

KeypadController::KeypadController(int row1, int row2, int row3, int row4, int col1, int col2, int col3, int col4) {
  m_rowPins[0] = row1;
  m_rowPins[1] = row2;
  m_rowPins[2] = row3;
  m_rowPins[3] = row4;
  m_colPins[0] = col1;
  m_colPins[1] = col2;
  m_colPins[2] = col3;
  m_colPins[3] = col4;
  
  initKeymap();
}
// init
void KeypadController::initKeymap() {
  m_keymap[0][0] = '1'; m_keymap[0][1] = '2'; m_keymap[0][2] = '3'; m_keymap[0][3] = 'A';
  m_keymap[1][0] = '4'; m_keymap[1][1] = '5'; m_keymap[1][2] = '6'; m_keymap[1][3] = 'B';
  m_keymap[2][0] = '7'; m_keymap[2][1] = '8'; m_keymap[2][2] = '9'; m_keymap[2][3] = 'C';
  m_keymap[3][0] = '*'; m_keymap[3][1] = '0'; m_keymap[3][2] = '#'; m_keymap[3][3] = 'D';
}

// Setup the keypad pins
void KeypadController::setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(m_rowPins[i], OUTPUT);
    digitalWrite(m_rowPins[i], HIGH);
  }
  
  // Set all column pins as inputs with pull-up resistors
  for (int i = 0; i < 4; i++) {
    pinMode(m_colPins[i], INPUT_PULLUP);
  }
}

// Scan the keypad matrix to detect pressed key
char KeypadController::scanKeypad() {
  for (int row = 0; row < 4; row++) {
    // Set current row LOW while keeping others HIGH
    for (int r = 0; r < 4; r++) {
      digitalWrite(m_rowPins[r], HIGH);
    }
    digitalWrite(m_rowPins[row], LOW);
    
    // Check each column for a LOW signal (key press)
    for (int col = 0; col < 4; col++) {
      if (digitalRead(m_colPins[col]) == LOW) {
        delay(50);
        
        // Double check if key is still pressed after debounce
        if (digitalRead(m_colPins[col]) == LOW) {
          // Wait for key release before returning
          while (digitalRead(m_colPins[col]) == LOW) {
            delay(10);
          }
          
          return m_keymap[row][col];
        }
      }
    }
  }
  
  return 0; // No key pressed
}

// Get the entire code sequence (waits for # to confirm entry)
String KeypadController::getCode() {
  String code = "";
  char key;
  
  while (true) {
    key = getKey();
    if (key != 0) {
      if (key >= '0' && key <= '9') {
        code += key;
      } else if (key == '#') {
        // Confirmation key pressed, return the code
        return code;
      } else if (key == '*') {
        // Clear key pressed, reset the code
        code = "";
      }
    }
    delay(50); // Small delay to prevent excessive polling
  }
}

// Public method to get a single key press
char KeypadController::getKey() {
  return scanKeypad();
}