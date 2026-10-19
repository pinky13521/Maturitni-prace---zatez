# Programovatelná stejnosměrná elektronická zátěž

Maturitní projekt na SPŠ elektrotechnické zaměřený na návrh a realizaci číslicově řízené laboratorní stejnosměrné zátěže. Zařízení slouží k testování napájecích zdrojů a automatizovanému měření kapacity akumulátorů.

## 🎯 Hlavní parametry
- **Vstupní napětí:** 0–30 V
- **Zatěžovací proud:** 0–5 A (krok cca 1,22 mA)
- **Ztrátový výkon:** trvale do 60 W (krátkodobě až 100 W)
- **Řídicí logika:** ESP32 DevKit V1
- **Uživatelské rozhraní:** 0.96" I2C OLED displej, rotační enkodér, Wi-Fi webové rozhraní

## 🛠️ Architektura systému
Projekt využívá **hybridní koncepci**:
1. **Rychlá analogová smyčka:** Operační zesilovač (MCP6002 / LM358), výkonový planární MOSFET (IRFP250N v pouzdře TO-247) a čtyřvodičově snímaný bočník (Kelvinovo zapojení, 0,1 Ω / 10 W). Smyčka okamžitě reaguje na změny a eliminuje proudové špičky.
2. **Číslicové řízení:** ESP32 nastavuje žádanou hodnotu proudu přes externí 12-bitový DAC (MCP4725) a měří skutečné napětí a proud pomocí 16-bitového diferenciálního ADC (ADS1115).
3. **Funkce a režimy:** CC (konstantní proud), softwarový výpočet CP/CR, měření vybíjecích křivek akumulátorů (mAh, Wh) s automatickým odpojením a webový server s živým grafem.

## 📁 Struktura projektu
- `/hardware`: Schéma a návrh All-in-One desky v KiCadu
- `/firmware`: Zdrojové kódy pro ESP32 (Arduino IDE / PlatformIO)
- `/docs`: Technická dokumentace, kusovník (BOM) a schémata
