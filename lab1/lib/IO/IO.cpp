#include "IO.h"

static FILE serial_stream;

namespace IO {
  int serial_putchar(char c, FILE *stream) {
    if (c == '\n') {
      Serial.write('\r');  // Send carriage return first
    }
    Serial.write(c);
    return 0;
  }

  int serial_getchar(FILE *stream) {
    while (!Serial.available());
    return Serial.read();
  }
  
  void setup() {
    // Initialize serial communication
    Serial.begin(9600);
  
    fdev_setup_stream(&serial_stream, serial_putchar, serial_getchar, _FDEV_SETUP_RW);
    stdout = &serial_stream;
    stdin = &serial_stream;
  }
}