#ifndef NTC_DRIVER_H
#define NTC_DRIVER_H

#include <stdint.h>

class NTCDriver
{
public:
  /**
   * @param pin        GPIO pin for analog reading
   * @param seriesR    Series resistor value (ohms)
   * @param nominalR   NTC resistance at nominal temperature (ohms)
   * @param nominalT   Nominal temperature (°C)
   * @param beta       Beta coefficient of the NTC
   * @param adcMax     ADC maximum value (4095 for 12-bit)
   * @param vRef       Reference voltage (V)
   */
  NTCDriver(uint8_t pin, float seriesR, float nominalR,
            float nominalT, float beta, float adcMax, float vRef);

  void begin();

  uint16_t readRawADC();
  float adcToVoltage(uint16_t adcValue);
  float voltageToTemperature(float voltage);

  // Convenience: read and convert in one call
  float readTemperature(uint16_t *outADC = nullptr, float *outVoltage = nullptr);

private:
  uint8_t _pin;
  float _seriesR;
  float _nominalR;
  float _nominalT;
  float _beta;
  float _adcMax;
  float _vRef;
};

#endif // NTC_DRIVER_H