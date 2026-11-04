/**
 * @file safety.h
 * @brief Bezpečnostní dohled (Safety Supervisor) zátěže
 */

#ifndef SAFETY_H
#define SAFETY_H

#include "types.h"

class SafetySupervisor {
public:
  SafetySupervisor();

  /**
   * @brief Zkontroluje mezní parametry. Při překročení okamžitě vybaví alarm.
   * @param meas Aktuální naměřené veličiny
   * @return true pokud je vše v normě, false při aktivaci ochrany
   */
  bool check(const Measurement& meas);

  /**
   * @brief Vyvolá havarijní alarm ručně nebo z jiného modulu
   */
  void trip(AlarmType type, const char* reason);

  /**
   * @brief Resetuje havarijní alarm po potvrzení uživatelem
   */
  void reset();

  bool isTripped() const;
  AlarmType getType() const;
  const char* getReason() const;

private:
  AlarmType _alarm;
  const char* _reason;
};

#endif // SAFETY_H
