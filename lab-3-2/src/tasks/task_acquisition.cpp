#include "task_acquisition.h"
#include "../config.h"
#include "../shared_data.h"
#include <NTCDriver.h>
#include <Arduino.h>

static NTCDriver *ntcSensor = nullptr;

void task_acquisition(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);

  ntcSensor = new NTCDriver(PIN_NTC_SENSOR, NTC_SERIES_RESISTOR,
                            NTC_NOMINAL_RESISTANCE, NTC_NOMINAL_TEMP,
                            NTC_BETA, ADC_RESOLUTION, ADC_VREF);
  ntcSensor->begin();

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(TASK_ACQUISITION_PERIOD_MS);

  for (;;)
  {
    uint16_t adcRaw = 0;
    float voltage = 0.0f;
    float ntcTemp = ntcSensor->readTemperature(&adcRaw, &voltage);

    if (xSemaphoreTake(g_mutex_sensor, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_sensor_data.ntc_adc = adcRaw;
      g_sensor_data.ntc_voltage = voltage;
      g_sensor_data.ntc_temp = ntcTemp;
      xSemaphoreGive(g_mutex_sensor);
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}