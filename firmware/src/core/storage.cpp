/**
 * @file storage.cpp
 * @brief Implementace ukládání kalibrace a parametrů do Flash NVS v ESP32
 */

#include "core/storage.h"
#include "config.h"
#include <Preferences.h>

static const char* NVS_NAMESPACE = "zatez_cfg";

NvsStorage::NvsStorage() {}

void NvsStorage::load(CalibrationData& cal, Setpoint& sp) {
  Preferences prefs;
  if (!prefs.begin(NVS_NAMESPACE, true)) {
    // Pokud jmenný prostor ještě neexistuje, ponecháme výchozí hodnoty
    cal.vScale   = 1.000f;
    cal.iScale   = 1.000f;
    cal.dacScale = 1.000f;
    cal.iOffset  = 0.000f;
    return;
  }

  cal.vScale   = constrain(prefs.getFloat("vScale", 1.000f), 0.80f, 1.20f);
  cal.iScale   = constrain(prefs.getFloat("iScale", 1.000f), 0.80f, 1.20f);
  cal.dacScale = constrain(prefs.getFloat("dacScale", 1.000f), 0.80f, 1.20f);
  cal.iOffset  = constrain(prefs.getFloat("iOffset", 0.000f), -0.20f, 0.20f);

  sp.current       = constrain(prefs.getFloat("spCurr", sp.current), 0.0f, LIMIT_MAX_CURRENT);
  sp.power         = constrain(prefs.getFloat("spPow", sp.power), 0.0f, LIMIT_MAX_POWER);
  sp.resistance    = constrain(prefs.getFloat("spRes", sp.resistance), 0.5f, 500.0f);
  sp.cutoffVoltage = constrain(prefs.getFloat("spCut", sp.cutoffVoltage), 0.5f, 30.0f);

  prefs.end();
}

void NvsStorage::saveCalibration(const CalibrationData& cal) {
  Preferences prefs;
  if (prefs.begin(NVS_NAMESPACE, false)) {
    prefs.putFloat("vScale", cal.vScale);
    prefs.putFloat("iScale", cal.iScale);
    prefs.putFloat("dacScale", cal.dacScale);
    prefs.putFloat("iOffset", cal.iOffset);
    prefs.end();
  }
}

void NvsStorage::saveSetpoints(const Setpoint& sp) {
  Preferences prefs;
  if (prefs.begin(NVS_NAMESPACE, false)) {
    prefs.putFloat("spCurr", sp.current);
    prefs.putFloat("spPow", sp.power);
    prefs.putFloat("spRes", sp.resistance);
    prefs.putFloat("spCut", sp.cutoffVoltage);
    prefs.end();
  }
}

void NvsStorage::resetCalibration(CalibrationData& cal) {
  cal.vScale   = 1.000f;
  cal.iScale   = 1.000f;
  cal.dacScale = 1.000f;
  cal.iOffset  = 0.000f;
  saveCalibration(cal);
}
