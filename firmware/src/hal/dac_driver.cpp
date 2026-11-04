/**
 * @file dac_driver.cpp
 * @brief Implementace řízení MCP4725
 */

#include "hal/dac_driver.h"
#include "config.h"
#include <Adafruit_MCP4725.h>

static Adafruit_MCP4725 dac;

DacDriver::DacDriver() : _ready(false), _lastCode(0) {}

bool DacDriver::begin() {
  _ready = dac.begin(I2C_ADDR_MCP4725);
  if (_ready) {
    // Vynulujeme výstup a zapíšeme 0 i do interní EEPROM paměti MCP4725,
    // aby se při příštím zapnutí napájení zátěž nespustila se starou hodnotou.
    dac.setVoltage(0, true);
    _lastCode = 0;
  }
  return _ready;
}

bool DacDriver::isConnected() const {
  return _ready;
}

void DacDriver::setCurrent(float targetAmps) {
  if (!_ready) return;

  if (targetAmps <= 0.0f) {
    shutdown();
    return;
  }

  if (targetAmps > LIMIT_MAX_CURRENT) {
    targetAmps = LIMIT_MAX_CURRENT;
  }

  // Úbytek na bočníku: V_target = I * R_shunt (pro 5 A a 0.1 Ohm je to 0.500 V)
  float targetVshunt = targetAmps * SHUNT_RESISTANCE_OHM;

  // Přepočet na 12-bitový kód DAC: Code = (V_target / Vref) * 4095
  int code = (int)((targetVshunt / DAC_VREF_VOLTS) * (float)DAC_MAX_VALUE);
  code = constrain(code, 0, DAC_MAX_VALUE);

  if (code != _lastCode) {
    _lastCode = code;
    dac.setVoltage((uint16_t)code, false);
  }
}

void DacDriver::shutdown() {
  if (_ready && _lastCode != 0) {
    _lastCode = 0;
    dac.setVoltage(0, false);
  }
}
