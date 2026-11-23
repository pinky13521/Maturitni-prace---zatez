/**
 * @file adc_driver.h
 * @brief Ovladač 16-bitového diferenciálního ADC převodníku ADS1115
 */

#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <Arduino.h>
#include "types.h"

class AdcDriver {
public:
  AdcDriver();
  bool begin();
  bool isConnected() const;
  void setCalibration(const CalibrationData& cal);

  /**
   * @brief Ne-blokující obsluha stavového automatu ADC (volána v hlavní smyčce)
   */
  void poll();

  /**
   * @brief Okamžitě vrátí filtrované napětí a proud bez jakéhokoliv blokování I2C
   * @param[out] outVoltage Skutečné napětí na svorkách [V]
   * @param[out] outCurrent Skutečný proud bočníkem [A]
   */
  void read(float &outVoltage, float &outCurrent);

private:
  enum AdcState {
    ADC_STATE_IDLE,
    ADC_WAIT_VMON,
    ADC_WAIT_IMON
  };

  bool _ready;
  bool _firstSample;
  AdcState _state;
  unsigned long _convStartMs;
  unsigned long _convStartUs;
  float _filteredVoltage;
  float _filteredCurrent;
  CalibrationData _cal;
};

#endif // ADC_DRIVER_H
