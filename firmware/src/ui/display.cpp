/**
 * @file display.cpp
 * @brief Implementace vykreslování obrazovky zátěže
 */

#include "ui/display.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 s_display(128, 64, &Wire, -1);

DisplayUi::DisplayUi() : _ready(false) {}

bool DisplayUi::begin() {
  _ready = s_display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR_OLED);
  if (_ready) {
    s_display.clearDisplay();
    s_display.setTextColor(SSD1306_WHITE);
    s_display.setTextSize(1);
    s_display.setCursor(15, 20);
    s_display.println("ELEKTRONICKA");
    s_display.setCursor(30, 35);
    s_display.println("ZATEZ 30V/5A");
    s_display.display();
  }
  return _ready;
}

bool DisplayUi::isConnected() const {
  return _ready;
}

void DisplayUi::showNetworkInfo(const char* ipStr, bool isAp) {
  if (!_ready) return;

  s_display.clearDisplay();
  s_display.setTextColor(SSD1306_WHITE);
  s_display.setTextSize(1);
  s_display.setCursor(0, 0);
  s_display.println("ELEKTRONICKA ZATEZ");
  s_display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

  s_display.setCursor(0, 16);
  s_display.printf("SIT: %s\n", isAp ? "AP (Vlastni)" : "STA (Router)");

  s_display.setCursor(0, 30);
  s_display.printf("IP: %s\n", ipStr);

  s_display.setCursor(0, 48);
  s_display.println("http://zatez.local");

  s_display.display();
  delay(1800);
}

void DisplayUi::render(const Measurement& meas, 
                       const SystemState& state, 
                       const Setpoint& setpoint, 
                       const BatteryStats& batt) {
  if (!_ready) return;

  s_display.clearDisplay();
  s_display.setTextColor(SSD1306_WHITE);

  // 1. Horní stavový řádek
  s_display.setTextSize(1);
  s_display.setCursor(0, 0);

  switch (state.mode) {
    case MODE_CC:   s_display.print("CC "); break;
    case MODE_CP:   s_display.print("CP "); break;
    case MODE_CR:   s_display.print("CR "); break;
    case MODE_BATT: s_display.print("BAT"); break;
  }

  s_display.setCursor(35, 0);
  if (state.alarm != ALARM_NONE) {
    s_display.print("!ALARM!");
  } else if (state.outputEnabled) {
    s_display.print("[ON]");
  } else {
    s_display.print("[OFF]");
  }

  s_display.setCursor(85, 0);
  s_display.printf("%2.0fC %d%%", meas.temperature, state.fanDutyPercent);
  s_display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

  // 2. Havarijní zobrazení nebo normální provoz
  if (state.alarm != ALARM_NONE) {
    s_display.setTextSize(1);
    s_display.setCursor(0, 22);
    s_display.println("VYP-BEZPECNOSTNI STOP");
    s_display.setCursor(0, 38);
    s_display.println(state.alarmText);
  } else {
    // Napětí (2x velikost písma)
    s_display.setTextSize(2);
    s_display.setCursor(0, 14);
    s_display.printf("%5.2fV", meas.voltage);

    // Proud (2x velikost písma)
    s_display.setCursor(0, 32);
    s_display.printf("%5.3fA", meas.current);

    // 3. Dolní řádek: Výkon a žádaná hodnota / kapacita
    s_display.drawLine(0, 50, 127, 50, SSD1306_WHITE);
    s_display.setTextSize(1);
    s_display.setCursor(0, 54);

    if (state.mode == MODE_BATT) {
      uint32_t sec = batt.durationSeconds;
      uint32_t mm = sec / 60;
      uint32_t ss = sec % 60;
      if (mm > 99) {
        s_display.printf("%dmAh %02dh%02d", (int)batt.discharged_mAh, (int)(mm / 60), (int)(mm % 60));
      } else {
        s_display.printf("%dmAh  %02d:%02d", (int)batt.discharged_mAh, (int)mm, (int)ss);
      }
    } else {
      float targetVal = (state.mode == MODE_CC) ? setpoint.current :
                        ((state.mode == MODE_CP) ? setpoint.power : setpoint.resistance);
      s_display.printf("%4.1fW  SET:%4.2f", meas.power, targetVal);
    }
  }

  s_display.display();
}
