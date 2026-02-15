#include "IO.h"

static FILE lcd_stream;
// static FILE serial_stream;

// Static LCD and Keypad instances within namespace
static LCDController* _lcd = nullptr;
static KeypadController* _keypad = nullptr;

namespace IO {
  int serial_putchar(char c, FILE *stream) {
    Serial.write(c);
    if (c == '\n') {
      Serial.write('\r');
    }
    return 0;
  }

  int lcd_putchar(char c, FILE *stream) {
    static char buffer[33];
    static int pos = 0;
    static int row = 0;
    
    if (c == '\n' || pos >= 16) {
      // Print the current line to the LCD
      if (pos > 0) {
        buffer[pos] = '\0';
        if (_lcd != nullptr) {
          _lcd->printAt(0, row, buffer);
        }
        pos = 0;
      }
      
    } else if (c >= 32 && c <= 126) { // Printable ASCII characters
      buffer[pos++] = c;
    }
    
    return 0;
  }

  int keypad_getchar(FILE *stream) {
    char key = 0;
    // Poll the keypad until a key is pressed
    while(key == 0) {
      key = _keypad->getKey();
      delay(50); // Small delay to prevent excessive polling
    }
    return key;
  }

  int serial_getchar(FILE *stream) {
    while (!Serial.available());
    char c = Serial.read();
    Serial.write(c);
    // if (c != '\r') {
    //   Serial.write(c);
    // } else {
    //   Serial.write('\n');
    //   Serial.write('\r');
    // }
    return c;
  }

  void setup() {
    // Initialize LCD and Keypad controllers
    _lcd = new LCDController(0x27, 16, 2);  // Common I2C address for 16x2 LCD
    _keypad = new KeypadController(7, 6, 5, 4, 3, 2, 1, 0); // Example pin assignments
    
    _lcd->setup();
    _keypad->setup();
    
    // Clear the LCD
    _lcd->clear();

    // start serial
    Serial.begin(9600);

    // redirect std to lcd
    fdev_setup_stream(&lcd_stream, lcd_putchar, keypad_getchar, _FDEV_SETUP_RW);
    
    stdout = &lcd_stream;  // Redirect stdout to LCD
    stdin = &lcd_stream;   // Redirect stdin to keypad input
  }
  
  LCDController* getLCD() {
    return _lcd;
  }
  
  KeypadController* getKeypad() {
    return _keypad;
  }
}