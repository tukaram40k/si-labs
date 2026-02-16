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
String secretCode = "3344";

bool programmingMode = false;
bool accessGranted = false;

void setup() {
  // init led controllers
  green_led.setup();
  red_led.setup();

  // init the lcd
  lcd.setup();
  lcd.clear();

  // init the keypad
  keypad.setup();

  // redirect stdio to lcd and keypad
  IO::setup(&lcd, &keypad);
  printf("Enter Code:\n");
}

void loop() {
  // check for key presses using stdio
  int key = getchar();

  if (key != EOF) {
    char ch = (char)key;
    
    if (ch == '#') {
      // allow changing the secret code if access is granted
      if (accessGranted) {
        programmingMode = true;
        codeBuffer = "";
        lcd.clear();
        printf("Prog Mode:\n");
      } else {
        lcd.clear();
        printf("Enter code first.\n");
        delay(2000);
        
        lcd.clear();
        printf("%s\n", codeBuffer.c_str());
      }
    } 
    else if (ch == '*') {
      codeBuffer = "";
      lcd.clear();
      printf("Code cleared\n");
      delay(500);

      lcd.clear();
      printf("Enter Code:\n");
    }
    else {
      codeBuffer += ch;

      // update lcd
      lcd.clear();
      
      if (programmingMode) {
        printf("Prog: %s\n", codeBuffer.c_str());
      } else {
        printf("%s\n", codeBuffer.c_str());
      }

      // check if in programming mode
      if (programmingMode) {
        if (codeBuffer.length() >= 4) {
          // set the new secret code
          secretCode = codeBuffer.substring(0, 4);
          programmingMode = false;
          codeBuffer = "";
          
          lcd.clear();
          printf("New code set!\n");
          delay(1000);
          
          lcd.clear();
          printf("Enter Code:\n");
        }
      } 
      else {
        // match the code
        if (codeBuffer == secretCode) {
          lcd.clear();
          printf("Correct code!\n");
          green_led.turnOn();
          red_led.turnOff();
          accessGranted = true;

          delay(2000);

          codeBuffer = "";

          lcd.clear();
          printf("Enter code:\n");
        }
        // check if 4 chars reached
        else if (codeBuffer.length() >= 4) {
          lcd.clear();
          printf("Incorrect!\n");
          red_led.turnOn();
          green_led.turnOff();
          accessGranted = false;

          delay(2000);

          codeBuffer = "";

          lcd.clear();
          printf("Enter code:\n");
        }
      }
    }
  }
}