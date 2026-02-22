#include "../Scheduler/Scheduler.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Global system tick counter
volatile uint32_t g_systemTick = 0;

// Singleton instance
static Scheduler* s_instance = nullptr;

Scheduler::Scheduler() 
    : m_tickCount(0), m_taskCount(0) {
    // Initialize task array
    for (uint8_t i = 0; i < MAX_TASKS; ++i) {
        m_tasks[i].function = nullptr;
        m_tasks[i].period = 0;
        m_tasks[i].offset = 0;
        m_tasks[i].nextRunTime = 0;
        m_tasks[i].enabled = false;
    }
    s_instance = this;
}

Scheduler& Scheduler::getInstance() {
    if (s_instance == nullptr) {
        s_instance = new Scheduler();
    }
    return *s_instance;
}

void Scheduler::begin() {
    // Configure Timer1 for 1ms interrupt
    // Using CTC mode with prescaler of 64
    // For 16MHz clock: 16000000 / 64 = 250000 Hz (4us per tick)
    // For 1ms: 250000 * 0.001 = 250 ticks, so OCR1A = 249
    
    // Stop timer while configuring
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    
    // Set CTC mode (Clear Timer on Compare Match)
    TCCR1B |= (1 << WGM12);
    
    // Set prescaler to 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
    
    // Set compare value for 1ms
    OCR1A = 249;
    
    // Enable Timer1 Compare Match A interrupt
    TIMSK1 |= (1 << OCIE1A);
    
    // Enable global interrupts
    sei();
}

bool Scheduler::addTask(TaskFunction function, uint32_t period, uint32_t offset) {
    if (m_taskCount >= MAX_TASKS || function == nullptr) {
        return false;
    }
    
    // Check if task already exists
    for (uint8_t i = 0; i < m_taskCount; ++i) {
        if (m_tasks[i].function == function) {
            return false; // Task already registered
        }
    }
    
    // Add new task
    m_tasks[m_taskCount].function = function;
    m_tasks[m_taskCount].period = period;
    m_tasks[m_taskCount].offset = offset;
    m_tasks[m_taskCount].nextRunTime = offset;
    m_tasks[m_taskCount].enabled = true;
    m_taskCount++;
    
    return true;
}

bool Scheduler::removeTask(TaskFunction function) {
    if (function == nullptr) {
        return false;
    }
    
    for (uint8_t i = 0; i < m_taskCount; ++i) {
        if (m_tasks[i].function == function) {
            // Shift remaining tasks
            for (uint8_t j = i; j < m_taskCount - 1; ++j) {
                m_tasks[j] = m_tasks[j + 1];
            }
            m_tasks[m_taskCount - 1].function = nullptr;
            m_taskCount--;
            return true;
        }
    }
    return false;
}

void Scheduler::setTaskEnabled(TaskFunction function, bool enabled) {
    for (uint8_t i = 0; i < m_taskCount; ++i) {
        if (m_tasks[i].function == function) {
            m_tasks[i].enabled = enabled;
            if (enabled && m_tasks[i].nextRunTime <= m_tickCount) {
                // Reschedule if enabling and overdue
                m_tasks[i].nextRunTime = m_tickCount + m_tasks[i].period;
            }
            return;
        }
    }
}

void Scheduler::run() {
    // Non-preemptive scheduler: iterate through tasks and execute ready ones
    for (uint8_t i = 0; i < m_taskCount; ++i) {
        if (m_tasks[i].enabled && 
            m_tasks[i].function != nullptr &&
            m_tickCount >= m_tasks[i].nextRunTime) {
            
            // Execute task
            m_tasks[i].function();
            
            // Schedule next run
            m_tasks[i].nextRunTime = m_tickCount + m_tasks[i].period;
        }
    }
}

uint32_t Scheduler::getTickCount() const {
    // Disable interrupts briefly for atomic read
    noInterrupts();
    uint32_t ticks = m_tickCount;
    interrupts();
    return ticks;
}

uint8_t Scheduler::getTaskCount() const {
    return m_taskCount;
}

void Scheduler::incrementTickCount() {
    m_tickCount++;
}

// Timer1 Compare Match A Interrupt Service Routine
ISR(TIMER1_COMPA_vect) {
    g_systemTick++;
    if (s_instance != nullptr) {
        s_instance->incrementTickCount();
    }
}
