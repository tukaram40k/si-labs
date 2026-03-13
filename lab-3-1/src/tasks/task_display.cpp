#include "task_display.h"
#include "../config.h"
#include "../shared_data.h"
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

// LCD is initialized in main.cpp setup() — use it directly
extern LiquidCrystal_I2C g_lcd;

void task_display(void *pvParameters) {
    printf("[DISPLAY] Task started on core %d\n", xPortGetCoreID());

    // Small delay to let other tasks populate data
    vTaskDelay(pdMS_TO_TICKS(500));

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TASK_DISPLAY_PERIOD_MS);

    // Alternate between two display pages
    uint8_t displayPage = 0;
    uint32_t reportCount = 0;

    for (;;) {
        reportCount++;

        // --- Read all shared data (mutex-protected) ---
        sensor_data_t    local_sensor = {0};
        processed_data_t local_processed = {0};
        alert_data_t     local_alert = { ALERT_OFF, ALERT_OFF };

        if (xSemaphoreTake(g_mutex_sensor, pdMS_TO_TICKS(10)) == pdTRUE) {
            local_sensor = g_sensor_data;
            xSemaphoreGive(g_mutex_sensor);
        }

        if (xSemaphoreTake(g_mutex_processed, pdMS_TO_TICKS(10)) == pdTRUE) {
            local_processed = g_processed_data;
            xSemaphoreGive(g_mutex_processed);
        }

        if (xSemaphoreTake(g_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE) {
            local_alert = g_alert_data;
            xSemaphoreGive(g_mutex_alert);
        }

        // --- Print detailed info to Serial (printf/STDIO) ---
        printf("\n========== SENSOR REPORT #%lu ==========\n", reportCount);
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
        printf("=========================================\n");

        // --- Update LCD (16x2 — limited space, alternate pages) ---
        char line0[17];
        char line1[17];

        g_lcd.clear();

        if (displayPage == 0) {
            // Page 0: NTC info
            snprintf(line0, sizeof(line0), "NTC:%.1fC %s",
                     local_processed.filtered_ntc_temp,
                     local_alert.ntc_alert == ALERT_ON ? "ALR" : "OK");
            snprintf(line1, sizeof(line1), "ADC:%u V:%.2f",
                     local_sensor.ntc_adc, local_sensor.ntc_voltage);
        } else {
            // Page 1: DS18B20 info
            snprintf(line0, sizeof(line0), "DS18:%.1fC %s",
                     local_processed.filtered_ds18b20_temp,
                     local_alert.ds18b20_alert == ALERT_ON ? "ALR" : "OK");
            snprintf(line1, sizeof(line1), "NTC:%s DS:%s",
                     local_alert.ntc_alert == ALERT_ON ? "ALR" : "OK",
                     local_alert.ds18b20_alert == ALERT_ON ? "ALR" : "OK");
        }

        g_lcd.setCursor(0, 0);
        g_lcd.print(line0);
        g_lcd.setCursor(0, 1);
        g_lcd.print(line1);

        printf("[DISPLAY] LCD page %d: L0=\"%s\" L1=\"%s\"\n", displayPage, line0, line1);

        // Alternate pages every display cycle
        displayPage = (displayPage + 1) % 2;

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}