#ifndef IO_H
#define IO_H

#include <Arduino.h>
#include <stdio.h>

namespace IO {
  // helper for std output
  int serial_putchar(char c, FILE *stream);

  // helper for std input
  int serial_getchar(FILE *stream);

  // setup stream redirection
  void setup();
}

#endif