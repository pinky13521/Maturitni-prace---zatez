/**
 * @file load_regulator.cpp
 * @brief Implementace regulačních režimů a integrátorů kapacity
 */

#include "core/load_regulator.h"
#include "config.h"

LoadRegulator::LoadRegulator() 
  : _mode(MODE_CC), _enabled(false), _cutoffLowSamples(0) {
  _setpoints.current = 0.500f;      // Výchozí: 500 mA
  _setpoints.power = 10.0f;         // Výchozí: 10 W
  _setpoints.resistance = 10.0f;    // Výchozí: 10 Ω
  _setpoints.cutoffVoltage = 3.0f;  // Výchozí cut-off mez pro Li-Ion

  resetBatteryStats();
}

void LoadRegulator::setMode(LoadMode mode) {
  if (_mode != mode) {
    // Z bezpečnostních důvodů při změně režimu vždy odpojíme zátěž,
    // aby nedošlo k proudovému rázu při přechodu mezi CC / CP / CR.
    _enabled = false;
    _mode = mode;
    if (_mode == MODE_BATT) {
      resetBatteryStats();
    }
  }
}

LoadMode LoadRegulator::getMode() const {
  return _mode;
}

void LoadRegulator::enable(bool en) {
  _enabled = en;
  if (_enabled && _mode == MODE_BATT) {
    resetBatteryStats();
    _battStats.testStartTime = millis();
  }
}

bool LoadRegulator::isEnabled() const {
  return _enabled;
}

Setpoint& LoadRegulator::getSetpoints() {
  return _setpoints;
}

const BatteryStats& LoadRegulator::getBatteryStats() const {
  return _battStats;
}

void LoadRegulator::resetBatteryStats() {
  _battStats.discharged_mAh = 0.0f;
  _battStats.discharged_Wh = 0.0f;
  _battStats.testStartTime = 0;
  _battStats.testCompleted = false;
}

float LoadRegulator::update(const Measurement& meas, float dt_s) {
  if (!_enabled) {
    return 0.0f;
  }

  float targetAmps = 0.0f;

  switch (_mode) {
    case MODE_CC:
      // Konstantní proud: Přímo zadaná hodnota
      targetAmps = _setpoints.current;
      break;

    case MODE_CP:
      // Konstantní výkon: I = P / U
      // Ochrana proti dělení nulou a podpětí pod 1.0 V
      if (meas.voltage >= 1.0f) {
        targetAmps = _setpoints.power / meas.voltage;
      } else {
        targetAmps = 0.0f;
      }
      break;

    case MODE_CR:
      // Konstantní odpor: I = U / R
      if (_setpoints.resistance >= 0.1f) {
        targetAmps = meas.voltage / _setpoints.resistance;
      } else {
        targetAmps = 0.0f;
      }
      break;

    case MODE_BATT:
      // Test kapacity akumulátoru: Vybíjení konstantním proudem
      targetAmps = _setpoints.current;

      // Integrace kapacity a energie: dt v hodinách = dt_s / 3600
      if (dt_s > 0.0f && !_battStats.testCompleted) {
        float dt_hours = dt_s / 3600.0f;
        _battStats.discharged_mAh += (meas.current * 1000.0f * dt_hours);
        _battStats.discharged_Wh += (meas.power * dt_hours);
      }

      // Kontrola dosažení vypínacího napětí (Cut-off) s filtrací šumu (10 vzorků = 200 ms)
      if (meas.voltage <= _setpoints.cutoffVoltage && meas.voltage > 0.5f) {
        _cutoffLowSamples++;
      } else {
        _cutoffLowSamples = 0;
      }

      if (_cutoffLowSamples >= 10) {
        _enabled = false;
        _battStats.testCompleted = true;
        targetAmps = 0.0f;
      }
      break;
  }

  // Softwarové omezení výkonu (Power Derating):
  // Zabraňuje tomu, aby regulátor v režimech CC/CR/CP při vyšším napětí
  // vybudil MOSFET nad bezpečný jmenovitý výkon (62 W) a shodil havarijní OPP (65 W).
  const float maxSafePower = LIMIT_MAX_POWER - 3.0f; // 62.0 W provozní strop
  if (meas.voltage > 1.0f && (targetAmps * meas.voltage) > maxSafePower) {
    targetAmps = maxSafePower / meas.voltage;
  }

  return constrain(targetAmps, 0.0f, LIMIT_MAX_CURRENT);
}
