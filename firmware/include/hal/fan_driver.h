/**
 * @file fan_driver.h
 * @brief Ovladač PWM ventilátoru (Intel 4-pin 25 kHz)
 */

#ifndef FAN_DRIVER_H
#define FAN_DRIVER_H

#include <Arduino.h>

class FanDriver {
public:
  FanDriver();
  void begin();

  /**
   * @brief Přepočítá otáčky ventilátoru podle aktuální teploty a stavu ochran
   * @param currentTemp Teplota chladiče [°C]
   * @param isAlarm Příznak aktivního havarijního alarmu
   */
  void update(float currentTemp, bool isAlarm);

  /**
   * @brief Vrátí aktuální střídu ventilátoru v procentech (0 až 100 %)
   */
  uint8_t getSpeedPercent() const;

private:
  uint8_t _speedPercent;
};

#endif // FAN_DRIVER_H
