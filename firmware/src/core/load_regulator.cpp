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
    _lastSweepStepMs(0),
    _sweepCurrentAmps(0.0f),
    _piTrimAmps(0.0f),
    _acc_mAh(0.0),
    _acc_Wh(0.0),
    _ocvVoltage(0.0f),
    _riCalculated(false) {
  _setpoints.current = 0.500f;      // Výchozí: 500 mA
  _setpoints.power = 10.0f;         // Výchozí: 10 W
  _setpoints.resistance = 10.0f;    // Výchozí: 10 Ω
  _setpoints.cutoffVoltage = 3.0f;  // Výchozí cut-off mez pro Li-Ion

  _sweepStats = { false, false, 0.0f, 0.0f, 0.0f, 0.0f };
  resetBatteryStats();
}

void LoadRegulator::setMode(LoadMode mode) {
  if (_mode != mode) {
    // Z bezpečnostních důvodů při změně režimu vždy odpojíme zátěž,
    // aby nedošlo k proudovému rázu při přechodu mezi CC / CP / CR.
    _enabled = false;
    _sweepStats.active = false;
    _piTrimAmps = 0.0f;
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
  if (!_enabled) {
    _sweepStats.active = false;
    _piTrimAmps = 0.0f;
  } else if (_mode == MODE_BATT) {
    resetBatteryStats();
    _battStats.testStartTime = millis();
    _riCalculated = false;
  }
}

void LoadRegulator::startSweep() {
  _mode = MODE_CC;
  _sweepStats.active = true;
  _sweepStats.completed = false;
  _sweepStats.openCircuitVoltage = _ocvVoltage;
  _sweepStats.maxCurrent = 0.0f;
  _sweepStats.maxPower = 0.0f;
  _sweepStats.sourceResistance = 0.0f;
  _sweepCurrentAmps = 0.050f; // Začínáme od 50 mA
  _setpoints.current = _sweepCurrentAmps;
  _lastSweepStepMs = millis();
  _piTrimAmps = 0.0f;
  _enabled = true;
}

void LoadRegulator::stopSweep() {
  _sweepStats.active = false;
  _enabled = false;
  _piTrimAmps = 0.0f;
}

const SweepStats& LoadRegulator::getSweepStats() const {
  return _sweepStats;
}

float LoadRegulator::getPiTrimAmps() const {
  return _piTrimAmps;
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
    // V klidu před testem baterie či sweepem sledujeme napětí naprázdno (OCV)
    if (meas.voltage > 0.1f) {
      _ocvVoltage = meas.voltage;
    }
    _piTrimAmps = 0.0f;
    return 0.0f;
  }

  // Automatický V-A Sweep test zdroje (plynulé zvyšování proudu a detekce OCP)
  if (_sweepStats.active) {
    if (_sweepStats.openCircuitVoltage < 0.2f && meas.voltage > 0.2f) {
      _sweepStats.openCircuitVoltage = meas.voltage;
    }
    if (meas.current > _sweepStats.maxCurrent) {
      _sweepStats.maxCurrent = meas.current;
    }
    if (meas.power > _sweepStats.maxPower) {
      _sweepStats.maxPower = meas.power;
    }
    if (meas.current > 0.15f && _sweepStats.openCircuitVoltage > meas.voltage) {
      _sweepStats.sourceResistance = (_sweepStats.openCircuitVoltage - meas.voltage) / meas.current;
    }

    unsigned long now = millis();
    if (now - _lastSweepStepMs >= 250) {
      _lastSweepStepMs = now;
      // Ukončení sweepu: pokles napětí o >25 % (nebo pod 1.0 V), dosažení proudového limitu zdroje (OCP), nebo stropu 60 W / 5 A
      bool vCollapse = (_sweepStats.openCircuitVoltage > 1.0f && meas.voltage < (_sweepStats.openCircuitVoltage * 0.75f)) || (meas.voltage < 0.8f);
      bool currentStall = (_sweepCurrentAmps >= 0.40f && meas.current < (_sweepCurrentAmps - 0.25f));
      bool maxReached = (_sweepCurrentAmps >= 5.0f || (meas.voltage * _sweepCurrentAmps) >= 60.0f);

      if (vCollapse || currentStall || maxReached) {
        _sweepStats.active = false;
        _sweepStats.completed = true;
        _enabled = false;
        _piTrimAmps = 0.0f;
        return 0.0f;
      }

      _sweepCurrentAmps += 0.100f; // Krok +100 mA každých 250 ms
      _setpoints.current = _sweepCurrentAmps;
    }
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

      // Ochrana před podvybitím (Cut-off) s 10 vzorky filtru (200 ms)
      // Zachytí i náhlé odpojení BMS na 0 V
      if (meas.voltage <= _setpoints.cutoffVoltage) {
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
  // Zabraňuje vybuzení tranzistoru nad bezpečný jmenovitý limit
  const float maxSafePower = LIMIT_MAX_POWER - 3.0f; // 62 W strop
  if (meas.voltage > 1.0f && (targetAmps * meas.voltage) > maxSafePower) {
    targetAmps = maxSafePower / meas.voltage;
  }

  targetAmps = constrain(targetAmps, 0.0f, LIMIT_MAX_CURRENT);

  // Digitální PI kompenzace vstupního ofsetu LM358 a kolísání 3.3V reference:
  // Rychlou odezvu (<1 us) drží analogová smyčka LM358, zatímco pomalý integrátor
  // podle přesného 16bitového ADS1115 jemně dolaďuje výstup DAC v rozmezí +/- 150 mA.
  if (targetAmps >= 0.020f && meas.voltage >= 0.5f && dt_s > 0.0f && dt_s < 0.2f) {
    float err = targetAmps - meas.current;
    // Pokud vůbec neteče proud (odpojený zdroj), nenabíjíme integrátor (anti-windup)
    if (meas.current < 0.005f && targetAmps > 0.100f) {
      _piTrimAmps *= 0.90f;
    } else {
      _piTrimAmps += err * 0.65f * dt_s;
      _piTrimAmps = constrain(_piTrimAmps, -0.150f, 0.150f);
    }
  } else {
    _piTrimAmps = 0.0f;
  }

  return constrain(targetAmps + _piTrimAmps, 0.0f, LIMIT_MAX_CURRENT);
}
