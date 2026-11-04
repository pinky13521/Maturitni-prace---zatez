/**
 * @file wifi_manager.cpp
 * @brief Implementace konfigurace Wi-Fi (Klient STA s automatickým záložním AP)
 */

#include "net/wifi_manager.h"
#include "config.h"
#include <WiFi.h>
#include <ESPmDNS.h>

WifiManager::WifiManager() : _active(false), _isAp(false) {}

bool WifiManager::begin() {
  WiFi.setSleep(false); // Vypnout úsporný režim Wi-Fi modemu (sníží latenci z 200 ms na ~2 ms)

  // 1. Pokus o připojení v režimu Klienta (STA) k zadané Wi-Fi síti
  if (strlen(WIFI_STA_SSID) > 0 && strcmp(WIFI_STA_SSID, "Vase-WiFi-Nazev") != 0) {
    LOG_PRINTF("[NET] Pripojuji k Wi-Fi siti (STA): '%s' ...\n", WIFI_STA_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_STA_SSID, WIFI_STA_PASS);

    unsigned long startMs = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startMs < WIFI_STA_TIMEOUT_MS)) {
      delay(200);
#if SERIAL_DEBUG
      Serial.print(".");
#endif
    }
#if SERIAL_DEBUG
    Serial.println();
#endif

    if (WiFi.status() == WL_CONNECTED) {
      _active = true;
      _isAp = false;
      _ip = WiFi.localIP();
      _activeSsid = WIFI_STA_SSID;

      // Spuštění mDNS (pro přístup přes http://zatez.local)
      if (MDNS.begin(MDNS_HOSTNAME)) {
        MDNS.addService("http", "tcp", WEB_SERVER_PORT);
      }
      return true;
    } else {
      LOG_PRINTLN("[NET] Pripojeni k STA selhalo (vyprsel casovy limit). Prepinam na zalozni AP...");
    }
  }

  // 2. Záložní režim Access Point (AP), pokud STA není nakonfigurováno nebo selhalo spojení
  return beginAp();
}

bool WifiManager::beginAp() {
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);
  bool ok = WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  if (ok) {
    _active = true;
    _isAp = true;
    _ip = WiFi.softAPIP();
    _activeSsid = WIFI_AP_SSID;

    if (MDNS.begin(MDNS_HOSTNAME)) {
      MDNS.addService("http", "tcp", WEB_SERVER_PORT);
    }
  }
  return ok;
}

bool WifiManager::isConnected() const {
  return _active;
}

bool WifiManager::isAp() const {
  return _isAp;
}

IPAddress WifiManager::getIp() const {
  return _ip;
}

String WifiManager::getIpStr() const {
  return _ip.toString();
}

const char* WifiManager::getSsid() const {
  return _activeSsid.c_str();
}
