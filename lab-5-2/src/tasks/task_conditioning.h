#pragma once

#include <Arduino.h>

namespace TaskConditioning
{
  struct Config
  {
    uint8_t dhtPin;
    uint8_t upButtonPin;
    uint8_t downButtonPin;
    bool buttonsActiveLow;
    uint16_t buttonDebounceMs;

    int setPointInitPct;
    int setPointMinPct;
    int setPointMaxPct;
    int setPointStepPct;

    float kp;
    float ki;
    float kd;
    float outputMinPct;
    float outputMaxPct;

    uint16_t buttonPeriodMs;
    uint16_t samplePeriodMs;
    uint16_t controlPeriodMs;
    uint16_t controlWindowMs;
  };

  struct State
  {
    float humidityPct;
    bool humidityValid;
    int setPointPct;
    float controlOutputPct;
    float controlDutyPct;
    bool actuatorRequestOn;
  };

  void setup(const Config &cfg);
  void tick();
  State getState();
}
