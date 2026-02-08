#ifndef IO_H
#define IO_H

#include <Arduino.h>
#include <stdio.h>

namespace IO {
  int serial_putchar(char c, FILE *stream);
  int serial_getchar(FILE *stream);
  void setup();
}

#endif // IO_H