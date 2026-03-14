#include "task_acquisition.h"
#include "../config.h"
#include "../shared_data.h"
#include <NTCDriver.h>
#include <DS18B20Driver.h>
#include <Arduino.h>

static NTCDriver *ntcSensor = nullptr;
static DS18B20Driver *ds18b20Sensor = nullptr;

void task_acquisition(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);
  printf("[ACQ] Task started on core %d\n", xPortGetCoreID());

  printf("[ACQ] Initializing NTC sensor on GPIO%d...\n", PIN_NTC_SENSOR);
  ntcSensor = new NTCDriver(PIN_NTC_SENSOR, NTC_SERIES_RESISTOR,
                            NTC_NOMINAL_RESISTANCE, NTC_NOMINAL_TEMP,
                            NTC_BETA, ADC_RESOLUTION, ADC_VREF);
  ntcSensor->begin();
  printf("[ACQ] NTC sensor initialized.\n");

  printf("[ACQ] Initializing DS18B20 sensor on GPIO%d...\n", PIN_DS18B20);
  ds18b20Sensor = new DS18B20Driver(PIN_DS18B20);
  ds18b20Sensor->begin();
  printf("[ACQ] DS18B20 sensor initialized.\n");

  printf("[ACQ] Requesting initial DS18B20 conversion...\n");
  ds18b20Sensor->readTemperature();
  vTaskDelay(pdMS_TO_TICKS(300));
  printf("[ACQ] Initial conversion done. Starting acquisition loop.\n");

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(TASK_ACQUISITION_PERIOD_MS);

  for (;;)
  {
    uint16_t adcRaw = 0;
    float voltage = 0.0f;
    float ntcTemp = ntcSensor->readTemperature(&adcRaw, &voltage);

    float ds18b20Temp = ds18b20Sensor->readTemperature();
    bool ds18b20Valid = ds18b20Sensor->isValid(ds18b20Temp);

    if (xSemaphoreTake(g_mutex_sensor, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_sensor_data.ntc_adc = adcRaw;
      g_sensor_data.ntc_voltage = voltage;
      g_sensor_data.ntc_temp = ntcTemp;
      g_sensor_data.ds18b20_temp = ds18b20Temp;
      g_sensor_data.ds18b20_valid = ds18b20Valid;
      xSemaphoreGive(g_mutex_sensor);
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}