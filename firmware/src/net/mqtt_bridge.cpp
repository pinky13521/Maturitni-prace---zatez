/**
 * @file mqtt_bridge.cpp
 * @brief Implementace MQTT klienta pro telemetrii a vzdálené řízení ze serveru
 */

#include "net/mqtt_bridge.h"
#include "config.h"

MqttBridge::MqttBridge(const Measurement& meas,
                       const SystemState& state,
                       LoadRegulator& regulator,
                       SafetySupervisor& safety)
  : _client(_wifiClient),
    _meas(meas),
    _state(state),
    _regulator(regulator),
    _safety(safety),
    _lastReconnectMs(0),
    _lastPublishMs(0) {}

void MqttBridge::begin() {
  _client.setServer(MQTT_BROKER_IP, MQTT_BROKER_PORT);
  _client.setBufferSize(640);
  _client.setCallback([this](char* topic, byte* payload, unsigned int length) {
    handleMessage(topic, payload, length);
  });
}

bool MqttBridge::isConnected() {
  return _client.connected();
}

void MqttBridge::loop() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  unsigned long now = millis();

  if (!_client.connected()) {
    // Neblokující pokus o znovupřipojení každých 5 sekund
    if (now - _lastReconnectMs >= 5000) {
      _lastReconnectMs = now;
      // LWT (Last Will and Testament): při výpadku ESP32 označí broker zátěž jako offline
      if (_client.connect(MQTT_CLIENT_ID, nullptr, nullptr,
                          MQTT_TOPIC_STATUS, 1, true, "offline")) {
        _client.publish(MQTT_TOPIC_STATUS, "online", true);
        _client.subscribe(MQTT_TOPIC_CONTROL);
        LOG_PRINTLN("[MQTT] Pripojeno k brokeru na serveru.");
      }
    }
    return;
  }

  _client.loop();

  if (now - _lastPublishMs >= MQTT_PUBLISH_INTERVAL_MS) {
    _lastPublishMs = now;
    publishTelemetry();
  }
}

void MqttBridge::publishTelemetry() {
  const BatteryStats& batt = _regulator.getBatteryStats();
  const Setpoint& sp = _regulator.getSetpoints();

  float activeSp = sp.current;
  if (_regulator.getMode() == MODE_CP) {
    activeSp = sp.power;
  } else if (_regulator.getMode() == MODE_CR) {
    activeSp = sp.resistance;
  }

  char json[512];
  snprintf(json, sizeof(json),
    "{\"voltage\":%.2f,\"current\":%.3f,\"power\":%.1f,\"temperature\":%.1f,"
    "\"fan\":%d,\"output\":%s,\"alarm\":%d,\"alarmText\":\"%s\",\"mode\":%d,"
    "\"setpoint\":%.3f,\"cutoff\":%.2f,\"discharged_mAh\":%.1f,\"discharged_Wh\":%.2f,"
    "\"duration\":%lu,\"ri\":%.3f,\"heap\":%lu,\"uptime\":%lu}",
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
    (unsigned long)(millis() / 1000)
  );

  _client.publish(MQTT_TOPIC_TELEMETRY, json);
}

void MqttBridge::handleMessage(char* topic, byte* payload, unsigned int length) {
  (void)topic;
  char buf[96];
  unsigned int copyLen = (length < sizeof(buf) - 1) ? length : (sizeof(buf) - 1);
  memcpy(buf, payload, copyLen);
  buf[copyLen] = '\0';

  if (strcmp(buf, "resetAlarm=1") == 0) {
    _safety.reset();
    _regulator.enable(false);
  } else if (strncmp(buf, "mode=", 5) == 0) {
    int m = atoi(buf + 5);
    if (m >= 0 && m <= 3) {
      _regulator.setMode((LoadMode)m);
    }
  } else if (strncmp(buf, "setpoint=", 9) == 0) {
    float val = atof(buf + 9);
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
  } else if (strncmp(buf, "cut=", 4) == 0) {
    float cut = atof(buf + 4);
    _regulator.getSetpoints().cutoffVoltage = constrain(cut, 0.5f, 30.0f);
  } else if (strncmp(buf, "out=", 4) == 0) {
    bool en = (buf[4] == '1');
    _regulator.enable(en && !_safety.isTripped());
  } else if (strncmp(buf, "batt=", 5) == 0) {
    const char* cmd = buf + 5;
    if (strcmp(cmd, "start") == 0 && !_safety.isTripped()) {
      _regulator.setMode(MODE_BATT);
      _regulator.enable(true);
    } else if (strcmp(cmd, "stop") == 0) {
      _regulator.enable(false);
    } else if (strcmp(cmd, "reset") == 0) {
      _regulator.enable(false);
      _regulator.resetBatteryStats();
    }
  }
}
