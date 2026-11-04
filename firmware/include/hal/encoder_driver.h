/**
 * @file encoder_driver.h
 * @brief Ovladač rotačního enkodéru s tlačítkem
 */

#ifndef ENCODER_DRIVER_H
#define ENCODER_DRIVER_H

#include <Arduino.h>

class EncoderDriver {
public:
  EncoderDriver();
  void begin();

  /**
   * @brief Přečte a atomicky vynuluje nasčítaný počet pulzů z enkodéru
   */
  int readDelta();

  /**
   * @brief Vrátí true při krátkém stisku tlačítka (< 600 ms, zapnutí/vypnutí zátěže)
   */
  bool readButtonClicked();

  /**
   * @brief Vrátí true při dlouhém podržení tlačítka (>= 600 ms, přepnutí režimu CC/CP/CR/BATT)
   */
  bool readButtonLongPressed();

  // Obslužná rutina přerušení (volaná z ISR)
  void handleClkInterrupt();

private:
  void pollButton();
  bool _btnLastRaw;
  bool _btnStableState;
  bool _longPressFired;
  bool _shortClickEvent;
  bool _longPressEvent;
  unsigned long _lastDebounceTime;
  unsigned long _pressStartTime;
};

#endif // ENCODER_DRIVER_H
