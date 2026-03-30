#pragma once
#include <Arduino.h>
#include "RemoteDriver.h"
#include "tasks/task_actuator_control.h"
#include "tasks/task_conditioning.h"
#include "tasks/task_reporting.h"

namespace Config {
  constexpr uint8_t IR_PIN = 2;
  constexpr uint8_t SERVO_PIN = 7;

  constexpr uint32_t IR_CODE_UP = 0xD92655AA;
  constexpr uint32_t IR_CODE_RIGHT = 0xDB2455AA;
  constexpr uint32_t IR_CODE_DOWN = 0xD82755AA;
  constexpr uint32_t IR_CODE_LEFT = 0xDA2555AA;

  inline constexpr RemoteDriver::Config REMOTE_CFG{
      .irReceivePin = IR_PIN,
      .codeUp = IR_CODE_UP,
      .codeRight = IR_CODE_RIGHT,
      .codeDown = IR_CODE_DOWN,
      .codeLeft = IR_CODE_LEFT,
  };

  inline constexpr TaskConditioning::Config CONDITIONING_CFG{
      .minDeg = 0,
      .maxDeg = 180,
      .wNew = 0.6f,
      .stepBypassDeg = 25,
      .rampStepDeg = 24,
      .periodMs = 40,
  };

  inline constexpr TaskActuatorControl::Config ACTUATOR_CFG{
      .servoPin = SERVO_PIN,
      .periodMs = 50,
  };

  inline constexpr TaskReporting::Config REPORTING_CFG{
      .periodMs = 200,
  };
}
