# ⚡ Programovatelná stejnosměrná elektronická zátěž (0–30 V / 0–5 A / 65 W)

![MCU](https://img.shields.io/badge/MCU-ESP32_DevKit_V1-blue?style=flat-square&logo=espressif)
![Framework](https://img.shields.io/badge/Framework-Arduino_%2F_PlatformIO-orange?style=flat-square&logo=platformio)
![Hardware](https://img.shields.io/badge/PCB-KiCad_9.0_(DRC_0_Errors)-green?style=flat-square&logo=kicad)
![Firmware](https://img.shields.io/badge/Firmware-v2.5_Optimized_Pro-brightgreen?style=flat-square)

**Maturitní projekt** na SPŠ elektrotechnické zaměřený na návrh, konstrukci a programové vybavení číslicově řízené laboratorní stejnosměrné elektronické zátěže. Zařízení slouží k testování charakteristik napájecích zdrojů, ověřování proudové zatížitelnosti adaptérů a automatizovanému měření kapacity i vnitřního odporu akumulátorů.

---

## 🎯 Hlavní technické parametry

| Parametr | Hodnota / Rozsah | Poznámka |
| :--- | :--- | :--- |
| **Vstupní napětí ($U_{\text{in}}$)** | **0 – 30 V DC** | Havarijní přepěťová ochrana OVP při 32,0 V (kritická 34,0 V) |
| **Zatěžovací proud ($I_{\text{load}}$)** | **0 – 5,0 A** | Nadproudová ochrana OCP při 5,20 A |
| **Maximální ztrátový výkon ($P_{\text{max}}$)** | **65 W trvale** (100 W špičkově) | Aktivní chlazení CPU chladičem s heatpipes + softwarový derating |
| **Rozlišení nastavení proudu (DAC)** | **~1,22 mA / LSB** | Externí 12-bitový I2C D/A převodník **MCP4725** |
| **Rozlišení měření (ADC)** | **1,38 mV / 1,25 mA** | Externí 16-bitový Sigma-Delta A/D převodník **ADS1115** (860 SPS) |
| **Provozní režimy** | **CC, CP, CR, BATT** | Konstantní proud, výkon, odpor a automatický test baterií |
| **Regulační smyčka** | **Analogová + 50 Hz digitální** | Plynulý soft-start náběh proudu (17,5 A/s) bez rázů |
| **Ovládání a rozhraní** | **OLED + Enkodér + Wi-Fi Web** | Lokální 0,96" OLED displej, rotační enkodér s akcelerací, Web HUD |

---

## 🛠️ Hybridní architektura systému

Zařízení využívá **hybridní analogově-digitální koncepci**, která spojuje bleskovou odezvu analogového regulátoru s přesností a inteligencí mikrokontroléru ESP32:

```mermaid
flowchart LR
    subgraph Power["Výkonová a analogová část"]
        DUT["Testovaný zdroj / Baterie\n(0–30 V / 0–5 A)"] --> Q1["IRFP250N\n(TO-247 MOSFET)"]
        Q1 --> RSH["Bočník 0,100 Ω / 10 W\n(4-vodičové Kelvinovo snímání)"]
        RSH --> GND["GND"]
        DAC["MCP4725\n(12-bit DAC)"] --> OA["LM358\n(Operační zesilovač)"]
        RSH -. "Zpětná vazba IMON" .-> OA
        OA -- "Buzení Gate" --> Q1
    end

    subgraph Digital["Číslicové řízení a telemetrie (ESP32)"]
        ADC["ADS1115\n(16-bit ADC)"] --> ESP["ESP32 DevKit V1\n(240 MHz Dual-Core)"]
        DUT -. "Dělič 1:11 (VMON)" .-> ADC
        RSH -. "Úbytek (IMON)" .-> ADC
        TEMP["DS18B20\n(Teplota chladiče)"] --> ESP
        ENC["Rotační enkodér\nKY-040"] --> ESP
        ESP --> DAC
        ESP -- "25 kHz PWM" --> FAN["4-pin PC Ventilátor"]
        ESP --> OLED["0.96\" SSD1306 OLED"]
        ESP <--> WEB["Wi-Fi Webové Rozhraní\n(http://zatez.local)"]
    end
```

1. **Rychlá analogová proudová smyčka**: Operační zesilovač **LM358** plynule porovnává žádané napětí z D/A převodníku s okamžitým úbytkem napětí na výkonovém bočníku **0,100 Ω / 10 W** a řídí hradlo výkonového N-MOSFETu **IRFP250N** v lineárním režimu. Analogová smyčka reaguje v řádu mikrosekund a potlačuje proudové špičky při připojení zátěže.
2. **Číslicový dohled a regulace (ESP32)**: Mikrokontrolér v deterministické 50Hz smyčce (perioda 20 ms) čte skutečné svorkové napětí a proud pomocí 16-bitového **ADS1115**, přepočítává požadovaný proud pro režimy **CP** ($I = P/U$), **CR** ($I = U/R$) a **BATT**, integruje kapacitu ($\text{mAh}, \text{Wh}$) a dohlíží na bezpečnostní limity.

---

## 💻 Webové rozhraní (Web Dashboard HUD)

Webová aplikace běží přímo z paměti PROGMEM mikrokontroléru ESP32 (dostupná v lokální síti přes mDNS na adrese `http://zatez.local` nebo v záložním AP režimu `ESP32-Elektronicka-Zatez`).

### Klíčové funkce webového rozhraní:
* **☰ Vysouvací postranní panel (GitHub-style Drawer Menu)**: Kliknutím na ikonu tří čárek `☰` v levém horním rohu se vysune přehledné navigační menu rozdělené do sekcí (*Provoz a měření*, *Konfigurace a systém*, *Vzhled a rozhraní*) včetně živého mini-přehledu veličin a rychlého přepínače motivů.
* **📊 4 barevně kódované digitální přístroje**:
  * ⚡ **Napětí ($U$)**: Ledově modrá (`0.00 V`)
  * 🟢 **Proud ($I$)**: Smaragdově zelená (`0.000 A`)
  * 🟡 **Výkon ($P$)**: Jantarově zlatá (`0.0 W`)
  * 🔴 **Teplota & Ventilátor ($T$)**: Korálově červená (`25.0 °C // 20%`)
* **📈 Retina HiDPI osciloskop**: Vykreslování průběhu napětí a proudu v reálném čase s automatickým škálováním podle `window.devicePixelRatio`, volbou délky paměti (`50 / 100 / 150` bodů) a přepínáním **lineárního (LIN)** i **logaritmického (LOG)** měřítka osy Y i posuvníku.
* **🔋 Analyzátor kapacity baterií**:
  * Přesné měření vybíjecí kapacity v **mAh / Ah** a energie ve **Wh** (64-bitová integrace).
  * Automatický výpočet **vnitřního odporu článku ($R_i$)** v $\text{m}\Omega$ metodou poklesu napětí při zatížení ($R_i = \Delta U / \Delta I$).
  * Rychlé předvolby pro **Li-Ion / LiPo (3,0 V)**, **LiFePO4 (2,5 V)**, **NiMH (1,0 V)** a **Pb 12V (10,5 V)** s automatickým odpojením při dosažení mezního napětí (Cut-off).
* **📥 Záznamník dat (Data Logger) a export**:
  * Záznam průběhu měření do paměti s přímým exportem do **Excelu (`.csv` s UTF-8 BOM a českým formátem desetinné čárky)**, surového **`.json`** a uložením snímku grafu jako **`.png` obrázku**.
* **🎨 5 laboratorních barevných motivů & lokalizace**:
  * *Cyber Dark*, *Keithley Amber*, *CRT Phosphor Green*, *Neon Cyan* a *Clean Paper Light* (světlý režim pro tisk do protokolů).
  * Okamžité přepínání jazyka **Čeština (CZ) / English (EN)** a zvuková odezva tlačítek i alarmů přes **Web Audio API**.

---

## ⚡ Optimalizace firmwaru (v2.5 Optimized Pro)

1. **Ne-blokující asynchronní stavový automat ADC (`adc_driver.cpp`)**:
   * Odstraněno blokující čekání na I2C převody. Stavový automat `poll()` spouští převody na pozadí (860 SPS) a střídá kanály napětí a proudu. Samotné čtení v regulační smyčce trvá **0 µs**.
2. **Plynulý Soft-Start náběh proudu (`dac_driver.cpp`)**:
   * Implementován omezovač strmosti náběhu (*slew-rate limiter* max. $+0{,}35\,\text{A} / 20\,\text{ms} = 17{,}5\,\text{A/s}$), který chrání testovaný zdroj i MOSFET před proudovými rázy a překmity. Vypnutí zátěže zůstává okamžité.
3. **Dynamická akcelerace rotačního enkodéru (`encoder_driver.cpp`)**:
   * Přerušovací rutina měří rychlost otáčení knoflíku ($\Delta t$ v $\mu\text{s}$). Při pomalém otáčení umožňuje jemné krokování po 50 mA ($1\times$), při rychlém protočení násobí krok ($2\times$ až $3\times$).
4. **64-bitová integrace kapacity (`load_regulator.cpp`)**:
   * Vnitřní akumulátory mAh a Wh využívají 64-bitový typ `double`, čímž eliminují zaokrouhlovací chybu plovoucí řádové čárky při dlouhodobých vícehodinových testech baterií.
5. **Odolnost bezpečnostních ochran proti šumu (`safety.cpp`)**:
   * Čtyřstupňový **Safety Supervisor** (**OVP** 32 V, **OCP** 5,2 A, **OPP** 65 W, **OTP** 75 °C) využívá persistenciální filtr (potvrzení 2–3 po sobě jdoucích vzorků za 40–60 ms) proti falešným sepnutím vlivem rušení, zatímco při kritickém přepětí ($>34\,\text{V}$) odpojuje zátěž okamžitě.
6. **Optimalizace Flash paměti (`min_spiffs.csv` + `-O2`)**:
   * Úpravou rozvržení partition tabulky a kompilační optimalizací `-O2` bylo využití paměti Flash sníženo z 94 % na **56,8 %**.

---

## 📌 Přiřazení pinů ESP32 (Pinout dle schématu v KiCadu)

| Periferie / Signál | Pin ESP32 (GPIO) | Konektor na DPS | Popis funkce |
| :--- | :--- | :--- | :--- |
| **I2C SDA** | `GPIO 26` | `J_ESP_R1` (pin 7) | Datová linka I2C (400 kHz Fast Mode – OLED, ADS1115, MCP4725) |
| **I2C SCL** | `GPIO 14` | `J_ESP_R1` (pin 5) | Hodinová linka I2C (400 kHz) |
| **1-Wire DS18B20** | `GPIO 32` | `J_ESP_R1` (pin 10) | Digitální teploměr chladiče Dallas DS18B20 (4,7 kΩ pull-up) |
| **PWM Ventilátor** | `GPIO 33` | `J_ESP_R1` (pin 9) | 25 kHz PWM řízení otáček 4-pinového PC ventilátoru (20–100 %) |
| **Enkodér CLK** | `GPIO 18` | `J_ESP_L1` (pin 9) | Externí přerušení rotačního enkodéru (kanál A) |
| **Enkodér DT** | `GPIO 19` | `J_ESP_L1` (pin 10) | Směrový signál rotačního enkodéru (kanál B) |
| **Enkodér SW** | `GPIO 21` | `J_ESP_L1` (pin 11) | Tlačítko enkodéru (krátký stisk = ZAP/VYP, dlouhý stisk = změna režimu) |

---

## 📁 Struktura repozitáře

```text
├── hardware/                  # Kompletní návrh DPS a schématu v KiCad 9.0
│   ├── elektronicka zatez.kicad_pro
│   ├── elektronicka zatez.kicad_sch   # Elektrické schéma (32 součástek, plně propojeno)
│   └── elektronicka zatez.kicad_pcb   # Dvouvrstvá DPS (DRC: 0 chyb, 0 nepřipojených spojů)
├── firmware/                  # Zdrojový kód pro ESP32 (PlatformIO / Arduino C++)
│   ├── platformio.ini         # Konfigurace sestavení, knihoven a partition tabulky
│   ├── include/
│   │   ├── config.h           # Konfigurace pinů, kalibrace, Wi-Fi a bezpečnostních limitů
│   │   ├── types.h            # Datové struktury a výčtové typy
│   │   ├── hal/               # Ovladače hardwaru (ADC, DAC, Ventilátor, Teploměr, Enkodér)
│   │   ├── core/              # Regulační smyčka (CC/CP/CR/BATT) a Safety Supervisor
│   │   ├── ui/                # Vykreslování na 0,96" OLED displej SSD1306
│   │   └── net/               # Wi-Fi manažer, REST API server a Web Dashboard (PROGMEM)
│   └── src/                   # Implementační soubory (.cpp) a hlavní smyčka main.cpp
└── docs/                      # Technická dokumentace a kusovník (BOM)
```

---

## 🚀 Kompilace a nahrání firmwaru

1. Otevřete složku `firmware/` ve vývojovém prostředí **VS Code** s rozšířením **PlatformIO**.
2. V souboru [`firmware/include/config.h`](firmware/include/config.h) můžete upravit přihlašovací údaje k domácí Wi-Fi síti (`WIFI_STA_SSID` a `WIFI_STA_PASS`). Pokud se zátěž k routeru nepřipojí, automaticky vytvoří vlastní přístupový bod **`ESP32-Elektronicka-Zatez`** (heslo `12345678`, IP `192.168.4.1`).
3. Zkompilujte a nahrajte projekt příkazem:
   ```bash
   pio run -t upload -d firmware
   ```
4. Otevřete webové rozhraní v prohlížeči na adrese **`http://zatez.local`** (nebo na IP adrese vypsané na OLED displeji při startu).

---
*Autor projektu: **Ondřej Pinkas** — Maturitní práce, SPŠ Elektrotechnická*
