/**
 * @file load_regulator.cpp
 * @brief Implementace regulačních režimů a integrátorů kapacity
 */

#include "core/load_regulator.h"
#include "config.h"

LoadRegulator::LoadRegulator() 
  : _mode(MODE_CC),
    _enabled(false),
    _cutoffLowSamples(0),
    _acc_mAh(0.0),
    _acc_Wh(0.0),
    _ocvVoltage(0.0f),
    _riCalculated(false) {
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
    _riCalculated = false;
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
  _acc_mAh = 0.0;
  _acc_Wh = 0.0;
  _battStats.discharged_mAh = 0.0f;
  _battStats.discharged_Wh = 0.0f;
  _battStats.testStartTime = 0;
  _battStats.durationSeconds = 0;
  _battStats.internalResistance = 0.0f;
  _battStats.testCompleted = false;
  _cutoffLowSamples = 0;
  _riCalculated = false;
}

float LoadRegulator::update(const Measurement& meas, float dt_s) {
  if (!_enabled) {
    // V klidu před testem baterie sledujeme napětí naprázdno (OCV)
    if (_mode == MODE_BATT && !_riCalculated) {
      _ocvVoltage = meas.voltage;
    }
    return 0.0f;
  }

  float targetAmps = 0.0f;

  switch (_mode) {
    case MODE_CC:
      targetAmps = _setpoints.current;
      break;

    case MODE_CP:
      if (meas.voltage >= 1.0f) {
        targetAmps = _setpoints.power / meas.voltage;
      } else {
        targetAmps = 0.0f;
      }
      break;

    case MODE_CR:
      if (_setpoints.resistance >= 0.1f) {
        targetAmps = meas.voltage / _setpoints.resistance;
      } else {
        targetAmps = 0.0f;
      }
      break;

    case MODE_BATT:
      targetAmps = _setpoints.current;

      if (dt_s > 0.0f && !_battStats.testCompleted) {
        // Přesná 64-bitová integrace pro zamezení zaokrouhlovacích chyb při dlouhých testech
        double dt_hours = (double)dt_s / 3600.0;
        _acc_mAh += ((double)meas.current * 1000.0 * dt_hours);
        _acc_Wh += ((double)meas.power * dt_hours);

        _battStats.discharged_mAh = (float)_acc_mAh;
        _battStats.discharged_Wh = (float)_acc_Wh;

        if (_battStats.testStartTime > 0) {
          _battStats.durationSeconds = (millis() - _battStats.testStartTime) / 1000;
        }

        // Výpočet vnitřního odporu článku Ri = Delta_U / I po ustálení zátěže (cca 1 s od startu)
        if (!_riCalculated && _battStats.durationSeconds >= 1 && meas.current > 0.1f && _ocvVoltage > meas.voltage) {
          _battStats.internalResistance = (_ocvVoltage - meas.voltage) / meas.current;
          _riCalculated = true;
        }
      }

      // Ochrana před podvybitím (Cut-off) s 5 vzorky filtru (100 ms)
      if (meas.voltage <= _setpoints.cutoffVoltage && meas.voltage > 0.5f) {
        _cutoffLowSamples++;
      } else {
        _cutoffLowSamples = 0;
      }

      if (_cutoffLowSamples >= 5) {
        _enabled = false;
        _battStats.testCompleted = true;
        targetAmps = 0.0f;
      }
      break;
  }

  // Softwarové omezení výkonu (Power Derating):
  // Zabraňuje vybuzení tranzistoru nad bezpečný jmenovitý limit
  const float maxSafePower = LIMIT_MAX_POWER - 3.0f; // 62 W strop
  if (meas.voltage > 1.0f && (targetAmps * meas.voltage) > maxSafePower) {
    targetAmps = maxSafePower / meas.voltage;
  }

  return constrain(targetAmps, 0.0f, LIMIT_MAX_CURRENT);
}
