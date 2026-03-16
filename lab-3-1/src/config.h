#ifndef CONFIG_H
#define CONFIG_H

// Pin Configuration
#define PIN_DS18B20 18       // 1-Wire data pin
#define PIN_LED_RED 16       // DS18B20 alert
#define PIN_LCD_SDA 21
#define PIN_LCD_SCL 22
#define LCD_I2C_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Signal Conditioning Parameters
#define TEMP_MIN -40.0f
#define TEMP_MAX 125.0f

// Alert / Hysteresis Parameters
#define ALERT_THRESHOLD 25.0f
#define ALERT_HYSTERESIS 1.0f
#define ALERT_HIGH (ALERT_THRESHOLD + ALERT_HYSTERESIS)
#define ALERT_LOW (ALERT_THRESHOLD - ALERT_HYSTERESIS)
#define ALERT_DEBOUNCE_MS 200

// Task Periods (ms)
#define TASK_ACQUISITION_PERIOD_MS 250
#define TASK_CONDITIONING_PERIOD_MS 100
#define TASK_ALERT_PERIOD_MS 50
#define TASK_DISPLAY_PERIOD_MS 500

// Task Stack Sizes
#define TASK_ACQUISITION_STACK 4096
#define TASK_CONDITIONING_STACK 4096
#define TASK_ALERT_STACK 4096
#define TASK_DISPLAY_STACK 4096

#endif // CONFIG_H
