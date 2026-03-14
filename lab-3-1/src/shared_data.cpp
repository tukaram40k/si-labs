#include "shared_data.h"
#include <string.h>

// Global shared data instances
sensor_data_t g_sensor_data = {0};
processed_data_t g_processed_data = {0};
alert_data_t g_alert_data = {ALERT_OFF, ALERT_OFF};

// Mutexes for protecting shared data
SemaphoreHandle_t g_mutex_sensor = NULL;
SemaphoreHandle_t g_mutex_processed = NULL;
SemaphoreHandle_t g_mutex_alert = NULL;

// Start gate semaphore — created "empty", tasks block until setup() gives it
SemaphoreHandle_t g_start_gate = NULL;

void shared_data_init(void)
{
  memset(&g_sensor_data, 0, sizeof(g_sensor_data));
  memset(&g_processed_data, 0, sizeof(g_processed_data));
  g_alert_data.ntc_alert = ALERT_OFF;
  g_alert_data.ds18b20_alert = ALERT_OFF;

  g_mutex_sensor = xSemaphoreCreateMutex();
  g_mutex_processed = xSemaphoreCreateMutex();
  g_mutex_alert = xSemaphoreCreateMutex();

  // Create counting semaphore: max=4 (one per task), initial=0 (all blocked)
  g_start_gate = xSemaphoreCreateCounting(4, 0);
}