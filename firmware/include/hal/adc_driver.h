/**
 * @file adc_driver.h
 * @brief Ovladač 16-bitového diferenciálního ADC převodníku ADS1115
 */

#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <Arduino.h>

class AdcDriver {
public:
  AdcDriver();
  bool begin();
  bool isConnected() const;

  /**
   * @brief Přečte napětí a proud z převodníku s filtrací šumu
   * @param[out] outVoltage Skutečné napětí na svorkách [V]
   * @param[out] outCurrent Skutečný proud bočníkem [A]
   */
  void read(float &outVoltage, float &outCurrent);

private:
  bool _ready;
  bool _firstSample;
  float _filteredVoltage;
  float _filteredCurrent;
};

#endif // ADC_DRIVER_H
