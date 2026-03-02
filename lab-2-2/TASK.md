## Context

We are implementing **Part 2 – Preemptive System using FreeRTOS** of a laboratory project.

A **bare-metal version already exists in the current working directory**.
You must:

* Reuse the existing:

  * Button driver
  * LED driver
  * IO / STDIO module
* Keep the same HW abstraction layers (MCAL / ECAL / SRV separation).
* Replace the bare-metal scheduler logic with a **FreeRTOS preemptive multitasking implementation**.
* Do NOT worry about documentation, diagrams, grading, or laboratory report.
* Focus strictly on implementing the required functionality correctly using FreeRTOS.

---

# APPLICATION DESCRIPTION

## System Overview

This is a **button press duration monitoring system** with:

* 1 physical button
* 3 LEDs:

  * Green LED
  * Red LED
  * Yellow LED
* STDIO (serial output)

The system must:

1. Measure the duration of every button press.
2. Classify presses as SHORT or LONG.
3. Provide visual LED feedback.
4. Maintain statistics.
5. Print statistics every 10 seconds.
6. Reset statistics after each report.

The system must ensure **latency under 100 ms** for button reaction.

---

# BUTTON PRESS RULES

* A press is detected on transition: RELEASED → PRESSED
* Duration is measured from press until release.
* Duration threshold:

  * SHORT press: < 500 ms
  * LONG press: ≥ 500 ms

Debounce must be handled (reuse logic from existing driver if available).

---

# LED BEHAVIOR (VERY IMPORTANT)

Each LED has a specific purpose:

## 1. GREEN LED

Indicates a SHORT press (< 500 ms).

Behavior:

* When a valid short press is detected:

  * Turn GREEN LED ON
  * Keep it ON briefly (e.g., 100–200 ms)
  * Then turn OFF

## 2. RED LED

Indicates a LONG press (≥ 500 ms).

Behavior:

* When a valid long press is detected:

  * Turn RED LED ON
  * Keep it ON briefly (e.g., 100–200 ms)
  * Then turn OFF

## 3. YELLOW LED

Indicates press count feedback via blinking.

Behavior:

* For SHORT press:

  * Blink YELLOW LED 5 times (fast blink)
* For LONG press:

  * Blink YELLOW LED 10 times (fast blink)

Blinking must not block other tasks (use vTaskDelay, not busy-wait).

---

# REQUIRED TASK ARCHITECTURE (FreeRTOS)

You must implement **3 separate FreeRTOS tasks**.
You must use <Arduino_FreeRTOS.h>.

---

## TASK 1 – Button Detection & Duration Measurement

Responsibilities:

* Continuously monitor button state.
* Detect transitions.
* Measure press duration in milliseconds.
* Classify press as SHORT or LONG.
* Store last duration in shared variable.
* Signal that a new press event occurred.

FreeRTOS requirements:

* Use `vTaskDelay()` or `vTaskDelayUntil()` for periodic polling.
* Use a **binary semaphore** to notify Task 2.
* Protect shared duration variable using a **mutex**.

---

## TASK 2 – Statistics & Yellow LED Blink

Triggered when a press event occurs.

Responsibilities:

* Increment:

  * Total press count
  * Short press count
  * Long press count
* Maintain:

  * Sum of short durations
  * Sum of long durations
* Blink YELLOW LED:

  * 5 times for SHORT
  * 10 times for LONG

FreeRTOS requirements:

* Wait on binary semaphore from Task 1.
* Use mutex when accessing shared statistics.
* Do NOT block indefinitely with busy loops.
* Use `vTaskDelay()` for blinking timing.

---

## TASK 3 – Periodic Reporting

Runs every 10 seconds.

Responsibilities:

Print via STDIO:

* Total number of presses
* Number of short presses
* Number of long presses
* Average duration of presses

Average formula:

```
average = (sum_short + sum_long) / total_presses
```

After printing:

* Reset all counters and sums to zero.

FreeRTOS requirements:

* Use `vTaskDelayUntil()` for accurate 10-second periodic execution.
* Use mutex to safely access shared statistics.

---

# SYNCHRONIZATION REQUIREMENTS

You MUST implement:

* 1 Binary Semaphore:

  * Used to signal press event from Task 1 → Task 2.

* 1 Mutex:

  * Protects:

    * Duration variable
    * Counters
    * Sum variables

No global variable may be accessed without mutex protection.

---

# STRUCTURE REQUIREMENTS

* Each task should be in its own module file:

  * task_measure.cpp
  * task_stats.cpp
  * task_report.cpp

* Use existing LED and button APIs.

* main():

  * Initialize hardware
  * Initialize drivers
  * Create tasks using `xTaskCreate()`
  * Create semaphore and mutex
  * Start scheduler with `vTaskStartScheduler()`

Scheduler must never return.

---

# TIMING REQUIREMENTS

* Button polling period: ~10–20 ms
* Yellow blink delay: ~50–100 ms
* Green/Red indication duration: ~100–200 ms
* Reporting interval: exactly 10 seconds

Use FreeRTOS timing primitives only.

---

# WHAT NOT TO DO

* Do NOT use busy waiting.
* Do NOT use bare-metal scheduler logic.
* Do NOT include lab report code.
* Do NOT implement the bare-metal version.

---

# EXPECTED RESULT

System should:

1. Detect button presses reliably.
2. Correctly classify short vs long.
3. Signal:

   * Green LED for short
   * Red LED for long
   * Yellow blinking feedback
4. Print statistics every 10 seconds.
5. Reset statistics after reporting.
6. Run fully preemptive under FreeRTOS.

---

Implement clean, modular, thread-safe FreeRTOS-based code only.
