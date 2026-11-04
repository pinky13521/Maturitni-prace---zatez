/**
 * @file display.h
 * @brief Uživatelské rozhraní pro 0.96" I2C OLED displej (SSD1306)
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include "types.h"

class DisplayUi {
public:
  DisplayUi();
  bool begin();
  bool isConnected() const;

  /**
   * @brief Zobrazí úvodní síťovou obrazovku s IP adresou a mDNS odkazem
   */
  void showNetworkInfo(const char* ipStr, bool isAp);

  /**
   * @brief Vykreslí aktuální stav zátěže, měřené hodnoty a případné alarmy
   */
  void render(const Measurement& meas, 
              const SystemState& state, 
              const Setpoint& setpoint, 
              const BatteryStats& batt);

private:
  bool _ready;
};

#endif // DISPLAY_H
