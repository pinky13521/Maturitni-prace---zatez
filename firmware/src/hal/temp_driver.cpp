/**
 * @file temp_driver.cpp
 * @brief Implementace neblokujícího čtení z DS18B20
 */

#include "hal/temp_driver.h"
#include "config.h"
#include <OneWire.h>
#include <DallasTemperature.h>

static OneWire s_oneWire(PIN_ONE_WIRE);
static DallasTemperature s_sensors(&s_oneWire);

TempDriver::TempDriver() : _ready(false), _lastValidTemp(25.0f), _failCount(0) {}

bool TempDriver::begin() {
  s_sensors.begin();
  s_sensors.setWaitForConversion(false); // Neblokovat cyklus čekáním na AD převod
  if (s_sensors.getDeviceCount() > 0) {
    _ready = true;
    requestAsync();
  }
  return _ready;
}

bool TempDriver::isConnected() const {
  return _ready;
}

void TempDriver::requestAsync() {
  if (_ready) {
    s_sensors.requestTemperatures();
  }
}

float TempDriver::read() {
  if (!_ready) return _lastValidTemp;

  float t = s_sensors.getTempCByIndex(0);
  // Platné teplotní rozmezí čidla DS18B20 (-55 °C až +125 °C, chyba odpojení vrací -127 °C)
  if (t > -50.0f && t < 125.0f) {
    _lastValidTemp = t;
    _failCount = 0;
  } else {
    if (_failCount < 255) _failCount++;
    // Pokud čidlo 5x za sebou neodpoví (5 sekund výpadku),
    // vrátíme havarijní teplotu nad limit OTP (85 °C) pro okamžité bezpečné odstavení zátěže.
    if (_failCount >= 5) {
      _lastValidTemp = 85.0f;
    }
  }
  return _lastValidTemp;
}
