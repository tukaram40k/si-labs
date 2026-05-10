#pragma once

#include <Arduino.h>

namespace Config
{
  constexpr uint8_t LED_PIN = 8;
  constexpr uint8_t BUTTON_PIN = 9;
  constexpr bool BUTTON_ACTIVE_LOW = true;
  constexpr uint16_t BUTTON_DEBOUNCE_MS = 30;
}
