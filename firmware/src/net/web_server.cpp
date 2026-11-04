/**
 * @file web_server.cpp
 * @brief Implementace HTTP API serveru
 */

#include "net/web_server.h"
#include "net/web_dashboard_html.h"
#include "config.h"

LoadWebServer::LoadWebServer(const Measurement& meas, 
                             const SystemState& state, 
                             LoadRegulator& regulator, 
                             SafetySupervisor& safety)
  : _server(WEB_SERVER_PORT),
    _meas(meas),
    _state(state),
    _regulator(regulator),
    _safety(safety) {}

void LoadWebServer::begin() {
  _server.on("/", HTTP_GET, [this]() { handleRoot(); });
  _server.on("/api/telemetry", HTTP_GET, [this]() { handleTelemetry(); });
  _server.on("/api/control", HTTP_GET, [this]() { handleControl(); });

  _server.begin();
}

void LoadWebServer::handle() {
  _server.handleClient();
}

void LoadWebServer::handleRoot() {
  _server.send_P(200, "text/html", DASHBOARD_HTML);
}

void LoadWebServer::handleTelemetry() {
  const BatteryStats& batt = _regulator.getBatteryStats();
  const Setpoint& sp = _regulator.getSetpoints();

  float activeSp = sp.current;
  if (_regulator.getMode() == MODE_CP) {
    activeSp = sp.power;
  } else if (_regulator.getMode() == MODE_CR) {
    activeSp = sp.resistance;
  }

  char json[410];
  snprintf(json, sizeof(json),
    "{\"voltage\":%.2f,\"current\":%.3f,\"power\":%.1f,\"temperature\":%.1f,"
    "\"fan\":%d,\"output\":%s,\"alarm\":%d,\"alarmText\":\"%s\",\"mode\":%d,"
    "\"setpoint\":%.3f,\"cutoff\":%.2f,\"discharged_mAh\":%.1f,\"discharged_Wh\":%.2f}",
    _meas.voltage, _meas.current, _meas.power, _meas.temperature,
    _state.fanDutyPercent,
    _regulator.isEnabled() ? "true" : "false",
    (int)_safety.getType(),
    _safety.getReason() ? _safety.getReason() : "",
    (int)_regulator.getMode(),
    activeSp,
    sp.cutoffVoltage,
    batt.discharged_mAh, batt.discharged_Wh
  );

  _server.sendHeader("Connection", "close");
  _server.send(200, "application/json", json);
}

void LoadWebServer::handleControl() {
  // Ovládání výstupu (Zapnout / Vypnout)
  if (_server.hasArg("out")) {
    bool enable = (_server.arg("out") == "1");
    if (enable && !_safety.isTripped()) {
      _regulator.enable(true);
    } else {
      _regulator.enable(false);
    }
  }

  // Reset bezpečnostního alarmu
  if (_server.hasArg("resetAlarm")) {
    _safety.reset();
    _regulator.enable(false);
  }

  // Změna režimu
  if (_server.hasArg("mode")) {
    int m = _server.arg("mode").toInt();
    if (m >= 0 && m <= 3) {
      _regulator.setMode((LoadMode)m);
    }
  }

  // Změna žádané hodnoty
  if (_server.hasArg("setpoint")) {
    float val = _server.arg("setpoint").toFloat();
    Setpoint& sp = _regulator.getSetpoints();
    switch (_regulator.getMode()) {
      case MODE_CC:
      case MODE_BATT:
        sp.current = constrain(val, 0.0f, LIMIT_MAX_CURRENT);
        break;
      case MODE_CP:
        sp.power = constrain(val, 0.0f, LIMIT_MAX_POWER);
        break;
      case MODE_CR:
        sp.resistance = constrain(val, 0.5f, 500.0f);
        break;
    }
  }

  // Nastavení vypínacího napětí pro test baterie (Cut-off)
  if (_server.hasArg("cut")) {
    float cut = _server.arg("cut").toFloat();
    _regulator.getSetpoints().cutoffVoltage = constrain(cut, 0.5f, 30.0f);
  }

  // Okamžitě vrátit nový stav v jednom požadavku (ušetří druhý HTTP round-trip)
  handleTelemetry();
}
