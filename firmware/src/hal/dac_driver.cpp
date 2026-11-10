/**
 * @file dac_driver.cpp
 * @brief Implementace řízení MCP4725 s předpočítanou kalibrací a soft-startem
 */

#include "hal/dac_driver.h"
#include "config.h"
#include <Adafruit_MCP4725.h>

static Adafruit_MCP4725 dac;

// Předpočítaná konstanta: Code = Amps * (R_shunt / Vref) * 4095
static constexpr float FACTOR_AMPS_TO_CODE = (SHUNT_RESISTANCE_OHM / DAC_VREF_VOLTS) * (float)DAC_MAX_VALUE;

// Maximální povolený přírůstek proudu za jeden krok (20 ms) -> 0.35 A / 20 ms = 17.5 A/s
// Zabraňuje tvrdým proudovým rázům a indukovaným špičkám na vodičích
static constexpr float MAX_RAMP_STEP_AMPS = 0.35f;

DacDriver::DacDriver() : _ready(false), _lastCode(0), _currentRampAmps(0.0f) {}

bool DacDriver::begin() {
  _ready = dac.begin(I2C_ADDR_MCP4725);
  if (_ready) {
    // Vynulujeme výstup i EEPROM paměť MCP4725 pro bezpečný boot
    dac.setVoltage(0, true);
    _lastCode = 0;
    _currentRampAmps = 0.0f;
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

  // Soft-start rampa při náběhu proudu
  if (targetAmps > _currentRampAmps) {
    _currentRampAmps += MAX_RAMP_STEP_AMPS;
    if (_currentRampAmps > targetAmps) {
      _currentRampAmps = targetAmps;
    }
  } else {
    // Snižování proudu je okamžité pro rychlou regulaci a bezpečnost
    _currentRampAmps = targetAmps;
  }

  // Rychlý přepočet proudu na 12-bitový kód DAC
  int code = (int)(_currentRampAmps * FACTOR_AMPS_TO_CODE);
  code = constrain(code, 0, DAC_MAX_VALUE);

  if (code != _lastCode) {
    _lastCode = code;
    dac.setVoltage((uint16_t)code, false);
  }
}

void DacDriver::shutdown() {
  _currentRampAmps = 0.0f;
  if (_ready && _lastCode != 0) {
    _lastCode = 0;
    dac.setVoltage(0, false);
  }
}
