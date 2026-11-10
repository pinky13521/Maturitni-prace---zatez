/**
 * @file load_regulator.h
 * @brief Algoritmy regulace CC, CP, CR a integrace kapacity akumulátoru
 */

#ifndef LOAD_REGULATOR_H
#define LOAD_REGULATOR_H

#include "types.h"

class LoadRegulator {
public:
  LoadRegulator();

  /**
   * @brief Vypočte cílový proud do analogové smyčky podle zvoleného režimu
   * @param meas Aktuální naměřené veličiny
   * @param dt_s Časový krok od minulého volání [s]
   * @return Požadovaný proud v ampérech
   */
  float update(const Measurement& meas, float dt_s);

  void setMode(LoadMode mode);
  LoadMode getMode() const;

  void enable(bool en);
  bool isEnabled() const;

  Setpoint& getSetpoints();
  const BatteryStats& getBatteryStats() const;
  void resetBatteryStats();

private:
  LoadMode _mode;
  bool _enabled;
  int _cutoffLowSamples;
  Setpoint _setpoints;
  BatteryStats _battStats;
  double _acc_mAh;
  double _acc_Wh;
  float _ocvVoltage;
  bool _riCalculated;
};

#endif // LOAD_REGULATOR_H
