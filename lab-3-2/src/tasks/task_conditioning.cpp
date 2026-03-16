#include "task_conditioning.h"
#include "../config.h"
#include "../shared_data.h"
#include "../services/signal_conditioning.h"

static conditioning_pipeline_t ntc_pipeline;

void task_conditioning(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);

  pipeline_init(&ntc_pipeline);

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

    conditioning_result_t ntc_result = pipeline_apply_with_intermediate(&ntc_pipeline, local_sensor.ntc_temp);

    if (xSemaphoreTake(g_mutex_processed, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_processed_data.saturated_ntc_temp = ntc_result.saturated;
      g_processed_data.median_ntc_temp = ntc_result.median;
      g_processed_data.filtered_ntc_temp = ntc_result.wma;
      xSemaphoreGive(g_mutex_processed);
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}