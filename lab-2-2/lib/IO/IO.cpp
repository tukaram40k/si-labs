#include "IO.h"
#include <cstdarg>

namespace IO {
  int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    Serial.print(buffer);
    return len;
  }

  void setup() {
    // start serial
    Serial.begin(9600);
    
    // wait for serial port to connect
    while (!Serial) {
      ; // wait
    }
  }
}