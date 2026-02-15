#ifndef LCD_CONTROLLER_H
#define LCD_CONTROLLER_H

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// class to control 16x2 LCD via I2C
class LCDController {
public:
  // make a new LCD Controller with I2C address, columns, and rows
  LCDController(int address, int cols, int rows);

  // setup the lcd
  void setup();

  // clear the lcd
  void clear();

  // print text to the lcd
  void print(const char* text);

  // print text at specific position
  void printAt(int col, int row, const char* text);

private:
  LiquidCrystal_I2C* m_lcd; // lcd object
  int m_address; // i2c address
  int m_cols; // num of columns
  int m_rows; // num of rows
};

#endif