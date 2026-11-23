/**
 * @file adc_driver.cpp
 * @brief Vysoce optimalizované asynchronní čtení z ADS1115 bez blokování smyčky
 */

#include "hal/adc_driver.h"
#include "config.h"
#include <Adafruit_ADS1X15.h>

static Adafruit_ADS1115 ads;

// Předpočítané konstanty převodu (šetří dělení v plovoucí řádové čárce za běhu)
// VMON: GAIN_ONE  (+/- 4.096 V) -> 1 LSB = 0.125 mV   -> s děličem 11:1 = 1.375 mV/LSB
// IMON: GAIN_FOUR (+/- 1.024 V) -> 1 LSB = 0.03125 mV -> s bočníkem 0.1 Ω = 0.3125 mA/LSB (4x vyšší přesnost)
static constexpr float FACTOR_VOLTS   = (4.096f / 32768.0f) * VOLTAGE_DIVIDER_RATIO;
static constexpr float FACTOR_CURRENT = (1.024f / 32768.0f) / SHUNT_RESISTANCE_OHM;

AdcDriver::AdcDriver() 
  : _ready(false),
    _firstSample(true),
    _state(ADC_STATE_IDLE),
    _convStartMs(0),
    _convStartUs(0),
    _filteredVoltage(0.0f),
    _filteredCurrent(0.0f),
    _cal({1.0f, 1.0f, 1.0f, 0.0f}) {}

void AdcDriver::setCalibration(const CalibrationData& cal) {
  _cal = cal;
}

bool AdcDriver::begin() {
  _ready = ads.begin(I2C_ADDR_ADS1115);
  if (_ready) {
    ads.setDataRate(RATE_ADS1115_860SPS); // Maximální rychlost 860 vzorků/s (~1.16 ms / převod)

    // Spustíme první asynchronní převod napětí (VMON) s rozsahem +/- 4.096 V
    ads.setGain(GAIN_ONE);
    ads.startADCReading(MUX_BY_CHANNEL[ADC_CHANNEL_VMON], false);
    _state = ADC_WAIT_VMON;
    _convStartMs = millis();
    _convStartUs = micros();
  }
  return _ready;
}

bool AdcDriver::isConnected() const {
  return _ready;
}

void AdcDriver::poll() {
  if (!_ready) return;

  // Převod při 860 SPS trvá ~1.16 ms; nezahlcujeme I2C sběrnici zbytečným dotazováním
  if (micros() - _convStartUs < 1150) return;

  // Kontrola dokončení převodu bez aktivního blokování CPU
  if (ads.conversionComplete()) {
    if (_state == ADC_WAIT_VMON) {
      int16_t rawV = ads.getLastConversionResults();
      if (rawV < 0) rawV = 0;
      float rawVoltage = (float)rawV * FACTOR_VOLTS * _cal.vScale;
      if (rawVoltage < 0.05f) rawVoltage = 0.0f;

      if (_firstSample) {
        _filteredVoltage = rawVoltage;
      } else {
        _filteredVoltage = 0.35f * rawVoltage + 0.65f * _filteredVoltage;
      }

      // Přepneme zisk na +/- 1.024 V a okamžitě zahájíme převod proudu (IMON)
      ads.setGain(GAIN_FOUR);
      ads.startADCReading(MUX_BY_CHANNEL[ADC_CHANNEL_IMON], false);
      _state = ADC_WAIT_IMON;
      _convStartMs = millis();
      _convStartUs = micros();

    } else if (_state == ADC_WAIT_IMON) {
      int16_t rawI = ads.getLastConversionResults();
      if (rawI < 0) rawI = 0;
      float rawCurrent = ((float)rawI * FACTOR_CURRENT * _cal.iScale) + _cal.iOffset;
      if (rawCurrent < 0.003f) rawCurrent = 0.0f;

      if (_firstSample) {
        _filteredCurrent = rawCurrent;
        _firstSample = false;
      } else {
        _filteredCurrent = 0.35f * rawCurrent + 0.65f * _filteredCurrent;
      }

      // Přepneme zisk zpět na +/- 4.096 V a zahájíme další převod napětí (VMON)
      ads.setGain(GAIN_ONE);
      ads.startADCReading(MUX_BY_CHANNEL[ADC_CHANNEL_VMON], false);
      _state = ADC_WAIT_VMON;
      _convStartMs = millis();
      _convStartUs = micros();
    }
  } else {
    // Watchdog převodu: Pokud by z nějakého důvodu převod ustrnul déle než 15 ms, restartujeme
    if (millis() - _convStartMs > 15) {
      ads.setGain(GAIN_ONE);
      ads.startADCReading(MUX_BY_CHANNEL[ADC_CHANNEL_VMON], false);
      _state = ADC_WAIT_VMON;
      _convStartMs = millis();
      _convStartUs = micros();
    }
  }
}

void AdcDriver::read(float &outVoltage, float &outCurrent) {
  // poll() je volána v hlavní smyčce; zde jen bleskově vrátíme nejnovější filtrované hodnoty
  outVoltage = _filteredVoltage;
  outCurrent = _filteredCurrent;
}
