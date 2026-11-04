/**
 * @file adc_driver.cpp
 * @brief Implementace čtení z ADS1115
 */

#include "hal/adc_driver.h"
#include "config.h"
#include <Adafruit_ADS1X15.h>

static Adafruit_ADS1115 ads;

AdcDriver::AdcDriver() 
  : _ready(false), _firstSample(true), _filteredVoltage(0.0f), _filteredCurrent(0.0f) {}

bool AdcDriver::begin() {
  ads.setGain(GAIN_ONE);                 // Rozsah +/- 4.096 V (1 LSB = 0.125 mV)
  ads.setDataRate(RATE_ADS1115_860SPS);  // Nejvyšší rychlost (860 SPS -> ~1.16 ms na kanál místo výchozích 8 ms)
  _ready = ads.begin(I2C_ADDR_ADS1115);
  return _ready;
}

bool AdcDriver::isConnected() const {
  return _ready;
}

void AdcDriver::read(float &outVoltage, float &outCurrent) {
  if (!_ready) {
    outVoltage = 0.0f;
    outCurrent = 0.0f;
    return;
  }

  // Kanál A1 (VMON): Napětí zátěže (přes napěťový dělič 11:1, rozsah +/- 4.096 V)
  ads.setGain(GAIN_ONE);
  int16_t rawV = ads.readADC_SingleEnded(ADC_CHANNEL_VMON);
  if (rawV < 0) rawV = 0; // Ošetření záporného offsetu v single-ended režimu
  float voltsVmon = ads.computeVolts(rawV);
  float rawVoltage = voltsVmon * VOLTAGE_DIVIDER_RATIO;
  if (rawVoltage < 0.05f) rawVoltage = 0.0f;

  // Kanál A0 (IMON): Proud zátěže (úbytek na bočníku R_shunt1 = 0.100 Ohm, rozsah +/- 1.024 V -> 4x vyšší rozlišení 0.31 mA/LSB)
  ads.setGain(GAIN_FOUR);
  int16_t rawI = ads.readADC_SingleEnded(ADC_CHANNEL_IMON);
  if (rawI < 0) rawI = 0;
  float voltsImon = ads.computeVolts(rawI);
  float rawCurrent = voltsImon / SHUNT_RESISTANCE_OHM;
  if (rawCurrent < 0.003f) rawCurrent = 0.0f;

  // Jednoduchý EMA (Exponential Moving Average) filtr pro potlačení šumu:
  // y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
  if (_firstSample) {
    _filteredVoltage = rawVoltage;
    _filteredCurrent = rawCurrent;
    _firstSample = false;
  } else {
    const float alpha = 0.35f;
    _filteredVoltage = alpha * rawVoltage + (1.0f - alpha) * _filteredVoltage;
    _filteredCurrent = alpha * rawCurrent + (1.0f - alpha) * _filteredCurrent;
  }

  outVoltage = _filteredVoltage;
  outCurrent = _filteredCurrent;
}
