/**
 * @file web_server.h
 * @brief HTTP REST API a webový server pro elektronickou zátěž
 */

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include "types.h"
#include "core/load_regulator.h"
#include "core/safety.h"
#include "core/storage.h"
#include "hal/adc_driver.h"
#include "hal/dac_driver.h"

class LoadWebServer {
public:
  LoadWebServer(const Measurement& meas, 
                const SystemState& state, 
                LoadRegulator& regulator, 
                SafetySupervisor& safety,
                CalibrationData& cal,
                NvsStorage& storage,
                AdcDriver& adc,
                DacDriver& dac);

  void begin();
  void handle();

private:
  WebServer _server;
  const Measurement& _meas;
  const SystemState& _state;
  LoadRegulator& _regulator;
  SafetySupervisor& _safety;
  CalibrationData& _cal;
  NvsStorage& _storage;
  AdcDriver& _adc;
  DacDriver& _dac;

  void handleRoot();
  void handleTelemetry();
  void handleControl();
};

#endif // WEB_SERVER_H
