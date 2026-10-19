/**
 * @file firmware.ino
 * @brief Hlavní program pro programovatelnou elektronickou stejnosměrnou zátěž
 * @project Programovatelná stejnosměrná elektronická zátěž
 * @author Ondřej Pinkas
 * 
 * Využívá:
 * - ESP32 DevKit V1 (dvoujádrové řízení a FreeRTOS)
 * - 12-bit DAC MCP4725 (nastavení proudu zátěže)
 * - 16-bit ADC ADS1115 (přesné měření U a I)
 * - 0.96" OLED SSD1306 (I2C zobrazení měřených hodnot a stavů)
 * - Rotační enkodér KY-040 (nastavení hodnot a ovládání menu)
 * - Dallas DS18B20 (digitální snímání teploty chladiče)
 * - 4-pin PC PWM ventilátor (plynulá regulace otáček)
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_MCP4725.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// =============================================================================
// TYPY A STAVY
// =============================================================================

enum LoadMode {
  MODE_CC = 0,   // Konstantní proud (Constant Current)
  MODE_CP = 1,   // Konstantní výkon (Constant Power)
  MODE_CR = 2,   // Konstantní odpor (Constant Resistance)
  MODE_BATT = 3  // Měření kapacity akumulátoru (Battery Capacity Test)
};

struct SystemState {
  // Měřené fyzikální veličiny
  float voltage;            // [V] Skutečné vstupní napětí na svorkách zátěže
  float current;            // [A] Skutečný proud tekoucí zátěží
  float power;              // [W] Ztrátový výkon na tranzistoru a bočníku
  float temperature;        // [°C] Teplota chladiče z DS18B20
  int fanSpeed;             // [%] Otáčky ventilátoru (0 až 100 %)

  // Nastavené žádané hodnoty
  LoadMode mode;            // Aktuální pracovní režim
  float setpointCurrent;    // [A] Žádaný proud pro CC / BATT
  float setpointPower;      // [W] Žádaný výkon pro CP
  float setpointResistance; // [Ohm] Žádaný odpor pro CR
  float battCutoffVoltage;  // [V] Vypínací mez pro test akumulátoru

  // Integrátory pro test akumulátoru
  float discharged_mAh;     // [mAh] Celkový odebraný náboj
  float discharged_Wh;      // [Wh] Celková odebraná energie
  unsigned long testStartTime;// Čas startu testu (ms)

  // Řídicí stavy a ochrany
  bool outputEnabled;       // true = zátěž zapnuta, false = zátěž odpojena (0 A)
  bool alarmActive;         // true = vybavena bezpečnostní ochrana
  const char* alarmReason;  // Textový popis důvodu výstrahy (OVP, OCP, OPP, OTP)
};

// Globální instance periferií
Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_ADS1115 ads;
Adafruit_MCP4725 dac;
OneWire oneWire(PIN_ONE_WIRE);
DallasTemperature tempSensors(&oneWire);

// Globální stav systému
SystemState sys = {
  .voltage = 0.0f,
  .current = 0.0f,
  .power = 0.0f,
  .temperature = 25.0f,
  .fanSpeed = 20,
  .mode = MODE_CC,
  .setpointCurrent = 0.500f,   // Výchozí žádaný proud 500 mA
  .setpointPower = 10.0f,      // Výchozí výkon 10 W
  .setpointResistance = 10.0f, // Výchozí odpor 10 Ohm
  .battCutoffVoltage = 3.0f,   // Výchozí cut-off pro Li-Ion
  .discharged_mAh = 0.0f,
  .discharged_Wh = 0.0f,
  .testStartTime = 0,
  .outputEnabled = false,
  .alarmActive = false,
  .alarmReason = ""
};

// Proměnné pro obsluhu enkodéru
volatile int encoderDelta = 0;
volatile bool buttonPressed = false;
int lastClkState = HIGH;

// Časování smyček
unsigned long lastFastLoop = 0;
unsigned long lastDisplayLoop = 0;
unsigned long lastTempLoop = 0;
unsigned long lastBatteryTick = 0;

// =============================================================================
// OBSLUHA ENKODÉRU (Přerušení)
// =============================================================================

void IRAM_ATTR isrEncoder() {
  int clk = digitalRead(PIN_ENC_CLK);
  int dt = digitalRead(PIN_ENC_DT);
  if (clk != dt) {
    encoderDelta++;
  } else {
    encoderDelta--;
  }
}

void IRAM_ATTR isrButton() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200) { // Debounce 200 ms
    buttonPressed = true;
    lastInterruptTime = interruptTime;
  }
}

// =============================================================================
// NÍZKOÚROVŇOVÉ ŘÍZENÍ ZÁTĚŽE A DAC
// =============================================================================

/**
 * @brief Nastaví skutečný žádaný proud do analogové smyčky přes DAC
 * @param targetAmps Proud v ampérech (0.0 až LIMIT_MAX_CURRENT)
 */
void applyDacCurrent(float targetAmps) {
  if (!sys.outputEnabled || sys.alarmActive || targetAmps <= 0.0f) {
    dac.setVoltage(0, false);
    return;
  }

  // Omezení na bezpečný maximální proud
  if (targetAmps > LIMIT_MAX_CURRENT) {
    targetAmps = LIMIT_MAX_CURRENT;
  }

  // Přepočet: Napětí na bočníku V_shunt = I * R_shunt (při 5A a 0.1R je to 0.5V)
  float targetVshunt = targetAmps * SHUNT_RESISTANCE_OHM;

  // Přepočet napětí na 12-bit hodnotu DAC (0 až 4095 při referenci 3.3V)
  int dacCode = (int)((targetVshunt / DAC_VREF_VOLTS) * (float)DAC_MAX_VALUE);
  dacCode = constrain(dacCode, 0, DAC_MAX_VALUE);

  dac.setVoltage(dacCode, false);
}

/**
 * @brief Vypne výstup zátěže a nastaví chybový stav
 */
void triggerAlarm(const char* reason) {
  sys.outputEnabled = false;
  sys.alarmActive = true;
  sys.alarmReason = reason;
  applyDacCurrent(0.0f);
  Serial.print("ALARM ACTIVATED: ");
  Serial.println(reason);
}

// =============================================================================
// MĚŘENÍ ADC A BEZPEČNOSTNÍ KONTROLY
// =============================================================================

/**
 * @brief Čte hodnoty napětí a proudu z převodníku ADS1115
 */
void readAdcSensors() {
  // Kanál 0: Napětí zátěže (přes dělič 11:1)
  int16_t rawV = ads.readADC_SingleEnded(0);
  float voltsPinA0 = ads.computeVolts(rawV);
  sys.voltage = voltsPinA0 * VOLTAGE_DIVIDER_RATIO;
  if (sys.voltage < 0.05f) sys.voltage = 0.0f; // Odstranění šumu kolem nuly

  // Kanál 1: Proud zátěže (úbytek na bočníku R_shunt1)
  int16_t rawI = ads.readADC_SingleEnded(1);
  float voltsPinA1 = ads.computeVolts(rawI);
  sys.current = voltsPinA1 / SHUNT_RESISTANCE_OHM;
  if (sys.current < 0.005f) sys.current = 0.0f; // Odstranění šumu kolem nuly

  // Výpočet ztrátového výkonu
  sys.power = sys.voltage * sys.current;
}

/**
 * @brief Kontrola mezních bezpečnostních hodnot (OVP, OCP, OPP, OTP)
 */
void checkProtections() {
  if (sys.voltage > LIMIT_MAX_VOLTAGE) {
    triggerAlarm("OVP: Prepeti na vstupu!");
    return;
  }
  if (sys.current > LIMIT_MAX_CURRENT) {
    triggerAlarm("OCP: Nadproud zateze!");
    return;
  }
  if (sys.power > LIMIT_MAX_POWER) {
    triggerAlarm("OPP: Prekrocen vykon!");
    return;
  }
  if (sys.temperature > LIMIT_MAX_TEMP) {
    triggerAlarm("OTP: Prehrati chladice!");
    return;
  }
}

// =============================================================================
// ŘÍZENÍ VENTILÁTORU (Intel 4-pin 25 kHz PWM)
// =============================================================================

void updateFanControl() {
  // Bezpečnostní stav: při alarmu teploměru roztočit ventilátor na 100%
  if (sys.alarmActive && sys.temperature >= LIMIT_WARN_TEMP) {
    sys.fanSpeed = 100;
  } else if (sys.temperature <= TEMP_FAN_MIN) {
    sys.fanSpeed = 20; // Minimální tichý chod pro průchod vzduchu
  } else if (sys.temperature >= TEMP_FAN_MAX) {
    sys.fanSpeed = 100; // Maximální otáčky
  } else {
    // Plynulá lineární křivka mezi MIN a MAX
    sys.fanSpeed = 20 + (int)((sys.temperature - TEMP_FAN_MIN) * 80.0f / (TEMP_FAN_MAX - TEMP_FAN_MIN));
  }

  // Zápis do PWM (0 až 255)
  int duty = map(sys.fanSpeed, 0, 100, 0, 255);
  ledcWrite(FAN_PWM_CHANNEL, duty);
}

// =============================================================================
// VÝPOČET REGULAČNÍCH REŽIMŮ (CC, CP, CR, BATT)
// =============================================================================

void executeRegulationLoop() {
  if (!sys.outputEnabled || sys.alarmActive) {
    applyDacCurrent(0.0f);
    return;
  }

  float targetCurrent = 0.0f;

  switch (sys.mode) {
    case MODE_CC:
      targetCurrent = sys.setpointCurrent;
      break;

    case MODE_CP:
      // P = U * I  =>  I = P / U
      if (sys.voltage > 1.0f) {
        targetCurrent = sys.setpointPower / sys.voltage;
      } else {
        targetCurrent = 0.0f; // Pod 1V nelze udržet výkon
      }
      break;

    case MODE_CR:
      // U = R * I  =>  I = U / R
      if (sys.setpointResistance > 0.1f) {
        targetCurrent = sys.voltage / sys.setpointResistance;
      }
      break;

    case MODE_BATT:
      // Vybíjení zadaným proudem do dosažení mezního napětí
      targetCurrent = sys.setpointCurrent;
      if (sys.voltage <= sys.battCutoffVoltage && sys.voltage > 0.5f) {
        sys.outputEnabled = false; // Úspěšně dokončen test baterie
        applyDacCurrent(0.0f);
        Serial.println("BATTERY TEST FINISHED: Cut-off voltage reached!");
        return;
      }
      break;
  }

  applyDacCurrent(targetCurrent);
}

// =============================================================================
// GRAFICKÉ ZOBRAZENÍ NA OLED (128x64)
// =============================================================================

void renderOledDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // 1. HORNÍ STAVOVÝ ŘÁDEK (Režim | Stav | Teplota)
  display.setTextSize(1);
  display.setCursor(0, 0);
  switch (sys.mode) {
    case MODE_CC:   display.print("CC "); break;
    case MODE_CP:   display.print("CP "); break;
    case MODE_CR:   display.print("CR "); break;
    case MODE_BATT: display.print("BAT"); break;
  }

  display.setCursor(35, 0);
  if (sys.alarmActive) {
    display.print("!ALARM!");
  } else if (sys.outputEnabled) {
    display.print("[ON]");
  } else {
    display.print("[OFF]");
  }

  display.setCursor(85, 0);
  display.print((int)sys.temperature);
  display.print("C ");
  display.print(sys.fanSpeed);
  display.print("%");

  display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

  // 2. HLAVNÍ HODNOTY (Měřené U a I velkým písmem)
  if (sys.alarmActive) {
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("POZOR! VYPNUTO");
    display.setCursor(0, 35);
    display.println(sys.alarmReason);
  } else {
    // Napětí
    display.setTextSize(2);
    display.setCursor(0, 14);
    display.printf("%5.2fV", sys.voltage);

    // Proud
    display.setCursor(0, 32);
    display.printf("%5.3fA", sys.current);

    // 3. DOLNÍ ŘÁDEK (Výkon a žádaná hodnota)
    display.drawLine(0, 50, 127, 50, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 54);

    if (sys.mode == MODE_BATT) {
      display.printf("%dmAh  %4.1fWh", (int)sys.discharged_mAh, sys.discharged_Wh);
    } else {
      display.printf("%4.1fW  SET:%4.2f", sys.power, 
                     sys.mode == MODE_CC ? sys.setpointCurrent : 
                    (sys.mode == MODE_CP ? sys.setpointPower : sys.setpointResistance));
    }
  }

  display.display();
}

// =============================================================================
// SETUP A LOOP
// =============================================================================

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n--- Programovatelna DC zatez startuje ---");

  // Konfigurace pinů enkodéru
  pinMode(PIN_ENC_CLK, INPUT_PULLUP);
  pinMode(PIN_ENC_DT, INPUT_PULLUP);
  pinMode(PIN_ENC_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_CLK), isrEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_SW), isrButton, FALLING);

  // Inicializace PWM pro ventilátor
  ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQ, FAN_PWM_RES);
  ledcAttachPin(PIN_FAN_PWM, FAN_PWM_CHANNEL);
  updateFanControl();

  // Inicializace I2C sběrnice na pinech definovaných v KiCadu
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, I2C_FREQ);

  // Inicializace OLED displeje
  if (!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR_OLED)) {
    Serial.println("CHYBA: OLED displej nenalezen!");
  } else {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(15, 20);
    display.println("ELEKTRONICKA");
    display.setCursor(35, 35);
    display.println("ZATEZ 30V/5A");
    display.display();
  }

  // Inicializace ADS1115
  ads.setGain(GAIN_ONE); // Rozsah +/- 4.096 V (1 LSB = 0.125 mV)
  if (!ads.begin(I2C_ADDR_ADS1115)) {
    Serial.println("CHYBA: ADS1115 nenalezen!");
  } else {
    Serial.println("ADS1115 pripraven (16-bit).");
  }

  // Inicializace MCP4725
  if (!dac.begin(I2C_ADDR_MCP4725)) {
    Serial.println("CHYBA: MCP4725 nenalezen!");
  } else {
    dac.setVoltage(0, false); // Bezpečný start: výstup zátěže 0 V
    Serial.println("MCP4725 pripraven (12-bit).");
  }

  // Inicializace teplotního čidla DS18B20
  tempSensors.begin();
  tempSensors.setWaitForConversion(false); // Nečinné čekání – neblokovat smyčku!
  tempSensors.requestTemperatures();

  delay(1000);
  Serial.println("Inicializace dokoncena. Pripraveno k provozu.");
}

void loop() {
  unsigned long now = millis();

  // 1. ZPRACOVÁNÍ VSTUPU Z ENKODÉRU
  if (encoderDelta != 0) {
    float step = 0.05f; // Krok změny proudu (50 mA)
    sys.setpointCurrent += (encoderDelta * step);
    sys.setpointCurrent = constrain(sys.setpointCurrent, 0.0f, LIMIT_MAX_CURRENT);
    encoderDelta = 0;
    Serial.printf("Zadany proud upraven na: %5.3f A\n", sys.setpointCurrent);
  }

  // Stisk tlačítka enkodéru: Přepnutí ZAP / VYP (nebo reset alarmu)
  if (buttonPressed) {
    buttonPressed = false;
    if (sys.alarmActive) {
      sys.alarmActive = false; // Reset alarmu
      sys.alarmReason = "";
      Serial.println("Reset alarmu potvrzen uzivatelem.");
    } else {
      sys.outputEnabled = !sys.outputEnabled;
      Serial.printf("Vystup zateze: %s\n", sys.outputEnabled ? "ZAPNUTO" : "VYPNUTO");
      if (sys.outputEnabled && sys.mode == MODE_BATT) {
        sys.discharged_mAh = 0.0f;
        sys.discharged_Wh = 0.0f;
        sys.testStartTime = now;
      }
    }
  }

  // 2. RYCHLÁ REGULAČNÍ SMYČKA (Každých 20 ms -> 50 Hz)
  if (now - lastFastLoop >= 20) {
    lastFastLoop = now;
    readAdcSensors();
    checkProtections();
    executeRegulationLoop();
  }

  // 3. INTEGRACE KAPACITY AKUMULÁTORU (Každých 1000 ms)
  if (sys.outputEnabled && sys.mode == MODE_BATT && (now - lastBatteryTick >= 1000)) {
    float dt_hours = (now - lastBatteryTick) / 3600000.0f;
    lastBatteryTick = now;
    sys.discharged_mAh += (sys.current * 1000.0f * dt_hours);
    sys.discharged_Wh += (sys.voltage * sys.current * dt_hours);
  }

  // 4. MĚŘENÍ TEPLOTY A VENTILÁTOR (Každých 1000 ms)
  if (now - lastTempLoop >= 1000) {
    lastTempLoop = now;
    float t = tempSensors.getTempCByIndex(0);
    if (t > -50.0f && t < 125.0f) {
      sys.temperature = t;
    }
    tempSensors.requestTemperatures(); // Požádat o další měření asynchronně
    updateFanControl();

    // Diagnostický výpis do Serial monitoru
    Serial.printf("U: %5.2fV | I: %5.3fA | P: %4.1fW | T: %4.1fC | Fan: %d%% | Out: %d\n",
                  sys.voltage, sys.current, sys.power, sys.temperature, sys.fanSpeed, sys.outputEnabled);
  }

  // 5. OBNOVA OLED DISPLEJE (Každých 150 ms -> cca 7 FPS, optimální pro lidské oko a I2C)
  if (now - lastDisplayLoop >= 150) {
    lastDisplayLoop = now;
    renderOledDisplay();
  }
}
