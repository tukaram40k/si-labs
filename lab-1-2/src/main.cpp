#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/IO/IO.h"
#include "../lib/LedDriver/LedController.h"
#include "../lib/LCDDriver/LCDController.h"

// create led controllers
LedController green_led(13);
LedController red_led(12);

// create an lcd controller (I2C address 0x27, 16 columns, 2 rows)
LCDController lcd(0x27, 16, 2);

void setup() {
  // init led controllers
  green_led.setup();
  red_led.setup();

  // init the lcd
  lcd.setup();
  lcd.clear();
  lcd.print("Ready");

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
        green_led.turnOn();
        red_led.turnOff();
        printf("LED is ON\n");
        lcd.clear();
        lcd.print("LED is ON");
      } else if (strcmp(input2, "OFF") == 0 or strcmp(input2, "off") == 0) {
        green_led.turnOff();
        red_led.turnOff();
        printf("LED is OFF\n");
        lcd.clear();
        lcd.print("LED is OFF");
      } else {
        red_led.turnOn();
        printf("Unknown command\n");
        lcd.clear();
        lcd.print("Unknown cmd");
      }
    } else if (strcmp(input, "3344") == 0) {
      green_led.turnOn();
      printf("Correct code\n");
      lcd.clear();
      lcd.print("correct code");
    } else {
      red_led.turnOn();
      green_led.turnOff();
      red_led.turnOn();
      printf("Unknown command\n");
      lcd.clear();
      lcd.print("Unknown cmd");
    }
  }
}