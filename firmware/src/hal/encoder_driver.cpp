/**
 * @file encoder_driver.cpp
 * @brief Implementace přerušení enkodéru a stavového automatu tlačítka
 */

#include "hal/encoder_driver.h"
#include "config.h"

static EncoderDriver* s_instance = nullptr;
static portMUX_TYPE s_SubMux = portMUX_INITIALIZER_UNLOCKED;

static volatile int s_delta = 0;
static volatile unsigned long s_lastClkUs = 0;

static void IRAM_ATTR isrClk() {
  if (s_instance) s_instance->handleClkInterrupt();
}

EncoderDriver::EncoderDriver()
  : _btnLastRaw(HIGH),
    _btnStableState(HIGH),
    _longPressFired(false),
    _shortClickEvent(false),
    _longPressEvent(false),
    _lastDebounceTime(0),
    _pressStartTime(0) {
  s_instance = this;
}

void EncoderDriver::begin() {
  pinMode(PIN_ENC_CLK, INPUT_PULLUP);
  pinMode(PIN_ENC_DT, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_ENC_CLK), isrClk, CHANGE);
}

void IRAM_ATTR EncoderDriver::handleClkInterrupt() {
  unsigned long nowUs = micros();
  // Odfiltrování zákmitů kontaktů (debounce 1000 us = 1 ms)
  if (nowUs - s_lastClkUs < 1000) {
    return;
  }
  s_lastClkUs = nowUs;

  int clk = digitalRead(PIN_ENC_CLK);
  int dt = digitalRead(PIN_ENC_DT);

  portENTER_CRITICAL_ISR(&s_SubMux);
  if (clk != dt) {
    s_delta = s_delta + 1;
  } else {
    s_delta = s_delta - 1;
  }
  portEXIT_CRITICAL_ISR(&s_SubMux);
}

int EncoderDriver::readDelta() {
  portENTER_CRITICAL(&s_SubMux);
  int d = s_delta;
  s_delta = 0;
  portEXIT_CRITICAL(&s_SubMux);
  return d;
}

void EncoderDriver::pollButton() {
  bool raw = digitalRead(PIN_ENC_SW);
  unsigned long now = millis();

  if (raw != _btnLastRaw) {
    _lastDebounceTime = now;
    _btnLastRaw = raw;
  }

  // Stabilizace stavu po 30 ms
  if ((now - _lastDebounceTime) >= 30) {
    if (raw != _btnStableState) {
      _btnStableState = raw;

      if (_btnStableState == LOW) {
        // Tlačítko právě stisknuto
        _pressStartTime = now;
        _longPressFired = false;
      } else {
        // Tlačítko uvolněno -> pokud nebyl aktivován dlouhý stisk, vyhodnoť jako krátký klik
        if (!_longPressFired) {
          _shortClickEvent = true;
        }
      }
    }

    // Detekce dlouhého podržení (>= 600 ms) ještě během držení
    if (_btnStableState == LOW && !_longPressFired && (now - _pressStartTime >= 600)) {
      _longPressFired = true;
      _longPressEvent = true;
    }
  }
}

bool EncoderDriver::readButtonClicked() {
  pollButton();
  if (_shortClickEvent) {
    _shortClickEvent = false;
    return true;
  }
  return false;
}

bool EncoderDriver::readButtonLongPressed() {
  pollButton();
  if (_longPressEvent) {
    _longPressEvent = false;
    return true;
  }
  return false;
}
