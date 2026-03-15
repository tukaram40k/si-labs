#include "task_display.h"
#include "../config.h"
#include "../shared_data.h"
#include "LCDController.h"
#include <Arduino.h>

extern LCDController lcd;

void task_display(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(500));

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(TASK_DISPLAY_PERIOD_MS);

  for (;;)
  {
    sensor_data_t local_sensor = {0};
    processed_data_t local_processed = {0};
    alert_data_t local_alert = {ALERT_OFF, ALERT_OFF};

    if (xSemaphoreTake(g_mutex_sensor, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      local_sensor = g_sensor_data;
      xSemaphoreGive(g_mutex_sensor);
    }

    if (xSemaphoreTake(g_mutex_processed, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      local_processed = g_processed_data;
      xSemaphoreGive(g_mutex_processed);
    }

    if (xSemaphoreTake(g_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      local_alert = g_alert_data;
      xSemaphoreGive(g_mutex_alert);
    }
    printf("\n========== SENSOR REPORT ==========\n");
    printf("NTC Sensor:\n");
    printf("  Raw ADC: %u\n", local_sensor.ntc_adc);
    printf("  Voltage: %.3f V\n", local_sensor.ntc_voltage);
    printf("  Raw Temp: %.2f C\n", local_sensor.ntc_temp);
    printf("  Filtered Temp: %.2f C\n", local_processed.filtered_ntc_temp);
    printf("DS18B20 Sensor:\n");
    printf("  Raw Temp: %.2f C\n", local_sensor.ds18b20_temp);
    printf("  Filtered Temp: %.2f C\n", local_processed.filtered_ds18b20_temp);
    printf("  Valid: %s\n", local_sensor.ds18b20_valid ? "YES" : "NO");
    printf("Alerts:\n");
    printf("  NTC:     %s (filtered=%.2f, high=%.1f, low=%.1f)\n",
           local_alert.ntc_alert == ALERT_ON ? "ALERT" : "OK",
           local_processed.filtered_ntc_temp, (float)ALERT_HIGH, (float)ALERT_LOW);
    printf("  DS18B20: %s (filtered=%.2f, high=%.1f, low=%.1f)\n",
           local_alert.ds18b20_alert == ALERT_ON ? "ALERT" : "OK",
           local_processed.filtered_ds18b20_temp, (float)ALERT_HIGH, (float)ALERT_LOW);
    printf("=====================================\n");

    char line0[17];
    char line1[17];

    snprintf(line0, sizeof(line0), "NTC:%.1fC %s",
             local_processed.filtered_ntc_temp,
             local_alert.ntc_alert == ALERT_ON ? "ALR" : "OK");
    snprintf(line1, sizeof(line1), "DS:%.1fC  %s",
             local_processed.filtered_ds18b20_temp,
             local_alert.ds18b20_alert == ALERT_ON ? "ALR" : "OK");

    lcd.clear();
    lcd.print(line0, 0, 0);
    lcd.print(line1, 0, 1);

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}