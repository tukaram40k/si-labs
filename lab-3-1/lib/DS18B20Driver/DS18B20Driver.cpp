#include "DS18B20Driver.h"

DS18B20Driver::DS18B20Driver(uint8_t pin)
    : _pin(pin), _oneWire(pin), _sensors(&_oneWire) {}

void DS18B20Driver::begin() {
    _sensors.begin();
    // Use asynchronous mode for non-blocking reads
    _sensors.setWaitForConversion(true);
    _sensors.setResolution(12); // 12-bit resolution
}

float DS18B20Driver::readTemperature() {
    _sensors.requestTemperatures();
    float tempC = _sensors.getTempCByIndex(0);
    return tempC;
}

bool DS18B20Driver::isValid(float temp) {
    return (temp != DEVICE_DISCONNECTED_C && temp > -127.0f);
}