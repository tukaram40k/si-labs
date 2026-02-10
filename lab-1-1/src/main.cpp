#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/LedDriver/LedController.h"
#include "../lib/IO/IO.h"

// create a led controller for pin 7
LedController led(7);

void setup() {
  // init the led controller
  led.setup();

  // redirect stdio to serial
  IO::setup();
  printf("Ready\n");
}

void loop() {
  char input[20];
  
  // check for data on serial port
  if (Serial.available() > 0) {
    scanf("%19s", input);

    // compare the command
    if (strcmp(input, "LED") == 0 or strcmp(input, "led") == 0) {
      char input2[10];
      scanf("%9s", input2);
      
      if (strcmp(input2, "ON") == 0 or strcmp(input2, "on") == 0) {
        led.turnOn();
        printf("LED is ON\n");
      } else if (strcmp(input2, "OFF") == 0 or strcmp(input2, "off") == 0) {
        led.turnOff();
        printf("LED is OFF\n");
      } else {
        printf("Unknown command\n");
      }
    } else {
      printf("Unknown command\n");
    }
  }
}