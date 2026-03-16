#ifndef LCDCONTROLLER_H
#define LCDCONTROLLER_H

#include <LiquidCrystal_I2C.h>

class LCDController
{
public:
  LCDController(uint8_t addr, uint8_t cols, uint8_t rows);

  void begin(int sdaPin = -1, int sclPin = -1);
  void clear();
  void print(const char *text, uint8_t col, uint8_t row);
  void backlight(bool on);

private:
  LiquidCrystal_I2C _lcd;
  uint8_t _cols;
  uint8_t _rows;
};

#endif // LCDCONTROLLER_H
