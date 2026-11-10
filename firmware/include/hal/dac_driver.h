/**
 * @file dac_driver.h
 * @brief Ovladač 12-bitového D/A převodníku MCP4725
 */

#ifndef DAC_DRIVER_H
#define DAC_DRIVER_H

#include <Arduino.h>

class DacDriver {
public:
  DacDriver();
  bool begin();
  bool isConnected() const;

  /**
   * @brief Nastaví referenční napětí pro analogový operační zesilovač s plynulým náběhem
   * @param targetAmps Požadovaný proud v ampérech (0.0 až LIMIT_MAX_CURRENT)
   */
  void setCurrent(float targetAmps);

  /**
   * @brief Okamžité bezpečné odpojení (výstup DAC = 0 V)
   */
  void shutdown();

private:
  bool _ready;
  uint16_t _lastCode;
  float _currentRampAmps;
};

#endif // DAC_DRIVER_H
