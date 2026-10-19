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

// I2C Sběrnice (OLED 128x64, ADS1115, MCP4725)
#define PIN_I2C_SDA         17    // J_ESP_R1 pin 7 (TX2)
#define PIN_I2C_SCL          2    // J_ESP_R1 pin 4 (D2)
#define I2C_FREQ        400000    // 400 kHz (Fast Mode)

// I2C Adresy modulů
#define I2C_ADDR_OLED     0x3C    // Výchozí adresa 0.96" SSD1306 displeje
#define I2C_ADDR_ADS1115  0x48    // Výchozí adresa 16-bit ADC (ADDR -> GND)
#define I2C_ADDR_MCP4725  0x60    // Výchozí adresa 12-bit DAC (A0 -> GND, případně 0x61)

// Rotační enkodér s tlačítkem
#define PIN_ENC_CLK         25    // J_ESP_L1 pin 9 (D25)
#define PIN_ENC_DT          26    // J_ESP_L1 pin 10 (D26)
#define PIN_ENC_SW          27    // J_ESP_L1 pin 11 (D27)

// PWM řízení 4pinového ventilátoru
#define PIN_FAN_PWM         18    // J_ESP_R1 pin 9 (D18)
#define FAN_PWM_CHANNEL      0    // LEDC PWM kanál
#define FAN_PWM_FREQ     25000    // 25 kHz (standardní specifikace pro 4-pin PC ventilátory)
#define FAN_PWM_RES          8    // 8-bitové rozlišení (0 až 255)

// 1-Wire sběrnice pro teplotní senzor Dallas DS18B20
#define PIN_ONE_WIRE         1    // J_ESP_R1 pin 13 (TX0)

// =============================================================================
// 2. HARDWAROVÉ A KALIBRAČNÍ KONSTANTY
// =============================================================================

// Proudový bočník (R_shunt1 = 0.1 Ohm / 10W)
#define SHUNT_RESISTANCE_OHM 0.100f

// Napěťový dělič pro měření vstupního napětí (R_div1 = 100k, R_div2 = 10k)
// Teoretický dělicí poměr: (100k + 10k) / 10k = 11.00
#define VOLTAGE_DIVIDER_RATIO 11.00f

// D/A Převodník MCP4725 (12-bit: 0..4095)
#define DAC_VREF_VOLTS        3.300f // Skutečné napájecí napětí DAC (lze zpřesnit změřením 3V3 větve)
#define DAC_MAX_VALUE         4095

// =============================================================================
// 3. BEZPEČNOSTNÍ LIMITY A OCHRANY (SAFETY LIMITS)
// =============================================================================

#define LIMIT_MAX_VOLTAGE     32.0f  // [V] OVP - Přepěťová ochrana
#define LIMIT_MAX_CURRENT      5.2f  // [A] OCP - Nadproudová ochrana
#define LIMIT_MAX_POWER       65.0f  // [W] OPP - Trvalá výkonová ochrana
#define LIMIT_MAX_TEMP        75.0f  // [°C] OTP - Tepelná ochrana (nouzové vypnutí zátěže)
#define LIMIT_WARN_TEMP       65.0f  // [°C] Výstraha přehřátí

// Teplotní křivka pro ventilátor
#define TEMP_FAN_MIN          35.0f  // [°C] Pod touto teplotou ventilátor běží na minimum (20%)
#define TEMP_FAN_MAX          60.0f  // [°C] Nad touto teplotou běží ventilátor na 100%

#endif // CONFIG_H
