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

// Global LCD instance — initialized in setup(), used by display task
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C g_lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

void setup() {
    Serial.begin(115200);
    delay(1000);

    printf("\n\n");
    printf("=============================================\n");
    printf("  ESP32 FreeRTOS Temperature Monitor System\n");
    printf("=============================================\n");
    printf("NTC Pin: GPIO%d, DS18B20 Pin: GPIO%d\n", PIN_NTC_SENSOR, PIN_DS18B20);
    printf("Alert threshold: %.1f C (+/- %.1f C hysteresis)\n", ALERT_THRESHOLD, ALERT_HYSTERESIS);
    printf("=============================================\n\n");

    // Initialize I2C and LCD in setup() (runs on Core 1, before tasks)
    printf("[SETUP] Initializing I2C on SDA=%d, SCL=%d...\n", PIN_LCD_SDA, PIN_LCD_SCL);
    Wire.begin(PIN_LCD_SDA, PIN_LCD_SCL);
    delay(100);

    printf("[SETUP] Initializing LCD at address 0x%02X...\n", LCD_I2C_ADDR);
    g_lcd.init();
    delay(100);
    g_lcd.backlight();
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print("System Starting");
    g_lcd.setCursor(0, 1);
    g_lcd.print("Please wait...");
    printf("[SETUP] LCD initialized OK.\n");

    // Initialize shared data structures and mutexes
    shared_data_init();
    printf("[SETUP] Shared data initialized.\n");

    // Create FreeRTOS tasks
    BaseType_t ret;

    ret = xTaskCreatePinnedToCore(
        task_acquisition,
        "Acquisition",
        TASK_ACQUISITION_STACK,
        NULL,
        3,    // High priority — sensor reading
        NULL,
        1     // Core 1
    );
    printf("[SETUP] Acquisition task created: %s\n", ret == pdPASS ? "OK" : "FAIL");

    ret = xTaskCreatePinnedToCore(
        task_conditioning,
        "Conditioning",
        TASK_CONDITIONING_STACK,
        NULL,
        2,    // Medium priority — signal processing
        NULL,
        1     // Core 1
    );
    printf("[SETUP] Conditioning task created: %s\n", ret == pdPASS ? "OK" : "FAIL");

    ret = xTaskCreatePinnedToCore(
        task_alert,
        "Alert",
        TASK_ALERT_STACK,
        NULL,
        2,    // Medium priority — alert detection
        NULL,
        0     // Core 0
    );
    printf("[SETUP] Alert task created: %s\n", ret == pdPASS ? "OK" : "FAIL");

    ret = xTaskCreatePinnedToCore(
        task_display,
        "Display",
        TASK_DISPLAY_STACK,
        NULL,
        1,    // Lower priority — display update
        NULL,
        0     // Core 0
    );
    printf("[SETUP] Display task created: %s\n", ret == pdPASS ? "OK" : "FAIL");

    printf("[SETUP] All tasks created. System running.\n\n");
}

void loop() {
    // FreeRTOS tasks handle everything.
    // Keep loop alive but idle.
    vTaskDelay(pdMS_TO_TICKS(1000));
}
