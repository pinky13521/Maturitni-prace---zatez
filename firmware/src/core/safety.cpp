/**
 * @file safety.cpp
 * @brief Implementace bezpečnostního dohledu
 */

#include "core/safety.h"
#include "config.h"

SafetySupervisor::SafetySupervisor() 
  : _alarm(ALARM_NONE), _reason("") {}

bool SafetySupervisor::check(const Measurement& meas) {
  if (_alarm != ALARM_NONE) {
    return false; // Už je ve stavu alarmu
  }

  // 1. OVP - Přepěťová ochrana
  if (meas.voltage > LIMIT_MAX_VOLTAGE) {
    trip(ALARM_OVP, "OVP: Prepeti na vstupu!");
    return false;
  }

  // 2. OCP - Nadproudová ochrana
  if (meas.current > LIMIT_MAX_CURRENT) {
    trip(ALARM_OCP, "OCP: Nadproud zateze!");
    return false;
  }

  // 3. OPP - Výkonová ochrana tranzistoru
  if (meas.power > LIMIT_MAX_POWER) {
    trip(ALARM_OPP, "OPP: Prekrocen max vykon!");
    return false;
  }

  // 4. OTP - Tepelná ochrana chladiče
  if (meas.temperature > LIMIT_MAX_TEMP) {
    trip(ALARM_OTP, "OTP: Prehrati chladice!");
    return false;
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
