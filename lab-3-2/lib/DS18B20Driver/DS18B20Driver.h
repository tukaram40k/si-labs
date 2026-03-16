#ifndef DS18B20_DRIVER_H
#define DS18B20_DRIVER_H

#include <stdint.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Driver
{
public:
  DS18B20Driver(uint8_t pin);

  void begin();

  /**
   * Request a temperature conversion and read the result.
   * @return Temperature in °C, or DEVICE_DISCONNECTED_C on failure.
   */
  float readTemperature();

  /**
   * Check if the last reading was valid.
   */
  bool isValid(float temp);

private:
  uint8_t _pin;
  OneWire _oneWire;
  DallasTemperature _sensors;
};

#endif // DS18B20_DRIVER_H