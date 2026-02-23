#include "../Scheduler/Scheduler.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t g_systemTick = 0;  // global system tick

static Scheduler *s_instance = nullptr;

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

void Scheduler::begin()
{
  // configure Timer1 for 1ms interrupt (CTC mode, prescaler 64)
  // 16MHz / 64 = 250kHz (4us), 1ms = 250 ticks, OCR1A = 249

  // stop timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // CTC mode
  TCCR1B |= (1 << WGM12);

  // prescaler 64
  TCCR1B |= (1 << CS11) | (1 << CS10);

  // 1ms compare
  OCR1A = 249;

  // enable compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  // enable global interrupts
  sei();
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

uint32_t Scheduler::getTickCount() const
{
  // atomic read
  noInterrupts();
  uint32_t ticks = m_tickCount;
  interrupts();
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

// Timer1 compare match ISR
ISR(TIMER1_COMPA_vect)
{
  g_systemTick++;
  if (s_instance != nullptr)
  {
    s_instance->incrementTickCount();
  }
}
