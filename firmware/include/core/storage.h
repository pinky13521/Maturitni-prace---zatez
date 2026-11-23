/**
 * @file storage.h
 * @brief Správa nevolatilní paměti NVS (Preferences) pro kalibraci a nastavení
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include "types.h"

class NvsStorage {
public:
  NvsStorage();

  /**
   * @brief Načte kalibrační koeficienty a poslední žádané hodnoty z NVS
   */
  void load(CalibrationData& cal, Setpoint& sp);

  /**
   * @brief Uloží kalibrační koeficienty do NVS
   */
  void saveCalibration(const CalibrationData& cal);

  /**
   * @brief Uloží aktuální žádané hodnoty (setpoints) do NVS
   */
  void saveSetpoints(const Setpoint& sp);

  /**
   * @brief Obnoví výchozí tovární kalibraci (1.000 / 0.000)
   */
  void resetCalibration(CalibrationData& cal);
};

#endif // STORAGE_H
