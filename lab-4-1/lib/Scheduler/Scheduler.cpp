#include "Scheduler.h"

Scheduler::Scheduler(TaskItem* tasks, size_t taskCount)
  : m_tasks(tasks), m_taskCount(taskCount) {
}

void Scheduler::start(uint32_t nowMs) {
  for (size_t i = 0; i < m_taskCount; i++) {
    m_tasks[i].nextRunAt = nowMs + m_tasks[i].periodMs;
  }
}

void Scheduler::tick(uint32_t nowMs) {
  for (size_t i = 0; i < m_taskCount; i++) {
    TaskItem& t = m_tasks[i];
    // Signed compare handles millis() wraparound.
    if ((int32_t)(nowMs - t.nextRunAt) >= 0) {
      t.fn();
      // Catch-up by stepping forward in fixed increments.
      do {
        t.nextRunAt += t.periodMs;
      } while ((int32_t)(nowMs - t.nextRunAt) >= 0);
    }
  }
}
