#include "IO.h"

static FILE serial_stream;

namespace IO {
  int serial_putchar(char c, FILE *stream) {
    Serial.write(c);
    if (c == '\n') {
      Serial.write('\r');
    }
    return 0;
  }

  int serial_getchar(FILE *stream) {
    while (!Serial.available());
    char c = Serial.read();
    Serial.write(c);
    return c;
  }
  
  void setup() {
    // start serial
    Serial.begin(9600);
  
    // redirect std to serial
    fdev_setup_stream(&serial_stream, serial_putchar, serial_getchar, _FDEV_SETUP_RW);
    stdout = &serial_stream;
    stdin = &serial_stream;
  }
}