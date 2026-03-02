#ifndef IO_H
#define IO_H

#include <Arduino.h>
#include <stdio.h>

namespace IO {
  // printf wrapper that outputs to Serial
  int printf(const char *format, ...);
  
  // setup stream redirection
  void setup();
}

#endif