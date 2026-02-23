#include "Tasks.h"
#include "../Scheduler/Scheduler.h"
#include <Arduino.h>
#include <stdio.h>

volatile bool g_buttonPressed = false;
volatile uint16_t g_pushCount = 0;
volatile bool g_blinkSequenceActive = false;
volatile uint8_t g_blinkState = 0;
volatile uint8_t g_blinkCounter = 0;

static uint8_t s_debounceCounter = 0;
static bool s_lastButtonState = false;
static bool s_buttonDetected = false;

static uint8_t s_blinkIndex = 0;
static uint32_t s_lastBlinkTime = 0;

static bool s_statusPending = false;

// task 1: button detector (10ms period, 0ms offset)
// debounces button, toggles led1, sets flag for task 2
void taskButtonDetector(void)
{
  // read button state (active low)
  bool currentButtonState = (digitalRead(PIN_BUTTON) == LOW);

  // debounce: count consecutive identical readings
  if (currentButtonState != s_lastButtonState)
  {
    // state changed, reset counter
    s_debounceCounter = 0;
    s_lastButtonState = currentButtonState;
  }
  else
  {
    // state stable, increment counter
    if (s_debounceCounter < DEBOUNCE_STABLE_COUNT)
    {
      s_debounceCounter++;
    }

    // check for stable press
    if (s_debounceCounter == DEBOUNCE_STABLE_COUNT && currentButtonState && !s_buttonDetected)
    {
      // valid press detected
      s_buttonDetected = true;

      // toggle led1
      digitalWrite(PIN_LED1, !digitalRead(PIN_LED1));

      // signal task 2
      g_buttonPressed = true;
    }
  }

  // reset flag on release
  if (!currentButtonState && s_buttonDetected)
  {
    s_buttonDetected = false;
  }
}

// task 2: press counter & visualizer (50ms period, 5ms offset)
// counts presses, triggers led2 blink sequence
void taskPressCounter(void)
{
  // get current time
  uint32_t currentTime = Scheduler::getInstance().getTickCount();

  // check for button press from task 1
  if (g_buttonPressed)
  {
    // consume flag
    g_buttonPressed = false;

    // increment counter
    g_pushCount++;

    // start blink sequence
    g_blinkSequenceActive = true;
    s_blinkIndex = 0;
    s_lastBlinkTime = currentTime;

    // turn on led2
    digitalWrite(PIN_LED2, HIGH);
    g_blinkState = HIGH;
  }

  // handle blink sequence
  if (g_blinkSequenceActive)
  {
    // check if period elapsed
    if (currentTime - s_lastBlinkTime >= BLINK_PERIOD_MS)
    {
      s_lastBlinkTime = currentTime;
      s_blinkIndex++;

      // check if sequence complete
      if (s_blinkIndex >= BLINK_COUNT)
      {
        // sequence done
        g_blinkSequenceActive = false;
        s_blinkIndex = 0;

        // ensure led2 off
        digitalWrite(PIN_LED2, LOW);
        g_blinkState = LOW;
      }
      else
      {
        // toggle led2
        g_blinkState = !g_blinkState;
        digitalWrite(PIN_LED2, g_blinkState);
      }
    }
  }
}

// task 3: status monitor (10s period, 1s offset)
// logs push count, resets counter, toggles led3
void taskStatusMonitor(void)
{
  // read count atomically
  noInterrupts();
  uint16_t currentCount = g_pushCount;
  interrupts();

  // transmit via uart
  printf("Total presses: %u\n", currentCount);

  // reset counter
  g_pushCount = 0;

  // toggle led3 heartbeat
  digitalWrite(PIN_LED3, !digitalRead(PIN_LED3));
}

// initialization
void tasksInit(void)
{
  // configure led pins
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);

  // configure button with pull-up
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // initialize leds off
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);
  digitalWrite(PIN_LED3, LOW);

  // initialize shared state
  g_buttonPressed = false;
  g_pushCount = 0;
  g_blinkSequenceActive = false;
  g_blinkState = 0;
  g_blinkCounter = 0;

  // initialize task state machines
  s_debounceCounter = 0;
  s_lastButtonState = false;
  s_buttonDetected = false;
  s_blinkIndex = 0;
  s_lastBlinkTime = 0;
  s_statusPending = false;

  // register tasks
  Scheduler &scheduler = Scheduler::getInstance();

  scheduler.addTask(taskButtonDetector, TASK1_PERIOD_MS, TASK1_OFFSET_MS);
  scheduler.addTask(taskPressCounter, TASK2_PERIOD_MS, TASK2_OFFSET_MS);
  scheduler.addTask(taskStatusMonitor, TASK3_PERIOD_MS, TASK3_OFFSET_MS);
}
