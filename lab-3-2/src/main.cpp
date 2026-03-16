#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "config.h"
#include "shared_data.h"
#include "tasks/task_acquisition.h"
#include "tasks/task_conditioning.h"
#include "tasks/task_alert.h"
#include "tasks/task_display.h"
#include "LCDController.h"

LCDController lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

void setup()
{
  Serial.begin(115200);
  delay(1000);

  lcd.begin(PIN_LCD_SDA, PIN_LCD_SCL);

  shared_data_init();

  BaseType_t ret;

  ret = xTaskCreatePinnedToCore(task_acquisition, "Acquisition", TASK_ACQUISITION_STACK, NULL, 3, NULL, 1);
  ret = xTaskCreatePinnedToCore(task_conditioning, "Conditioning", TASK_CONDITIONING_STACK, NULL, 2, NULL, 1);
  ret = xTaskCreatePinnedToCore(task_alert, "Alert", TASK_ALERT_STACK, NULL, 2, NULL, 0);
  ret = xTaskCreatePinnedToCore(task_display, "Display", TASK_DISPLAY_STACK, NULL, 1, NULL, 0);

  for (int i = 0; i < 4; i++)
  {
    xSemaphoreGive(g_start_gate);
  }
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}
