#pragma once
#include <Arduino.h>
#include "tasks/task_actuator_control.h"
#include "tasks/task_conditioning.h"
#include "tasks/task_reporting.h"

namespace Config {
  constexpr uint8_t RELAY_PIN = 7;
  constexpr uint8_t DHT11_PIN = 3;
  constexpr uint8_t BTN_UP_PIN = 9;
  constexpr uint8_t BTN_DOWN_PIN = 8;

  inline constexpr TaskConditioning::Config CONDITIONING_CFG{
      .dhtPin = DHT11_PIN,
      .upButtonPin = BTN_UP_PIN,
      .downButtonPin = BTN_DOWN_PIN,
      .buttonsActiveLow = true,
      .buttonDebounceMs = 30,
      .setPointInitPct = 55,
      .setPointMinPct = 30,
      .setPointMaxPct = 90,
      .setPointStepPct = 1,
      .hysteresisPct = 3.0f,
      // Fast button response with debouncing.
      .buttonPeriodMs = 30,
    // DHT11 minimum read interval is about 2 s.
    .samplePeriodMs = 2000,
      // Control check can be faster than sensor sampling.
      .controlPeriodMs = 100,
  };

  inline constexpr TaskActuatorControl::Config ACTUATOR_CFG{
      .relayPin = RELAY_PIN,
      .relayActiveHigh = true,
      // Relay command update period.
      .periodMs = 50,
  };

  inline constexpr TaskReporting::Config REPORTING_CFG{
      // Human-readable periodic report over STDIO.
      .periodMs = 1000,
  };
}
