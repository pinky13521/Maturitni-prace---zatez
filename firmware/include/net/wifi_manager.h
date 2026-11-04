/**
 * @file wifi_manager.h
 * @brief Správce Wi-Fi konektivity (Access Point / Station)
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <IPAddress.h>

class WifiManager {
public:
  WifiManager();
  bool begin();
  bool beginAp();
  bool isConnected() const;
  bool isAp() const;
  IPAddress getIp() const;
  String getIpStr() const;
  const char* getSsid() const;

private:
  bool _active;
  bool _isAp;
  IPAddress _ip;
  String _activeSsid;
};

#endif // WIFI_MANAGER_H
