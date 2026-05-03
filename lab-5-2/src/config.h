#pragma once
#include <Arduino.h>
#include "tasks/task_actuator_control.h"
#include "tasks/task_conditioning.h"
#include "tasks/task_reporting.h"

namespace Config
{
  constexpr uint8_t SERVO_PIN = 7;
  constexpr uint8_t DHT11_PIN = 3;
  constexpr uint8_t BTN_UP_PIN = 9;
  constexpr uint8_t BTN_DOWN_PIN = 8;

  inline constexpr TaskConditioning::Config CONDITIONING_CFG{
      .dhtPin = DHT11_PIN,
      .upButtonPin = BTN_UP_PIN,
      .downButtonPin = BTN_DOWN_PIN,
      .buttonsActiveLow = true,
      .buttonDebounceMs = 30,
      .setPointInitPct = 45,
      .setPointMinPct = 30,
      .setPointMaxPct = 90,
      .setPointStepPct = 1,
      .kp = 5.0f,
      .ki = 0.1f,
      .kd = 0.8f,
    .outputMinPct = 0.0f,
      .outputMaxPct = 100.0f,
      .buttonPeriodMs = 30,
      .samplePeriodMs = 2000,
      .controlPeriodMs = 2000,
      .controlWindowMs = 2000,
  };

  inline constexpr TaskActuatorControl::Config ACTUATOR_CFG{
      .servoPin = SERVO_PIN,
      .periodMs = 50,
      .minFlipStepPeriodMs = 10,
      .maxFlipStepPeriodMs = 80,
  };

  inline constexpr TaskReporting::Config REPORTING_CFG{
      .periodMs = 1000,
  };
}
