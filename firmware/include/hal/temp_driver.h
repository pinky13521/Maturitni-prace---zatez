/**
 * @file temp_driver.h
 * @brief Ovladač digitálního teplotního senzoru DS18B20 (1-Wire)
 */

#ifndef TEMP_DRIVER_H
#define TEMP_DRIVER_H

#include <Arduino.h>

class TempDriver {
public:
  TempDriver();
  bool begin();
  bool isConnected() const;

  /**
   * @brief Vyžádá nové měření teploty bez blokování procesoru
   */
  void requestAsync();

  /**
   * @brief Přečte poslední naměřenou teplotu [°C]
   */
  float read();

private:
  bool _ready;
  float _lastValidTemp;
};

#endif // TEMP_DRIVER_H
