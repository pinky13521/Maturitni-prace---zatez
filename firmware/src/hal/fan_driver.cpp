/**
 * @file fan_driver.cpp
 * @brief Implementace 25 kHz PWM řízení ventilátoru
 */

#include "hal/fan_driver.h"
#include "config.h"
#include <esp_arduino_version.h>

FanDriver::FanDriver() : _speedPercent(20) {}

void FanDriver::begin() {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  ledcAttach(PIN_FAN_PWM, FAN_PWM_FREQ, FAN_PWM_RES);
#else
  ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQ, FAN_PWM_RES);
  ledcAttachPin(PIN_FAN_PWM, FAN_PWM_CHANNEL);
#endif

  update(25.0f, false);
}

void FanDriver::update(float currentTemp, bool isAlarm) {
  if (isAlarm || currentTemp >= LIMIT_WARN_TEMP) {
    _speedPercent = 100; // Při jakémkoliv alarmu nebo zvýšené teplotě chladíme naplno
  } else if (currentTemp <= TEMP_FAN_MIN) {
    _speedPercent = 20; // Minimální otáčky pro trvalé ofukování
  } else if (currentTemp >= TEMP_FAN_MAX) {
    _speedPercent = 100;
  } else {
    // Lineární interpolace mezi TEMP_FAN_MIN a TEMP_FAN_MAX
    float factor = (currentTemp - TEMP_FAN_MIN) / (TEMP_FAN_MAX - TEMP_FAN_MIN);
    _speedPercent = (uint8_t)(20.0f + factor * 80.0f);
  }

  uint8_t duty = (uint8_t)map(_speedPercent, 0, 100, 0, 255);

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  ledcWrite(PIN_FAN_PWM, duty);
#else
  ledcWrite(FAN_PWM_CHANNEL, duty);
#endif
}

uint8_t FanDriver::getSpeedPercent() const {
  return _speedPercent;
}
