#include "task_conditioning.h"
#include "../config.h"
#include "../shared_data.h"
#include "../services/signal_conditioning.h"

void task_conditioning(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(TASK_CONDITIONING_PERIOD_MS);

  for (;;)
  {
    sensor_data_t local_sensor;
    if (xSemaphoreTake(g_mutex_sensor, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      local_sensor = g_sensor_data;
      xSemaphoreGive(g_mutex_sensor);
    }
    else
    {
      vTaskDelayUntil(&xLastWakeTime, xPeriod);
      continue;
    }

    float ds_result;
    if (local_sensor.ds18b20_valid)
    {
      ds_result = saturate(local_sensor.ds18b20_temp, TEMP_MIN, TEMP_MAX);
    }
    else
    {
      ds_result = 0.0f;
    }

    if (xSemaphoreTake(g_mutex_processed, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_processed_data.saturated_ds18b20_temp = ds_result;
      xSemaphoreGive(g_mutex_processed);
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}