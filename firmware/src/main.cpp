/**
 * @file main.cpp
 * @brief Hlavní program číslicově řízené elektronické stejnosměrné zátěže
 * @project Programovatelná stejnosměrná elektronická zátěž 30V / 5A
 * @author Ondřej Pinkas
 * 
 * Architektura:
 * - Modulární C++ s oddělenou abstrakcí hardwaru (HAL), bezpečnostním dohledem
 *   a deterministickou regulační smyčkou.
 */

#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "types.h"
#include "hal/adc_driver.h"
#include "hal/dac_driver.h"
#include "hal/fan_driver.h"
#include "hal/encoder_driver.h"
#include "hal/temp_driver.h"
#include "core/safety.h"
#include "core/load_regulator.h"
#include "ui/display.h"
#include "net/wifi_manager.h"
#include "net/web_server.h"

// Instance periferií a řídicích modulů
static AdcDriver        s_adc;
static DacDriver        s_dac;
static FanDriver        s_fan;
static EncoderDriver    s_encoder;
static TempDriver       s_temp;
static SafetySupervisor s_safety;
static LoadRegulator    s_regulator;
static DisplayUi        s_ui;
static WifiManager      s_wifi;

// Globální měřené veličiny a stav
static Measurement s_meas = { 0.0f, 0.0f, 0.0f, 25.0f };
static SystemState s_state = {
  .mode = MODE_CC,
  .outputEnabled = false,
  .alarm = ALARM_NONE,
  .alarmText = "",
  .fanDutyPercent = 20
};

static LoadWebServer    s_webServer(s_meas, s_state, s_regulator, s_safety);

// Časovače smyček (Deterministický cooperative scheduler)
static unsigned long s_lastFastLoop = 0;
static unsigned long s_lastUiLoop   = 0;
static unsigned long s_lastSlowLoop = 0;

/**
 * @brief Synchronizuje globální stavovou strukturu s_state s řídicími moduly
 */
static void syncSystemState() {
  s_state.mode = s_regulator.getMode();
  s_state.outputEnabled = s_regulator.isEnabled();
  s_state.alarm = s_safety.getType();
  s_state.alarmText = s_safety.getReason();
  s_state.fanDutyPercent = s_fan.getSpeedPercent();
}

void setup() {
#if SERIAL_DEBUG
  Serial.begin(115200);
  delay(300);
  Serial.println("\n==================================================");
  Serial.println("  PROGRAMOVATELNA STEJNOSMERNA ELEKTRONICKA ZATEZ");
  Serial.println("  Maturitni projekt - Ondrej Pinkas");
  Serial.println("==================================================");
#endif

  // 1. Inicializace I2C sběrnice na pinech navržených v KiCadu
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, I2C_FREQ);

  // 2. Inicializace HAL ovladačů
  bool oledOk = s_ui.begin();
  LOG_PRINTF("[HAL] OLED SSD1306 (0x%02X): %s\n", I2C_ADDR_OLED, oledOk ? "OK" : "NENALEZEN");

  bool adsOk = s_adc.begin();
  LOG_PRINTF("[HAL] ADC ADS1115  (0x%02X): %s\n", I2C_ADDR_ADS1115, adsOk ? "OK" : "NENALEZEN");

  bool dacOk = s_dac.begin();
  LOG_PRINTF("[HAL] DAC MCP4725  (0x%02X): %s\n", I2C_ADDR_MCP4725, dacOk ? "OK" : "NENALEZEN");

  bool tempOk = s_temp.begin();
  LOG_PRINTF("[HAL] Dallas DS18B20 (1-Wire): %s\n", tempOk ? "OK" : "NENALEZEN");

  s_fan.begin();
  LOG_PRINTLN("[HAL] 25 kHz PWM Ventilator: AKTIVNI");

  s_encoder.begin();
  LOG_PRINTLN("[HAL] Rotacni enkoder s prerusenim: AKTIVNI");

  // 3. Inicializace Wi-Fi (Režim STA - Klient s automatickým fallbackem na AP)
  if (s_wifi.begin()) {
    if (s_wifi.isAp()) {
      LOG_PRINTF("[NET] Wi-Fi AP zalozni rezim spusten: SSID '%s'\n", s_wifi.getSsid());
    } else {
      LOG_PRINTF("[NET] Wi-Fi pripojeno v rezimu Klienta (STA) k: '%s'\n", s_wifi.getSsid());
    }
    LOG_PRINT("[NET] IP adresa: ");
    LOG_PRINTLN(s_wifi.getIp());
    LOG_PRINTF("[NET] Webovy dashboard dostupny na: http://%s.local nebo http://", MDNS_HOSTNAME);
    LOG_PRINTLN(s_wifi.getIp());

    s_ui.showNetworkInfo(s_wifi.getIpStr().c_str(), s_wifi.isAp());
    s_webServer.begin();
  } else {
    LOG_PRINTLN("[NET] CHYBA: Spusteni Wi-Fi selhalo!");
  }

  LOG_PRINTLN("--------------------------------------------------");
  LOG_PRINTLN("System inicializovan. Bezi v bezpecnem stavu (Vystup OFF).");
  LOG_PRINTLN("==================================================\n");
}

void loop() {
  unsigned long now = millis();

  // 0. Obsluha HTTP požadavků webového serveru a asynchronní obsluha ADC
  s_webServer.handle();
  s_adc.poll();

  // ===========================================================================
  // 1. OBSLUHA UŽIVATELSKÉHO VSTUPU (Rotační enkodér a tlačítko)
  // ===========================================================================
  int delta = s_encoder.readDelta();
  if (delta != 0) {
    Setpoint& sp = s_regulator.getSetpoints();

    switch (s_regulator.getMode()) {
      case MODE_CC:
      case MODE_BATT:
        sp.current += (delta * 0.050f); // Krok 50 mA
        sp.current = constrain(sp.current, 0.0f, LIMIT_MAX_CURRENT);
        LOG_PRINTF("[SETPOINT] Proud: %5.3f A\n", sp.current);
        break;

      case MODE_CP:
        sp.power += (delta * 1.0f);     // Krok 1 W
        sp.power = constrain(sp.power, 0.0f, LIMIT_MAX_POWER);
        LOG_PRINTF("[SETPOINT] Vykon: %4.1f W\n", sp.power);
        break;

      case MODE_CR:
        sp.resistance += (delta * 0.5f);// Krok 0.5 Ohm
        sp.resistance = constrain(sp.resistance, 0.5f, 500.0f);
        LOG_PRINTF("[SETPOINT] Odpor: %4.1f Ohm\n", sp.resistance);
        break;
    }
  }

  // Krátký stisk tlačítka: Potvrzení alarmu nebo zapnutí/vypnutí zátěže (ON/OFF)
  if (s_encoder.readButtonClicked()) {
    if (s_safety.isTripped()) {
      // Potvrzení a reset bezpečnostního alarmu
      s_safety.reset();
      s_regulator.enable(false);
      s_dac.shutdown();
      LOG_PRINTLN("[SAFETY] Havarijni alarm resetovan uzivatelem.");
    } else {
      // Přepnutí stavu zátěže (ZAP / VYP)
      bool newState = !s_regulator.isEnabled();
      s_regulator.enable(newState);
      if (!newState) {
        s_dac.shutdown();
      }
      LOG_PRINTF("[LOAD] Vystup zateze: %s\n", newState ? "ZAPNUTO" : "VYPNUTO");
    }
    syncSystemState();
  }

  // Dlouhý stisk tlačítka (>= 600 ms): Cyklické přepnutí provozního režimu (CC -> CP -> CR -> BATT)
  if (s_encoder.readButtonLongPressed()) {
    LoadMode nextMode = (LoadMode)(((int)s_regulator.getMode() + 1) % 4);
    s_regulator.setMode(nextMode);
    s_dac.shutdown();
    syncSystemState();

    const char* modeNames[] = { "CC (Konstantni proud)", "CP (Konstantni vykon)", "CR (Konstantni odpor)", "BATT (Test baterie)" };
    LOG_PRINTF("[MODE] Prepnuto na rezim: %s (Vystup VYPNUT)\n", modeNames[(int)nextMode]);
  }

  // ===========================================================================
  // 2. RYCHLÁ REGULAČNÍ A BEZPEČNOSTNÍ SMYČKA (50 Hz / perioda 20 ms)
  // ===========================================================================
  if (now - s_lastFastLoop >= 20) {
    float dt = (now - s_lastFastLoop) / 1000.0f;
    s_lastFastLoop = now;

    // Čtení senzorů z ADC (pokud není ADC připojen, drží se bezpečné nuly)
    s_adc.read(s_meas.voltage, s_meas.current);
    s_meas.power = s_meas.voltage * s_meas.current;

    // Kontrola bezpečnostních ochran (OVP, OCP, OPP, OTP)
    if (!s_safety.check(s_meas)) {
      s_regulator.enable(false);
      s_dac.shutdown();
    }

    // Výpočet žádaného proudu zátěže
    float targetCurrent = s_regulator.update(s_meas, dt);

    // Zápis do DAC (pokud je výstup povolen a není aktivní ochrana)
    if (s_regulator.isEnabled() && !s_safety.isTripped()) {
      s_dac.setCurrent(targetCurrent);
    } else {
      s_dac.shutdown();
    }

    syncSystemState();
  }

  // ===========================================================================
  // 3. ZOBRAZOVACÍ SMYČKA UI (6.6 Hz / perioda 150 ms - šetří I2C pásmo pro ADC/DAC)
  // ===========================================================================
  if (now - s_lastUiLoop >= 150) {
    s_lastUiLoop = now;
    syncSystemState();
    s_ui.render(s_meas, s_state, s_regulator.getSetpoints(), s_regulator.getBatteryStats());
  }

  // ===========================================================================
  // 4. POMALÁ DIAGNOSTICKÁ A TEPELNÁ SMYČKA (1 Hz / perioda 1000 ms)
  // ===========================================================================
  if (now - s_lastSlowLoop >= 1000) {
    s_lastSlowLoop = now;

    // Čtení teploty a příprava na další převod
    s_meas.temperature = s_temp.read();
    s_temp.requestAsync();

    // Přepočet otáček ventilátoru
    s_fan.update(s_meas.temperature, s_safety.isTripped());

    // Diagnostický telemetrický výpis do konzole
    LOG_PRINTF("U: %5.2f V | I: %5.3f A | P: %4.1f W | T: %4.1f C | Fan: %3d%% | Out: %d\n",
               s_meas.voltage, s_meas.current, s_meas.power, 
               s_meas.temperature, s_fan.getSpeedPercent(), s_regulator.isEnabled());
  }
}
