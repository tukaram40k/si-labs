#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/IO/IO.h"
#include "../lib/LedDriver/LedController.h"

// create led controllers
LedController led1(12);
LedController led2(11);
LedController led3(10);

void setup() {
  // init led controllers
  led1.setup();
  led2.setup();
  led3.setup();

  // redirect stdio to serial
  IO::setup();
  printf("Ready\n");
}

void loop() {
  
}