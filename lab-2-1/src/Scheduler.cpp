#include "Scheduler.h"

// Static member initialization
TaskControlBlock Scheduler::tasks[MAX_TASKS];
int Scheduler::taskCount = 0;
volatile uint32_t Scheduler::systemTick = 0;

// Timer1 Compare Match A interrupt vector
ISR(TIMER1_COMPA_vect) {
    Scheduler::tick();
}

void Scheduler::tick() {
    systemTick++;
}

void Scheduler::init() {
    // Initialize task array
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].function = nullptr;
        tasks[i].period = 0;
        tasks[i].offset = 0;
        tasks[i].nextRunTime = 0;
        tasks[i].enabled = false;
    }
    taskCount = 0;
    systemTick = 0;
}

void Scheduler::start() {
    setupTimer1();
}

void Scheduler::stop() {
    // Disable Timer1 Compare Match A interrupt
    TIMSK1 &= ~(1 << OCIE1A);
    // Stop timer
    TCCR1B = 0;
}

int Scheduler::addTask(TaskFunction function, uint32_t period, uint32_t offset) {
    if (taskCount >= MAX_TASKS || function == nullptr) {
        return -1;
    }

    int index = taskCount;
    tasks[index].function = function;
    tasks[index].period = period;
    tasks[index].offset = offset;
    tasks[index].nextRunTime = systemTick + offset;
    tasks[index].enabled = true;
    taskCount++;

    return index;
}

void Scheduler::removeTask(int taskIndex) {
    if (taskIndex < 0 || taskIndex >= taskCount) {
        return;
    }

    // Shift remaining tasks
    for (int i = taskIndex; i < taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];
    }

    // Clear last task
    tasks[taskCount - 1].function = nullptr;
    tasks[taskCount - 1].period = 0;
    tasks[taskCount - 1].offset = 0;
    tasks[taskCount - 1].nextRunTime = 0;
    tasks[taskCount - 1].enabled = false;

    taskCount--;
}

void Scheduler::enableTask(int taskIndex, bool enabled) {
    if (taskIndex < 0 || taskIndex >= taskCount) {
        return;
    }
    tasks[taskIndex].enabled = enabled;
}

void Scheduler::run() {
    uint32_t currentTime = systemTick;

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].enabled && tasks[i].function != nullptr) {
            // Check if task is due (using signed comparison for wrap-around safety)
            if ((int32_t)(currentTime - tasks[i].nextRunTime) >= 0) {
                // Execute the task
                tasks[i].function();

                // Schedule next run
                tasks[i].nextRunTime = currentTime + tasks[i].period;
            }
        }
    }
}

uint32_t Scheduler::getTickCount() {
    // Disable interrupts briefly for atomic read
    noInterrupts();
    uint32_t tick = systemTick;
    interrupts();
    return tick;
}

int Scheduler::getTaskCount() {
    return taskCount;
}

void Scheduler::setupTimer1() {
    // Configure Timer1 for 1ms interrupt at 16MHz
    // Using CTC mode with prescaler of 64
    // OCR1A = (16000000 / 64) * 0.001 - 1 = 249

    // Stop timer first
    TCCR1B = 0;
    TCCR1A = 0;

    // Set CTC mode (Clear Timer on Compare Match)
    TCCR1B |= (1 << WGM12);

    // Set prescaler to 64
    TCCR1B |= (1 << CS11) | (1 << CS10);

    // Set compare value for 1ms interrupt
    OCR1A = 249;

    // Enable Timer1 Compare Match A interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Enable global interrupts
    sei();
}
