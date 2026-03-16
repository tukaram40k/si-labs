#include "DS18B20Driver.h"

DS18B20Driver::DS18B20Driver(uint8_t pin)
    : _pin(pin), _oneWire(pin), _sensors(&_oneWire) {}

void DS18B20Driver::begin()
{
  _sensors.begin();
  // Use NON-BLOCKING mode so requestTemperatures() returns immediately
  _sensors.setWaitForConversion(false);
  _sensors.setResolution(10); // 10-bit resolution (~187ms conversion)
}

float DS18B20Driver::readTemperature()
{
  // Read result from the PREVIOUS conversion request
  float tempC = _sensors.getTempCByIndex(0);
  // Start the NEXT conversion (non-blocking, returns immediately)
  _sensors.requestTemperatures();
  return tempC;
}

bool DS18B20Driver::isValid(float temp)
{
  return (temp != DEVICE_DISCONNECTED_C && temp > -127.0f);
}