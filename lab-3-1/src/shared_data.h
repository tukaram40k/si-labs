#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// ============================================================
// Alert State Enum
// ============================================================
typedef enum {
    ALERT_OFF = 0,
    ALERT_ON  = 1
} alert_state_t;

// ============================================================
// Raw Sensor Data (written by Acquisition Task)
// ============================================================
typedef struct {
    float    ntc_temp;       // Converted NTC temperature (°C)
    uint16_t ntc_adc;        // Raw ADC value
    float    ntc_voltage;    // ADC voltage
    float    ds18b20_temp;   // DS18B20 temperature (°C)
    bool     ds18b20_valid;  // Whether DS18B20 reading is valid
} sensor_data_t;

// ============================================================
// Processed / Filtered Data (written by Conditioning Task)
// ============================================================
typedef struct {
    float filtered_ntc_temp;
    float filtered_ds18b20_temp;
} processed_data_t;

// ============================================================
// Alert Data (written by Alert Task)
// ============================================================
typedef struct {
    alert_state_t ntc_alert;
    alert_state_t ds18b20_alert;
} alert_data_t;

// ============================================================
// Global shared instances (declared in shared_data.cpp)
// ============================================================
extern sensor_data_t    g_sensor_data;
extern processed_data_t g_processed_data;
extern alert_data_t     g_alert_data;

extern SemaphoreHandle_t g_mutex_sensor;
extern SemaphoreHandle_t g_mutex_processed;
extern SemaphoreHandle_t g_mutex_alert;

// Start gate: all tasks wait on this before running their main loop
extern SemaphoreHandle_t g_start_gate;

// Initialize shared data and mutexes
void shared_data_init(void);

#endif // SHARED_DATA_H