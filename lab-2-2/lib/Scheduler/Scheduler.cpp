#include "../Scheduler/Scheduler.h"
#include <Arduino.h>
#include "esp_timer.h"

volatile uint32_t g_systemTick = 0;  // global system tick

static Scheduler *s_instance = nullptr;
static esp_timer_handle_t s_timerHandle;

Scheduler::Scheduler()
    : m_tickCount(0), m_taskCount(0)
{
  // init task array
  for (uint8_t i = 0; i < MAX_TASKS; ++i)
  {
    m_tasks[i].function = nullptr;
    m_tasks[i].period = 0;
    m_tasks[i].offset = 0;
    m_tasks[i].nextRunTime = 0;
    m_tasks[i].enabled = false;
  }
  // initialize spinlock
  m_spinlock.owner = portMUX_FREE_VAL;
  m_spinlock.count = 0;
  s_instance = this;
}

Scheduler &Scheduler::getInstance()
{
  if (s_instance == nullptr)
  {
    s_instance = new Scheduler();
  }
  return *s_instance;
}

static void IRAM_ATTR timerCallback(void *arg)
{
  g_systemTick++;
  if (s_instance != nullptr)
  {
    s_instance->incrementTickCount();
  }
}

void Scheduler::begin()
{
  // configure ESP32 hardware timer for 1ms period
  esp_timer_create_args_t timerArgs = {
    .callback = &timerCallback,
    .arg = nullptr,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "scheduler_timer",
    .skip_unhandled_events = false
  };
  
  esp_timer_create(&timerArgs, &s_timerHandle);
  esp_timer_start_periodic(s_timerHandle, 1000); // 1000 us = 1ms
}

bool Scheduler::addTask(TaskFunction function, uint32_t period, uint32_t offset)
{
  if (m_taskCount >= MAX_TASKS || function == nullptr)
  {
    return false;
  }

  // check if already registered
  for (uint8_t i = 0; i < m_taskCount; ++i)
  {
    if (m_tasks[i].function == function)
    {
      return false;
    }
  }

  // add task
  m_tasks[m_taskCount].function = function;
  m_tasks[m_taskCount].period = period;
  m_tasks[m_taskCount].offset = offset;
  m_tasks[m_taskCount].nextRunTime = offset;
  m_tasks[m_taskCount].enabled = true;
  m_taskCount++;

  return true;
}

bool Scheduler::removeTask(TaskFunction function)
{
  if (function == nullptr)
  {
    return false;
  }

  for (uint8_t i = 0; i < m_taskCount; ++i)
  {
    if (m_tasks[i].function == function)
    {
      // shift remaining tasks
      for (uint8_t j = i; j < m_taskCount - 1; ++j)
      {
        m_tasks[j] = m_tasks[j + 1];
      }
      m_tasks[m_taskCount - 1].function = nullptr;
      m_taskCount--;
      return true;
    }
  }
  return false;
}

void Scheduler::setTaskEnabled(TaskFunction function, bool enabled)
{
  for (uint8_t i = 0; i < m_taskCount; ++i)
  {
    if (m_tasks[i].function == function)
    {
      m_tasks[i].enabled = enabled;
      if (enabled && m_tasks[i].nextRunTime <= m_tickCount)
      {
        // reschedule if enabling and overdue
        m_tasks[i].nextRunTime = m_tickCount + m_tasks[i].period;
      }
      return;
    }
  }
}

void Scheduler::run()
{
  // iterate and execute ready tasks
  for (uint8_t i = 0; i < m_taskCount; ++i)
  {
    if (m_tasks[i].enabled &&
        m_tasks[i].function != nullptr &&
        m_tickCount >= m_tasks[i].nextRunTime)
    {

      // execute task
      m_tasks[i].function();

      // schedule next run
      m_tasks[i].nextRunTime = m_tickCount + m_tasks[i].period;
    }
  }
}

uint32_t Scheduler::getTickCount()
{
  // atomic read using critical section
  portENTER_CRITICAL(&m_spinlock);
  uint32_t ticks = m_tickCount;
  portEXIT_CRITICAL(&m_spinlock);
  return ticks;
}

uint8_t Scheduler::getTaskCount() const
{
  return m_taskCount;
}

void Scheduler::incrementTickCount()
{
  m_tickCount++;
}
