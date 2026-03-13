#include "NTCDriver.h"
#include <Arduino.h>
#include <math.h>

NTCDriver::NTCDriver(uint8_t pin, float seriesR, float nominalR,
                     float nominalT, float beta, float adcMax, float vRef)
    : _pin(pin), _seriesR(seriesR), _nominalR(nominalR),
      _nominalT(nominalT), _beta(beta), _adcMax(adcMax), _vRef(vRef) {}

void NTCDriver::begin() {
    // GPIO 34-39 are input-only, no need for pinMode
    // Configure ADC: 12-bit resolution, 11dB attenuation for full 0-3.3V range
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
}

uint16_t NTCDriver::readRawADC() {
    return (uint16_t)analogRead(_pin);
}

float NTCDriver::adcToVoltage(uint16_t adcValue) {
    return ((float)adcValue / _adcMax) * _vRef;
}

float NTCDriver::voltageToTemperature(float voltage) {
    // NTC is in a voltage divider: Vout = Vcc * R_ntc / (R_series + R_ntc)
    // Solving for R_ntc: R_ntc = R_series * Vout / (Vcc - Vout)
    if (voltage >= _vRef || voltage <= 0.0f) {
        return -999.0f; // Invalid reading
    }

    float resistance = _seriesR * voltage / (_vRef - voltage);

    // Steinhart-Hart simplified (Beta equation):
    // 1/T = 1/T0 + (1/B) * ln(R/R0)
    float steinhart = resistance / _nominalR;
    steinhart = logf(steinhart);
    steinhart /= _beta;
    steinhart += 1.0f / (_nominalT + 273.15f);
    float tempK = 1.0f / steinhart;
    float tempC = tempK - 273.15f;

    return tempC;
}

float NTCDriver::readTemperature(uint16_t *outADC, float *outVoltage) {
    uint16_t adc = readRawADC();
    float voltage = adcToVoltage(adc);
    float temp = voltageToTemperature(voltage);

    if (outADC) *outADC = adc;
    if (outVoltage) *outVoltage = voltage;

    return temp;
}