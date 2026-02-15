#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/IO/IO.h"
#include "../lib/LedDriver/LedController.h"
#include "../lib/LCDDriver/LCDController.h"
#include "../lib/KeypadDriver/KeypadController.h"

// create led controllers
LedController green_led(13);
LedController red_led(12);

// create an lcd controller (I2C address 0x27, 16 columns, 2 rows)
LCDController lcd(0x27, 16, 2);

// create a keypad controller (pins 7 - 0)
KeypadController keypad(7, 6, 5, 4, 3, 2, 1, 0);

// current code being entered
String codeBuffer = "";

void setup() {
  // init led controllers
  green_led.setup();
  red_led.setup();

  // init the lcd
  lcd.setup();
  lcd.clear();
  lcd.print("Enter Code:");

  // init the keypad
  keypad.setup();

  // redirect stdio to serial
  IO::setup();
  printf("Ready\n");
}

void loop() {
  // check for key presses
  char key = keypad.getKey();
  if (key != 0) {
    if (key != '*') {
      codeBuffer += key;
      
      // update lcd
      lcd.clear();
      lcd.print(codeBuffer.c_str());
      
      // match the code
      if (codeBuffer == "3344") {
        lcd.clear();
        lcd.print("Correct code!");
        green_led.turnOn();
        red_led.turnOff();
        printf("Correct code!\n");
        delay(2000);
        
        codeBuffer = "";
        
        lcd.clear();
        lcd.print("Enter code:");
      }
      // check if 4 chars reached
      else if (codeBuffer.length() >= 4) {
        lcd.clear();
        lcd.print("Incorrect!");
        red_led.turnOn();
        green_led.turnOff();
        printf("Incorrect!\n");
        delay(2000);
        
        codeBuffer = "";
        
        lcd.clear();
        lcd.print("Enter code:");
      }
    } 
    else {
      codeBuffer = "";
      lcd.clear();
      lcd.print("Code cleared");
      printf("Code cleared\n");
      delay(500);
      
      lcd.clear();
      lcd.print("Enter Code:");
    }
  }
}