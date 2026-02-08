#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/LedDriver/LedController.h"
#include "../lib/IO/IO.h"

// Create an LED controller instance for pin 7
LedController led(7);

void setup() {
  // Initialize the LED controller
  led.setup();

  // Redirect stdout/stdin to Serial using IO library
  IO::setup();

  // Print startup message using printf
  printf("Ready\n");
}

void loop() {
  char input[20]; // Buffer to store the command
  
  // Check if there's incoming data on the serial port
  if (Serial.available() > 0) {
    // Read the incoming string using scanf
    scanf("%19s", input); // Read up to 19 characters to prevent buffer overflow

    // Check the command and control the LED accordingly
    if (strcmp(input, "LED") == 0) {
      // Read the next word to get ON or OFF
      char input2[10];
      scanf("%9s", input2); // Read the next word (ON or OFF)
      
      if (strcmp(input2, "ON") == 0) {
        led.turnOn();  // Turn LED on
        printf("LED is ON\n");
      } else if (strcmp(input2, "OFF") == 0) {
        led.turnOff(); // Turn LED off
        printf("LED is OFF\n");
      } else {
        printf("Unknown command\n");
      }
    } else {
      printf("Unknown command\n");
    }
  }
}