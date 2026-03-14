#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// Pin Configuration (from diagram.json)
// ============================================================
#define PIN_NTC_SENSOR 34    // Analog NTC thermistor output (ADC1_CH6)
#define PIN_DS18B20 18       // DS18B20 1-Wire data pin
#define PIN_LED_GREEN 16     // Green LED - DS18B20 (digital) sensor alert
#define PIN_LED_RED 17       // Red LED - NTC (analog) sensor alert
#define PIN_LCD_SDA 21       // LCD I2C SDA
#define PIN_LCD_SCL 22       // LCD I2C SCL
#define LCD_I2C_ADDR 0x27 // LCD I2C address
#define LCD_COLS 16
#define LCD_ROWS 2

// ============================================================
// ADC Configuration
// ============================================================
#define ADC_RESOLUTION 4095.0f // 12-bit ADC
#define ADC_VREF 3.3f          // Reference voltage

// ============================================================
// NTC Thermistor Parameters
// ============================================================
#define NTC_SERIES_RESISTOR 10000.0f    // Series resistor in ohms
#define NTC_NOMINAL_RESISTANCE 10000.0f // NTC resistance at 25°C
#define NTC_NOMINAL_TEMP 25.0f          // Nominal temperature
#define NTC_BETA 3950.0f                // Beta coefficient
#define KELVIN_OFFSET 273.15f

// ============================================================
// Signal Conditioning Parameters
// ============================================================
#define TEMP_MIN -40.0f // Saturation lower limit
#define TEMP_MAX 125.0f // Saturation upper limit

#define MEDIAN_WINDOW_SIZE 5 // Median filter window size

// Weighted moving average weights (must sum to 1.0)
#define WMA_W1 0.6f // Weight for current sample
#define WMA_W2 0.3f // Weight for previous sample
#define WMA_W3 0.1f // Weight for two samples ago

// ============================================================
// Alert / Hysteresis Parameters
// ============================================================
#define ALERT_THRESHOLD 25.0f                           // Center threshold in °C
#define ALERT_HYSTERESIS 1.0f                           // ±1°C hysteresis
#define ALERT_HIGH (ALERT_THRESHOLD + ALERT_HYSTERESIS) // 26°C
#define ALERT_LOW (ALERT_THRESHOLD - ALERT_HYSTERESIS)  // 24°C

// Alert debounce: condition must persist for this many ms
#define ALERT_DEBOUNCE_MS 200

// ============================================================
// Task Periods (in ms)
// ============================================================
#define TASK_ACQUISITION_PERIOD_MS 250 // Must be > DS18B20 conversion time (~187ms for 10-bit)
#define TASK_CONDITIONING_PERIOD_MS 100
#define TASK_ALERT_PERIOD_MS 50
#define TASK_DISPLAY_PERIOD_MS 500

// ============================================================
// Task Stack Sizes
// ============================================================
#define TASK_ACQUISITION_STACK 4096
#define TASK_CONDITIONING_STACK 4096
#define TASK_ALERT_STACK 4096
#define TASK_DISPLAY_STACK 4096

#endif // CONFIG_H
