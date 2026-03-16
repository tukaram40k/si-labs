#include "task_acquisition.h"
#include "../config.h"
#include "../shared_data.h"
#include <DS18B20Driver.h>
#include <Arduino.h>

static DS18B20Driver *ds18b20Sensor = nullptr;

void task_acquisition(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);

  ds18b20Sensor = new DS18B20Driver(PIN_DS18B20);
  ds18b20Sensor->begin();
  ds18b20Sensor->readTemperature();
  vTaskDelay(pdMS_TO_TICKS(300));

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(TASK_ACQUISITION_PERIOD_MS);

  for (;;)
  {
    float ds18b20Temp = ds18b20Sensor->readTemperature();
    bool ds18b20Valid = ds18b20Sensor->isValid(ds18b20Temp);

    if (xSemaphoreTake(g_mutex_sensor, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_sensor_data.ds18b20_temp = ds18b20Temp;
      g_sensor_data.ds18b20_valid = ds18b20Valid;
      xSemaphoreGive(g_mutex_sensor);
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}