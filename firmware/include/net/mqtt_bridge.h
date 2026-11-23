/**
 * @file mqtt_bridge.h
 * @brief Volitelný MQTT klient pro propojení s lokálním serverem (Mosquitto)
 */

#ifndef MQTT_BRIDGE_H
#define MQTT_BRIDGE_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "types.h"
#include "core/load_regulator.h"
#include "core/safety.h"

class MqttBridge {
public:
  MqttBridge(const Measurement& meas,
             const SystemState& state,
             LoadRegulator& regulator,
             SafetySupervisor& safety);

  void begin();
  void loop();
  bool isConnected();

private:
  WiFiClient _wifiClient;
  PubSubClient _client;
  const Measurement& _meas;
  const SystemState& _state;
  LoadRegulator& _regulator;
  SafetySupervisor& _safety;

  unsigned long _lastReconnectMs;
  unsigned long _lastPublishMs;

  void publishTelemetry();
  void handleMessage(char* topic, byte* payload, unsigned int length);
};

#endif // MQTT_BRIDGE_H
