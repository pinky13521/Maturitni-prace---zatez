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
                             SafetySupervisor& safety,
                             CalibrationData& cal,
                             NvsStorage& storage,
                             AdcDriver& adc,
                             DacDriver& dac)
  : _server(WEB_SERVER_PORT),
    _meas(meas),
    _state(state),
    _regulator(regulator),
    _safety(safety),
    _cal(cal),
    _storage(storage),
    _adc(adc),
    _dac(dac) {}

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
  const SweepStats& sw = _regulator.getSweepStats();
  const Setpoint& sp = _regulator.getSetpoints();

  float activeSp = sp.current;
  if (_regulator.getMode() == MODE_CP) {
    activeSp = sp.power;
  } else if (_regulator.getMode() == MODE_CR) {
    activeSp = sp.resistance;
  }

  char json[768];
  snprintf(json, sizeof(json),
    "{\"voltage\":%.2f,\"current\":%.3f,\"power\":%.1f,\"temperature\":%.1f,"
    "\"fan\":%d,\"output\":%s,\"alarm\":%d,\"alarmText\":\"%s\",\"mode\":%d,"
    "\"setpoint\":%.3f,\"cutoff\":%.2f,\"discharged_mAh\":%.1f,\"discharged_Wh\":%.2f,"
    "\"duration\":%lu,\"ri\":%.3f,\"heap\":%lu,\"uptime\":%lu,"
    "\"piTrim\":%.3f,\"sweepActive\":%s,\"sweepDone\":%s,\"sweepU0\":%.2f,"
    "\"sweepMaxI\":%.3f,\"sweepMaxP\":%.1f,\"sweepR\":%.3f,"
    "\"calV\":%.4f,\"calI\":%.4f,\"calDac\":%.4f,\"calOff\":%.4f}",
    _meas.voltage, _meas.current, _meas.power, _meas.temperature,
    _state.fanDutyPercent,
    _regulator.isEnabled() ? "true" : "false",
    (int)_safety.getType(),
    _safety.getReason() ? _safety.getReason() : "",
    (int)_regulator.getMode(),
    activeSp,
    sp.cutoffVoltage,
    batt.discharged_mAh, batt.discharged_Wh,
    (unsigned long)batt.durationSeconds,
    batt.internalResistance,
    (unsigned long)ESP.getFreeHeap(),
    (unsigned long)(millis() / 1000),
    _regulator.getPiTrimAmps(),
    sw.active ? "true" : "false",
    sw.completed ? "true" : "false",
    sw.openCircuitVoltage,
    sw.maxCurrent,
    sw.maxPower,
    sw.sourceResistance,
    _cal.vScale, _cal.iScale, _cal.dacScale, _cal.iOffset
  );

  _server.sendHeader("Connection", "close");
  _server.send(200, "application/json", json);
}

void LoadWebServer::handleControl() {
  // 1. Reset bezpečnostního alarmu (musí proběhnout před zapnutím výstupu)
  if (_server.hasArg("resetAlarm")) {
    _safety.reset();
    _regulator.enable(false);
  }

  // 2. Změna režimu (setMode z bezpečnostních důvodů vypíná výstup, proto předchází 'out')
  if (_server.hasArg("mode")) {
    int m = _server.arg("mode").toInt();
    if (m >= 0 && m <= 3) {
      _regulator.setMode((LoadMode)m);
    }
  }

  // 3. Změna žádané hodnoty
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

  // 4. Nastavení vypínacího napětí pro test baterie (Cut-off)
  if (_server.hasArg("cut")) {
    float cut = _server.arg("cut").toFloat();
    _regulator.getSetpoints().cutoffVoltage = constrain(cut, 0.5f, 30.0f);
  }

  // 5. Ovládání výstupu (Zapnout / Vypnout)
  if (_server.hasArg("out")) {
    bool enable = (_server.arg("out") == "1");
    if (enable && !_safety.isTripped()) {
      _regulator.enable(true);
    } else {
      _regulator.enable(false);
    }
  }

  // 6. Příkaz pro test baterie (start, stop, reset)
  if (_server.hasArg("batt")) {
    String cmd = _server.arg("batt");
    if (cmd == "start" && !_safety.isTripped()) {
      _regulator.setMode(MODE_BATT);
      _regulator.enable(true);
    } else if (cmd == "stop") {
      _regulator.enable(false);
    } else if (cmd == "reset") {
      _regulator.enable(false);
      _regulator.resetBatteryStats();
    }
  }

  // 7. Automatický V-A Sweep test zdroje (start / stop)
  if (_server.hasArg("sweep")) {
    String cmd = _server.arg("sweep");
    if (cmd == "start" && !_safety.isTripped()) {
      _regulator.startSweep();
    } else if (cmd == "stop") {
      _regulator.stopSweep();
    }
  }

  // 8. Softwarová kalibrace a uložení do NVS paměti
  bool calChanged = false;
  if (_server.hasArg("calV")) {
    _cal.vScale = constrain(_server.arg("calV").toFloat(), 0.80f, 1.20f);
    calChanged = true;
  }
  if (_server.hasArg("calI")) {
    _cal.iScale = constrain(_server.arg("calI").toFloat(), 0.80f, 1.20f);
    calChanged = true;
  }
  if (_server.hasArg("calDac")) {
    _cal.dacScale = constrain(_server.arg("calDac").toFloat(), 0.80f, 1.20f);
    calChanged = true;
  }
  if (_server.hasArg("calOff")) {
    _cal.iOffset = constrain(_server.arg("calOff").toFloat(), -0.20f, 0.20f);
    calChanged = true;
  }
  if (calChanged) {
    _adc.setCalibration(_cal);
    _dac.setDacScale(_cal.dacScale);
    _storage.saveCalibration(_cal);
  }
  if (_server.hasArg("resetCal")) {
    _storage.resetCalibration(_cal);
    _adc.setCalibration(_cal);
    _dac.setDacScale(_cal.dacScale);
  }
  if (_server.hasArg("saveSp")) {
    _storage.saveSetpoints(_regulator.getSetpoints());
  }

  // Okamžitě vrátit nový stav v jednom požadavku (ušetří druhý HTTP round-trip)
  handleTelemetry();
}
