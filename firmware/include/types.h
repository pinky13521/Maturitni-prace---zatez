/**
 * @file types.h
 * @brief Datové typy, výčty a stavové struktury elektronické zátěže
 */

#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

/**
 * @brief Pracovní režimy zátěže
 */
enum LoadMode {
  MODE_CC   = 0, // Konstantní proud (Constant Current)
  MODE_CP   = 1, // Konstantní výkon (Constant Power)
  MODE_CR   = 2, // Konstantní odpor (Constant Resistance)
  MODE_BATT = 3  // Měření kapacity akumulátoru (Battery Capacity Test)
};

/**
 * @brief Typ bezpečnostního alarmu
 */
enum AlarmType {
  ALARM_NONE = 0,
  ALARM_OVP  = 1, // Over-Voltage Protection (Přepětí)
  ALARM_OCP  = 2, // Over-Current Protection (Nadproud)
  ALARM_OPP  = 3, // Over-Power Protection (Přetížení výkonem)
  ALARM_OTP  = 4  // Over-Temperature Protection (Přehřátí chladiče)
};

/**
 * @brief Naměřené fyzikální veličiny ze senzorů
 */
struct Measurement {
  float voltage;      // [V] Skutečné svorkové napětí
  float current;      // [A] Skutečný proud bočníkem
  float power;        // [W] Vypočtený ztrátový výkon (U * I)
  float temperature;  // [°C] Teplota chladiče
};

/**
 * @brief Uživatelem nastavené žádané hodnoty
 */
struct Setpoint {
  float current;        // [A] Žádaný proud pro CC / BATT
  float power;          // [W] Žádaný výkon pro CP
  float resistance;     // [Ω] Žádaný odpor pro CR
  float cutoffVoltage;  // [V] Vypínací napětí pro test akumulátoru
};

/**
 * @brief Statistika vybíjení akumulátoru
 */
struct BatteryStats {
  float discharged_mAh;
  float discharged_Wh;
  uint32_t testStartTime;
  uint32_t durationSeconds;
  float internalResistance; // [Ω] Odhad vnitřního odporu článku
  bool testCompleted;
};

/**
 * @brief Kalibrační koeficienty uložené v NVS paměti ESP32
 */
struct CalibrationData {
  float vScale;   // Korekce měření napětí (výchozí 1.000)
  float iScale;   // Korekce měření proudu (výchozí 1.000)
  float dacScale; // Korekce buzení DAC    (výchozí 1.000)
  float iOffset;  // Nulový ofset proudu [A] (výchozí 0.000)
};

/**
 * @brief Výsledky automatického testu V-A charakteristiky a OCP limitu zdroje
 */
struct SweepStats {
  bool active;
  bool completed;
  float openCircuitVoltage; // [V] Napětí zdroje naprázdno (U0)
  float maxCurrent;         // [A] Zjištěný maximální proud zdroje (OCP)
  float maxPower;           // [W] Zjištěný maximální výkon zdroje
  float sourceResistance;   // [Ω] Vypočtený vnitřní odpor zdroje
};

/**
 * @brief Celkový stav systému
 */
struct SystemState {
  LoadMode mode;
  bool outputEnabled;
  AlarmType alarm;
  const char* alarmText;
  uint8_t fanDutyPercent;
};

#endif // TYPES_H
