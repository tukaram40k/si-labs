#include "task_conditioning.h"
#include "../config.h"
#include "../shared_data.h"
#include "../services/signal_conditioning.h"

static conditioning_pipeline_t ntc_pipeline;
static conditioning_pipeline_t ds18b20_pipeline;

void task_conditioning(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);
  printf("[COND] Task started on core %d\n", xPortGetCoreID());

  pipeline_init(&ntc_pipeline);
  pipeline_init(&ds18b20_pipeline);
  printf("[COND] Pipelines initialized.\n");

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

    float filtered_ntc = pipeline_apply(&ntc_pipeline, local_sensor.ntc_temp);

    float filtered_ds = local_sensor.ds18b20_temp;
    if (local_sensor.ds18b20_valid)
    {
      filtered_ds = pipeline_apply(&ds18b20_pipeline, local_sensor.ds18b20_temp);
    }

    if (xSemaphoreTake(g_mutex_processed, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_processed_data.filtered_ntc_temp = filtered_ntc;
      g_processed_data.filtered_ds18b20_temp = filtered_ds;
      xSemaphoreGive(g_mutex_processed);
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}