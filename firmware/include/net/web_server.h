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

class LoadWebServer {
public:
  LoadWebServer(const Measurement& meas, 
                const SystemState& state, 
                LoadRegulator& regulator, 
                SafetySupervisor& safety);

  void begin();
  void handle();

private:
  WebServer _server;
  const Measurement& _meas;
  const SystemState& _state;
  LoadRegulator& _regulator;
  SafetySupervisor& _safety;

  void handleRoot();
  void handleTelemetry();
  void handleControl();
};

#endif // WEB_SERVER_H
