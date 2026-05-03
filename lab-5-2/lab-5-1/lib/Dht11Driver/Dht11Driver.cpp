#include "Dht11Driver.h"

#include <DHT.h>
#include <math.h>

namespace {
  DHT* g_sensor = nullptr;
}

void Dht11Driver::setup(const Config& cfg) {
  m_dataPin = cfg.dataPin;

  if (g_sensor != nullptr) {
    delete g_sensor;
    g_sensor = nullptr;
  }

  g_sensor = new DHT(m_dataPin, DHT11);
  g_sensor->begin();
}

bool Dht11Driver::readHumidityPercent(float& outHumidityPercent) {
  if (g_sensor == nullptr) {
    return false;
  }

  const float h = g_sensor->readHumidity();
  if (isnan(h)) {
    return false;
  }

  outHumidityPercent = h;
  return true;
}
