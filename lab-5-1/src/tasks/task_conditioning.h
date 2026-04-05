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
    float hysteresisPct;

    uint16_t buttonPeriodMs;
    uint16_t samplePeriodMs;
    uint16_t controlPeriodMs;
  };

  struct State
  {
    float humidityPct;
    bool humidityValid;
    int setPointPct;
    float lowerThresholdPct;
    float upperThresholdPct;
    bool relayRequestOn;
  };

  void setup(const Config &cfg);
  void tick();
  State getState();
}
