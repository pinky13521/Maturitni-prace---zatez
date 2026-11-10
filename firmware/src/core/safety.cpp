/**
 * @file safety.cpp
 * @brief Implementace bezpečnostního dohledu s potlačením falešných alarmů
 */

#include "core/safety.h"
#include "config.h"

SafetySupervisor::SafetySupervisor() 
  : _alarm(ALARM_NONE), _reason(""), _ovpCount(0), _ocpCount(0), _oppCount(0), _otpCount(0) {}

bool SafetySupervisor::check(const Measurement& meas) {
  if (_alarm != ALARM_NONE) {
    return false; // Už je ve stavu alarmu
  }

  // Okamžitá havarijní ochrana při hrubém přepětí (> 34 V)
  if (meas.voltage > (LIMIT_MAX_VOLTAGE + 2.0f)) {
    trip(ALARM_OVP, "OVP: Kriticke prepeti na vstupu!");
    return false;
  }

  // 1. OVP - Přepěťová ochrana (potvrzení 2 po sobě jdoucí vzorky = 40 ms)
  if (meas.voltage > LIMIT_MAX_VOLTAGE) {
    if (++_ovpCount >= 2) {
      trip(ALARM_OVP, "OVP: Prepeti na vstupu!");
      return false;
    }
  } else {
    _ovpCount = 0;
  }

  // 2. OCP - Nadproudová ochrana (potvrzení 2 vzorky = 40 ms)
  if (meas.current > LIMIT_MAX_CURRENT) {
    if (++_ocpCount >= 2) {
      trip(ALARM_OCP, "OCP: Nadproud zateze!");
      return false;
    }
  } else {
    _ocpCount = 0;
  }

  // 3. OPP - Výkonová ochrana tranzistoru (3 vzorky = 60 ms pro toleranci náběhu)
  if (meas.power > LIMIT_MAX_POWER) {
    if (++_oppCount >= 3) {
      trip(ALARM_OPP, "OPP: Prekrocen max vykon!");
      return false;
    }
  } else {
    _oppCount = 0;
  }

  // 4. OTP - Tepelná ochrana chladiče (2 vzorky = 40 ms)
  if (meas.temperature > LIMIT_MAX_TEMP) {
    if (++_otpCount >= 2) {
      trip(ALARM_OTP, "OTP: Prehrati chladice!");
      return false;
    }
  } else {
    _otpCount = 0;
  }

  return true;
}

void SafetySupervisor::trip(AlarmType type, const char* reason) {
  _alarm = type;
  _reason = reason;
}

void SafetySupervisor::reset() {
  _alarm = ALARM_NONE;
  _reason = "";
  _ovpCount = 0;
  _ocpCount = 0;
  _oppCount = 0;
  _otpCount = 0;
}

bool SafetySupervisor::isTripped() const {
  return _alarm != ALARM_NONE;
}

AlarmType SafetySupervisor::getType() const {
  return _alarm;
}

const char* SafetySupervisor::getReason() const {
  return _reason;
}
