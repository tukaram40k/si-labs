#include "LCDController.h"
#include <Wire.h>

LCDController::LCDController(uint8_t addr, uint8_t cols, uint8_t rows)
    : _lcd(addr, cols, rows), _cols(cols), _rows(rows) {}

void LCDController::begin(int sdaPin, int sclPin)
{
  // Initialize I2C with explicit SDA/SCL pins if provided
  if (sdaPin >= 0 && sclPin >= 0)
  {
    Wire.begin(sdaPin, sclPin);
  }
  _lcd.init();
  _lcd.backlight();
  _lcd.clear();
}

void LCDController::clear()
{
  _lcd.clear();
}

void LCDController::print(const char *text, uint8_t col, uint8_t row)
{
  _lcd.setCursor(col, row);
  _lcd.print(text);
}

void LCDController::backlight(bool on)
{
  if (on)
  {
    _lcd.backlight();
  }
  else
  {
    _lcd.noBacklight();
  }
}
