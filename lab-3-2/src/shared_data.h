#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

typedef enum
{
  ALERT_OFF = 0,
  ALERT_ON = 1
} alert_state_t;

typedef struct
{
  float ntc_temp;
  uint16_t ntc_adc;
  float ntc_voltage;
} sensor_data_t;

typedef struct
{
  float saturated_ntc_temp;
  float median_ntc_temp;
  float filtered_ntc_temp;
} processed_data_t;

typedef struct
{
  alert_state_t ntc_alert;
} alert_data_t;

extern sensor_data_t g_sensor_data;
extern processed_data_t g_processed_data;
extern alert_data_t g_alert_data;

extern SemaphoreHandle_t g_mutex_sensor;
extern SemaphoreHandle_t g_mutex_processed;
extern SemaphoreHandle_t g_mutex_alert;
extern SemaphoreHandle_t g_start_gate;

void shared_data_init(void);

#endif // SHARED_DATA_H