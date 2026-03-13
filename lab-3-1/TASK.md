# Prompt for Coding Agent

You are implementing an **ESP32 embedded project using FreeRTOS**. The project is already initialized and contains:

* A **working ESP32 placeholder project**
* **FreeRTOS already configured**
* An **LCD driver implementation already present in the project** (reuse it and modify if necessary)
* A **Wokwi simulation configuration in `diagram.json`**
* The simulation uses the following sensors:

  * `wokwi-ntc-temperature-sensor` → analog NTC thermistor
  * `wokwi-ds18b20` → digital temperature sensor (1-Wire)

Do **not implement laboratory report content or theory**. Only implement the **code architecture and functionality**.

The final system must implement **both parts of the assignment in a single project**:

* Binary signal processing (alert detection)
* Analog signal processing (filtering and conditioning)

The final system must read **two sensors simultaneously**:

* **Analog temperature sensor (NTC)**
* **Digital temperature sensor (DS18B20)**

The system must display information for **both sensors on an LCD using STDIO (`printf`)**.

---

# System Requirements

The system must implement a **real-time multitasking architecture using FreeRTOS**.

Use **separate tasks** for different functionalities.

Target behavior:

* Periodic sensor acquisition
* Signal conditioning
* Threshold detection with hysteresis
* Display reporting

The system must react to signal changes with **latency below 100 ms**.

---

# Hardware Components (Simulation)

MCU:

* ESP32

Sensors:

### Analog Sensor

`wokwi-ntc-temperature-sensor`

Characteristics:

* Analog voltage output
* Must be read via **ESP32 ADC**

### Digital Sensor

`wokwi-ds18b20`

Characteristics:

* Digital temperature sensor
* Uses **1-Wire protocol**

---

# Sensors To Implement

## Digital Temperature Sensor (DS18B20)

Responsibilities:

* Implement a driver for reading temperature
* Use a 1-Wire communication library or implement basic protocol

Returned value should be **temperature in Celsius**.

---

## Analog Temperature Sensor (NTC Thermistor)

Responsibilities:

* Read analog voltage via ADC
* Convert ADC value to voltage
* Convert voltage to temperature

---

# Acquisition Requirements

Sensors must be sampled periodically.

Sampling period:

```
20–100 ms
```

Acquisition should return **raw data**.

---

# Signal Conditioning

This is required mainly for the **analog sensor**, but the architecture should support both.

Implement the following processing stages:

### 1. Saturation

Clamp values inside valid limits.

Example:

```
temperature ∈ [-40°C, 125°C]
```

---

### 2. Median Filter

Remove **salt-and-pepper noise**.

Use a sliding window.

Example:

```
window size = 3 or 5 samples
```

---

### 3. Weighted Moving Average

Smooth the signal.

Example formula:

```
filtered = w1*x[n] + w2*x[n-1] + w3*x[n-2]
```

Example weights:

```
0.6, 0.3, 0.1
```

---

# Binary Signal Processing (Alert Detection)

This part implements **threshold detection with hysteresis**.

Use temperature thresholds:

```
Threshold: 25°C
Hysteresis: ±1°C
```

Behavior:

```
Alert ON  when T > 26°C
Alert OFF when T < 24°C
```

Purpose:

Prevent alert oscillations when temperature fluctuates near the threshold.

---

# Alert Validation (Debouncing)

Alert conditions must not trigger immediately.

Implement **persistence validation**:

Alert is triggered only if the condition remains true for a **minimum time window**.

Example:

```
condition must persist for 200 ms
```

Implementation options:

* counter
* timer

---

# FreeRTOS Task Architecture

Implement the following tasks.

---

# 1. Acquisition Task

Frequency:

```
50 ms
```

Responsibilities:

* Read NTC raw ADC value
* Read DS18B20 temperature
* Store results in shared structures

Example shared structure:

```
typedef struct {
    float ntc_temp;
    uint16_t ntc_adc;
    float ntc_voltage;

    float ds18b20_temp;
} sensor_data_t;
```

Protect shared data with:

* mutex
* semaphore

---

# 2. Conditioning Task

Frequency:

```
50–100 ms
```

Responsibilities:

* Apply filtering to analog temperature
* Apply saturation
* Produce processed temperature values

Outputs:

```
filtered_ntc_temp
filtered_ds18b20_temp
```

---

# 3. Alert Task

Frequency:

```
50 ms
```

Responsibilities:

* Evaluate temperature thresholds
* Implement hysteresis
* Implement alert persistence (debouncing)
* Maintain system alert state

Example state:

```
typedef enum {
    ALERT_OFF,
    ALERT_ON
} alert_state_t;
```

Alerts should be evaluated **for both sensors independently**.

---

# 4. Display Task

Frequency:

```
500 ms
```

Responsibilities:

* Print system status via LCD (using printf)
* Display intermediate and processed values

Example output format:

```
NTC Sensor:
  Raw ADC: 2134
  Voltage: 1.65 V
  Filtered Temp: 23.4 C

DS18B20 Sensor:
  Temp: 24.1 C

Alerts:
  NTC: OK
  DS18B20: ALERT
```

---

# Inter-Task Communication

Use:

* mutex for shared data protection
* binary semaphores if necessary

Shared structures:

```
sensor_data_t
processed_data_t
alert_state_t
```

---

# Simulation Environment

The system runs in **Wokwi**.

Sensors are already defined in `diagram.json`.

Do **not modify the sensors** unless necessary.

Ensure your code works with:

```
wokwi-ntc-temperature-sensor
wokwi-ds18b20
```

---

# Coding Guidelines

Follow these principles:

* Modular design
* Separate drivers / services / tasks
* Avoid code duplication
* Use clear interfaces
* Comment important logic
* Make parameters configurable

---

# Expected Outcome

A FreeRTOS ESP32 application that:

* Reads **two temperature sensors simultaneously**
* Processes signals in **parallel tasks**
* Implements **filtering and signal conditioning**
* Detects **threshold alerts with hysteresis**
* Displays **intermediate and final values on LCD**
* Runs correctly in **Wokwi simulation**
