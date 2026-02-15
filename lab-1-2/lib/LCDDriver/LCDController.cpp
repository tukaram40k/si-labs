#include "LCDController.h"

LCDController::LCDController(int address, int cols, int rows) : m_address(address), m_cols(cols), m_rows(rows) {
  m_lcd = new LiquidCrystal_I2C(m_address, m_cols, m_rows);
}

void LCDController::setup() {
  m_lcd->init();
  m_lcd->backlight();
}

void LCDController::clear() {
  m_lcd->clear();
}

void LCDController::print(const char* text) {
  m_lcd->print(text);
}

void LCDController::printAt(int col, int row, const char* text) {
  m_lcd->setCursor(col, row);
  m_lcd->print(text);
}