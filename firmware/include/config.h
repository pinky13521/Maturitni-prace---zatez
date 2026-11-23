/**
 * @file config.h
 * @brief Konfigurace pinů, kalibračních konstant a bezpečnostních limitů
 * @project Programovatelná stejnosměrná elektronická zátěž
 * @author Ondřej Pinkas
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =============================================================================
// 1. PŘIŘAZENÍ PINŮ (Dle KiCad schématu pro ESP32 DevKit V1 30-pin)
// =============================================================================

// I2C Sběrnice (OLED 128x64, ADS1115, MCP4725) - ESP32 USB-C konektorem nahoru
#define PIN_I2C_SDA 26  // J_ESP_R1 pin 7 (GPIO26 / D26)
#define PIN_I2C_SCL 14  // J_ESP_R1 pin 5 (GPIO14 / D14)
#define I2C_FREQ 400000 // 400 kHz (Fast Mode)

// I2C Adresy modulů
#define I2C_ADDR_OLED 0x3C    // Výchozí adresa 0.96" SSD1306 displeje
#define I2C_ADDR_ADS1115 0x48 // Výchozí adresa 16-bit ADC (ADDR -> GND)
#define I2C_ADDR_MCP4725 0x60 // Výchozí adresa 12-bit DAC (A0 -> GND)

// Kanály ADS1115 (Dle KiCad schématu: J_ADC1 pin 7 = A0 -> IMON, pin 8 = A1 ->
// VMON)
#define ADC_CHANNEL_IMON 0 // Kanál A0: Snímání proudu bočníkem (IMON)
#define ADC_CHANNEL_VMON 1 // Kanál A1: Snímání napětí děličem (VMON)

// Rotační enkodér s tlačítkem
#define PIN_ENC_CLK 18 // J_ESP_L1 pin 9 (GPIO18 / D18)
#define PIN_ENC_DT 19  // J_ESP_L1 pin 10 (GPIO19 / D19)
#define PIN_ENC_SW 21  // J_ESP_L1 pin 11 (GPIO21 / D21)

// PWM řízení 4pinového ventilátoru
#define PIN_FAN_PWM 33     // J_ESP_R1 pin 9 (GPIO33 / D33)
#define FAN_PWM_CHANNEL 0  // LEDC kanál (pro Core 2.x)
#define FAN_PWM_FREQ 25000 // 25 kHz (standard pro 4-pin PC ventilátory)
#define FAN_PWM_RES 8      // 8-bitové rozlišení (0 až 255)

// 1-Wire sběrnice pro teplotní senzor Dallas DS18B20
#define PIN_ONE_WIRE 32 // J_ESP_R1 pin 10 (GPIO32 / D32)

#define SERIAL_DEBUG 1 // 1 = ladicí výpisy do Serial monitoru, 0 = vypnuto

#if SERIAL_DEBUG
#define LOG_PRINTLN(x) Serial.println(x)
#define LOG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define LOG_PRINT(x) Serial.print(x)
#else
#define LOG_PRINTLN(x)
#define LOG_PRINTF(...)
#define LOG_PRINT(x)
#endif

// =============================================================================
// 2. HARDWAROVÉ A KALIBRAČNÍ KONSTANTY
// =============================================================================

// Proudový bočník (R_shunt1 = 0.1 Ohm / 10W Kelvinovo čtyřvodičové zapojení)
#define SHUNT_RESISTANCE_OHM 0.100f

// Napěťový dělič vstupu (R_div1 = 100k, R_div2 = 10k) => poměr (100+10)/10
// = 11.0
#define VOLTAGE_DIVIDER_RATIO 11.00f

// D/A Převodník MCP4725 (12-bit: 0..4095)
#define DAC_VREF_VOLTS 3.300f // Skutečná napájecí větev 3.3V
#define DAC_MAX_VALUE 4095

// =============================================================================
// 3. BEZPEČNOSTNÍ LIMITY (SAFETY SUPERVISOR)
// =============================================================================

#define LIMIT_MAX_VOLTAGE 32.0f // [V] OVP - Přepěťová ochrana
#define LIMIT_MAX_CURRENT 5.2f  // [A] OCP - Nadproudová ochrana
#define LIMIT_MAX_POWER 65.0f   // [W] OPP - Maximální ztrátový výkon
#define LIMIT_MAX_TEMP 75.0f    // [°C] OTP - Havarijní vypnutí zátěže
#define LIMIT_WARN_TEMP 65.0f   // [°C] Výstražná teplota (ventilátor 100%)

// Křivka ventilátoru
#define TEMP_FAN_MIN 32.0f // [°C] Pod touto teplotou minimální otáčky (20%)
#define TEMP_FAN_MAX 60.0f // [°C] Nad touto teplotou plné otáčky (100%)

// =============================================================================
// 4. WI-FI A WEBOVÝ SERVER
// =============================================================================

// Režim Klienta (STA) - ESP32 se připojí k vaší domácí / školní Wi-Fi síti nebo
// hotspotu. Skutečné údaje uložte do souboru include/secrets.h (viz secrets.h.example),
// který je v .gitignore a nenahrává se na GitHub:
#if __has_include("secrets.h")
  #include "secrets.h"
#else
  #define WIFI_STA_SSID "Vase-WiFi-Nazev"  // Zadejte název své Wi-Fi sítě
  #define WIFI_STA_PASS "Vase-WiFi-Heslo"  // Zadejte heslo k Wi-Fi
#endif
#define WIFI_STA_TIMEOUT_MS 9000 // Časový limit pro připojení k routeru [ms]

// Záložní režim Access Point (AP) - Spustí se automaticky, pokud se nepodaří
// připojit k STA:
#define WIFI_AP_SSID "ESP32-Elektronicka-Zatez"
#define WIFI_AP_PASS "12345678"
#define WEB_SERVER_PORT 80

// Lokální mDNS adresa - V prohlížeči otevřete přímo: http://zatez.local
#define MDNS_HOSTNAME "zatez"

// =============================================================================
// 5. MQTT PROPOJENÍ S LOKÁLNÍM SERVEREM (VOLITELNÉ)
// =============================================================================
// Přepněte na 1, jakmile rozchodíte Mosquitto broker na vašem serveru.
// Při 0 běží pouze vestavěný lokální webový server na ESP32.
#define MQTT_ENABLED             0
#define MQTT_BROKER_IP           "192.168.31.251"
#define MQTT_BROKER_PORT         1883
#define MQTT_CLIENT_ID           "esp32-zatez"
#define MQTT_PUBLISH_INTERVAL_MS 200
#define MQTT_TOPIC_TELEMETRY     "zatez/telemetry"
#define MQTT_TOPIC_CONTROL       "zatez/control"
#define MQTT_TOPIC_STATUS        "zatez/status"

#endif // CONFIG_H
