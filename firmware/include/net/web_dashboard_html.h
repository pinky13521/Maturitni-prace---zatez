/**
 * @file web_dashboard_html.h
 * @brief Technický minimalistický dashboard elektronické zátěže (B - Technical Monochrome)
 *        Obsahuje lineární i logaritmické stupnice grafu i posuvníku (LIN / LOG decades).
 */

#ifndef WEB_DASHBOARD_HTML_H
#define WEB_DASHBOARD_HTML_H

#include <Arduino.h>

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
  <meta name="apple-mobile-web-app-capable" content="yes">
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
  <meta name="theme-color" content="#000000">
  <title>DC ELECTRONIC LOAD 30V / 5A</title>
  <style>
    :root {
      --bg: #000000;
      --panel: #0a0a0a;
      --panel-alt: #121212;
      --border: #333333;
      --border-focus: #666666;
      --text: #ffffff;
      --text-muted: #8e8e8e;
      --text-dim: #b8b8b8;
      --invert-bg: #ffffff;
      --invert-text: #000000;
      --alarm: #ff2a2a;
      --mono: ui-monospace, "SF Mono", Monaco, "Cascadia Code", "Roboto Mono", Consolas, monospace;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: var(--mono);
      -webkit-tap-highlight-color: transparent;
    }

    html, body {
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
      min-height: -webkit-fill-available;
      -webkit-font-smoothing: antialiased;
    }

    body {
      display: flex;
      justify-content: center;
      padding: 16px;
    }

    .frame {
      width: 100%;
      max-width: 980px;
      display: flex;
      flex-direction: column;
      gap: 1px;
      background: var(--border);
      border: 1px solid var(--border);
    }

    /* --- ZÁHLAVÍ (HEADER) --- */
    .header-bar {
      background: var(--panel);
      padding: 12px 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 10px;
    }

    .header-left {
      display: flex;
      flex-direction: column;
    }

    .header-title {
      font-size: 11px;
      letter-spacing: 0.15em;
      text-transform: uppercase;
      font-weight: 700;
    }

    .header-sub {
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.08em;
      margin-top: 2px;
    }

    .header-right {
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .link-indicator {
      font-size: 10px;
      letter-spacing: 0.1em;
      font-weight: 700;
      padding: 4px 8px;
      border: 1px solid var(--border);
    }
    .link-ok { color: #22ff66; border-color: #1a552a; background: #001a08; }
    .link-err { color: #ff3333; border-color: #aa2222; background: #220000; }

    .fs-btn {
      background: #0e0e0e;
      color: #cccccc;
      border: 1px solid #444444;
      padding: 4px 8px;
      font-size: 10px;
      font-weight: 700;
      letter-spacing: 0.08em;
      cursor: pointer;
      outline: none;
      border-radius: 0;
      transition: background 0.1s, color 0.1s;
    }
    .fs-btn:hover {
      color: #ffffff;
      border-color: #888888;
      background: #1a1a1a;
    }

    .annunciator {
      font-size: 11px;
      font-weight: 800;
      letter-spacing: 0.12em;
      padding: 4px 10px;
      border: 1px solid #555555;
    }

    .ann-on { background: #ffffff; color: #000000; border-color: #ffffff; }
    .ann-off { background: #080808; color: #aaaaaa; border-color: #444444; }
    .ann-alarm { background: #ff2222; color: #ffffff; border-color: #ff2222; }

    /* --- MĚŘENÉ HODNOTY (READOUTS) --- */
    .readouts {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 1px;
      background: var(--border);
    }

    .readout-cell {
      background: var(--panel);
      padding: 16px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      min-height: 94px;
    }

    .cell-tag {
      font-size: 10px;
      color: #999999;
      font-weight: 700;
      letter-spacing: 0.12em;
      text-transform: uppercase;
      display: flex;
      justify-content: space-between;
    }

    .cell-num {
      font-size: clamp(24px, 4vw, 36px);
      font-weight: 800;
      letter-spacing: -0.02em;
      line-height: 1.1;
      margin: 8px 0 4px 0;
      color: #ffffff;
      font-variant-numeric: tabular-nums;
    }

    .cell-unit {
      font-size: 11px;
      color: #999999;
      font-weight: 600;
      letter-spacing: 0.05em;
    }

    /* --- OVLÁDACÍ PANEL (CONTROL DECK) --- */
    .control-deck {
      background: var(--panel);
      padding: 16px 18px;
      display: flex;
      flex-direction: column;
      gap: 14px;
    }

    .deck-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 12px;
      flex-wrap: wrap;
    }

    .btn-group {
      display: inline-flex;
      border: 1px solid var(--border);
      gap: 1px;
      background: var(--border);
    }

    .mode-btn {
      background: var(--panel);
      color: var(--text-muted);
      border: none;
      padding: 10px 16px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.1em;
      cursor: pointer;
      outline: none;
      border-radius: 0;
      transition: background 0.1s, color 0.1s;
    }

    .mode-btn:hover { color: var(--text); }
    .mode-btn.active {
      background: var(--invert-bg);
      color: var(--invert-text);
    }

    .power-btn {
      background: transparent;
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 10px 24px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.12em;
      text-transform: uppercase;
      cursor: pointer;
      outline: none;
      border-radius: 0;
      transition: background 0.1s, color 0.1s;
    }

    .power-btn:hover { border-color: var(--text); }
    .power-btn:active { opacity: 0.6; transform: scale(0.99); }
    .power-btn.engaged {
      background: var(--invert-bg);
      color: var(--invert-text);
      border-color: var(--invert-bg);
    }

    /* --- NASTAVENÍ HODNOTY (SLIDER & INPUT) --- */
    .slider-wrap {
      display: flex;
      flex-direction: column;
      gap: 10px;
      width: 100%;
    }

    .slider-legend {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 11px;
      letter-spacing: 0.08em;
      gap: 8px;
      flex-wrap: wrap;
    }

    .scale-selector {
      display: inline-flex;
      align-items: center;
      gap: 1px;
      border: 1px solid var(--border);
      background: var(--border);
    }

    .scale-label {
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.08em;
      padding: 0 4px;
    }

    .scale-btn {
      background: var(--panel);
      color: var(--text-muted);
      border: none;
      padding: 4px 8px;
      font-size: 10px;
      font-weight: 700;
      letter-spacing: 0.08em;
      cursor: pointer;
      outline: none;
      border-radius: 0;
      transition: background 0.1s, color 0.1s;
    }

    .scale-btn:hover {
      color: var(--text);
    }

    .scale-btn.active {
      background: var(--invert-bg);
      color: var(--invert-text);
    }

    .sp-input-box {
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }

    .num-input {
      background: #000000;
      border: 1px solid var(--border-focus);
      color: var(--text);
      font-size: 13px;
      font-weight: 700;
      font-family: var(--mono);
      padding: 6px 8px;
      width: 86px;
      text-align: right;
      outline: none;
      border-radius: 0;
      -moz-appearance: textfield;
    }

    .num-input::-webkit-outer-spin-button,
    .num-input::-webkit-inner-spin-button {
      -webkit-appearance: none;
      margin: 0;
    }

    .num-input:focus {
      border-color: var(--text);
      background: #0a0a0a;
    }

    .sp-set-btn {
      background: transparent;
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 6px 12px;
      font-size: 10px;
      font-weight: 700;
      font-family: var(--mono);
      letter-spacing: 0.08em;
      cursor: pointer;
      outline: none;
      border-radius: 0;
    }

    .sp-set-btn:hover {
      background: var(--invert-bg);
      color: var(--invert-text);
      border-color: var(--invert-bg);
    }

    .sp-set-btn:active {
      opacity: 0.7;
    }

    /* Posuvník */
    input[type=range] {
      -webkit-appearance: none;
      width: 100%;
      height: 6px;
      background: var(--border-focus);
      outline: none;
      cursor: pointer;
      border-radius: 0;
    }

    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 20px;
      height: 26px;
      background: var(--invert-bg);
      border: 1px solid #000000;
      cursor: pointer;
      border-radius: 0;
    }

    input[type=range]::-moz-range-thumb {
      width: 20px;
      height: 26px;
      background: var(--invert-bg);
      border: 1px solid #000000;
      cursor: pointer;
      border-radius: 0;
    }

    /* Rychlé předvolby */
    .presets-row {
      display: flex;
      align-items: center;
      gap: 6px;
      flex-wrap: wrap;
    }

    .preset-label {
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.08em;
      margin-right: 4px;
    }

    .preset-btn {
      background: #000000;
      color: var(--text-dim);
      border: 1px solid var(--border);
      padding: 6px 12px;
      font-size: 10px;
      font-weight: 700;
      letter-spacing: 0.06em;
      cursor: pointer;
      outline: none;
      border-radius: 0;
      transition: border-color 0.1s, color 0.1s;
    }

    .preset-btn:hover {
      border-color: var(--border-focus);
      color: var(--text);
    }

    .preset-btn:active {
      background: var(--invert-bg);
      color: var(--invert-text);
      border-color: var(--invert-bg);
    }

    /* --- GRAFICKÝ PANEL (OSCILLOSCOPE GRATICULE) --- */
    .graph-panel {
      background: var(--panel);
      padding: 14px 18px;
      display: flex;
      flex-direction: column;
      gap: 10px;
    }

    .graph-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.1em;
      text-transform: uppercase;
      flex-wrap: wrap;
      gap: 6px;
    }

    canvas {
      width: 100%;
      height: 190px;
      display: block;
      background: #020202;
    }

    /* --- ALARMOVÝ PÁS (SAFETY TRIP) --- */
    .alarm-banner {
      display: none;
      background: #110000;
      border-bottom: 1px solid var(--alarm);
      padding: 10px 18px;
      justify-content: space-between;
      align-items: center;
      color: var(--alarm);
      font-size: 11px;
      letter-spacing: 0.08em;
    }

    .btn-reset {
      background: transparent;
      color: var(--alarm);
      border: 1px solid var(--alarm);
      padding: 6px 12px;
      font-size: 10px;
      letter-spacing: 0.1em;
      font-weight: 700;
      cursor: pointer;
      border-radius: 0;
    }

    .btn-reset:hover {
      background: var(--alarm);
      color: #000000;
    }

    /* RESPONSIVE DESIGN */
    @media (max-width: 680px) {
      body { padding: 0; }
      .frame { border: none; max-width: 100%; min-height: 100vh; }
      .header-bar { padding: 10px 12px; }
      .header-title { font-size: 10px; }
      .header-sub { display: none; }
      .readouts { grid-template-columns: repeat(2, 1fr); }
      .readout-cell { padding: 12px; min-height: 80px; }
      .cell-num { font-size: 24px; }
      .control-deck { padding: 12px; gap: 12px; }
      .deck-row { flex-direction: column; align-items: stretch; gap: 8px; }
      .btn-group { display: flex; width: 100%; }
      .mode-btn { flex: 1; padding: 12px 4px; text-align: center; font-size: 11px; }
      .power-btn { width: 100%; padding: 14px; font-size: 12px; text-align: center; }
      .slider-legend { flex-direction: column; align-items: flex-start; gap: 8px; }
      .sp-input-box { width: 100%; justify-content: space-between; }
      .num-input { flex: 1; font-size: 16px; padding: 8px 10px; }
      .sp-set-btn { padding: 8px 16px; }
      .presets-row { display: grid; grid-template-columns: repeat(5, 1fr); gap: 4px; }
      .preset-label { grid-column: 1 / -1; margin-bottom: 2px; }
      .preset-btn { padding: 8px 2px; text-align: center; font-size: 10px; }
      .graph-panel { padding: 12px; }
      canvas { height: 160px; }
    }
  </style>
</head>
<body>

  <div class="frame">
    <!-- Havarijní výstraha -->
    <div id="alarmBar" class="alarm-banner">
      <span id="alarmReason" data-i18n="alarm_default">SYSTÉMOVÝ ALARM: VYBAVENÍ OCHRANY</span>
      <button class="btn-reset" onclick="ackAlarm()" data-i18n="btn_ack">[ POTVRDIT / RESET ]</button>
    </div>

    <!-- Hlavní lišta -->
    <div class="header-bar">
      <div class="header-left">
        <div class="header-title" data-i18n="header_title">DC ELEKTRONICKÁ ZÁTĚŽ // 30V 5A</div>
        <div class="header-sub" data-i18n="header_sub">DIGITÁLNÍ REGULACE &bull; 50 HZ SMYČKA</div>
      </div>
      <div class="header-right">
        <span id="linkStatus" class="link-indicator link-ok">● ŽIVĚ</span>
        <button id="langBtn" class="fs-btn" onclick="toggleLanguage()" title="Přepnout jazyk / Toggle language">[ EN ]</button>
        <button id="fsBtn" class="fs-btn" onclick="toggleFullscreen()" title="Fullscreen">[ ⛶ FULL ]</button>
        <span id="stateAnn" class="annunciator ann-off">[ VYP ]</span>
      </div>
    </div>

    <!-- 4 digitální měřáky -->
    <div class="readouts">
      <div class="readout-cell">
        <div class="cell-tag"><span data-i18n="lbl_v">NAPĚTÍ</span> <span class="cell-unit" data-i18n="lbl_v_max">MAX 32V</span></div>
        <div class="cell-num"><span id="valV">00.00</span></div>
        <div class="cell-unit" data-i18n="unit_v">VOLTY DC</div>
      </div>

      <div class="readout-cell">
        <div class="cell-tag"><span data-i18n="lbl_i">PROUD</span> <span class="cell-unit" data-i18n="lbl_i_max">MAX 5.2A</span></div>
        <div class="cell-num"><span id="valI">0.000</span></div>
        <div class="cell-unit" data-i18n="unit_i">AMPÉRY</div>
      </div>

      <div class="readout-cell">
        <div class="cell-tag"><span data-i18n="lbl_p">VÝKON</span> <span class="cell-unit" data-i18n="lbl_p_max">MAX 65W</span></div>
        <div class="cell-num"><span id="valP">00.0</span></div>
        <div class="cell-unit" data-i18n="unit_p">WATTY</div>
      </div>

      <div class="readout-cell">
        <div class="cell-tag"><span data-i18n="lbl_t">TEPLOTA</span> <span class="cell-unit" data-i18n="lbl_t_max">OTP 75°C</span></div>
        <div class="cell-num"><span id="valT">25.0</span></div>
        <div class="cell-unit">&deg;C // <span data-i18n="lbl_fan">VENT.</span> <span id="valFan">20</span>%</div>
      </div>
    </div>

    <!-- Ovládací prvky -->
    <div class="control-deck">
      <div class="deck-row">
        <div class="btn-group">
          <button class="mode-btn active" id="m0" onclick="selectMode(0)">CC</button>
          <button class="mode-btn" id="m1" onclick="selectMode(1)">CP</button>
          <button class="mode-btn" id="m2" onclick="selectMode(2)">CR</button>
          <button class="mode-btn" id="m3" onclick="selectMode(3)">BATT</button>
        </div>
        <button id="pwrBtn" class="power-btn" onclick="togglePower()">VÝSTUP: VYP</button>
      </div>

      <!-- Nastavení žádané hodnoty (posuvník + číslo + přepínač LIN/LOG) -->
      <div class="slider-wrap">
        <div class="slider-legend">
          <div style="display:flex; align-items:center; gap:8px;">
            <span id="lblSp" style="color:var(--text-muted);">ŽÁDANÝ PROUD:</span>
            <div class="scale-selector">
              <span class="scale-label" data-i18n="lbl_curve">PRŮBĚH:</span>
              <button class="scale-btn active" id="btnSliderLin" onclick="setSliderCurve('lin')">LIN</button>
              <button class="scale-btn" id="btnSliderLog" onclick="setSliderCurve('log')">LOG</button>
            </div>
          </div>

          <div class="sp-input-box">
            <input type="number" id="spInput" class="num-input" value="0.500" step="0.025" min="0" max="5.2"
                   onkeydown="if(event.key==='Enter'){submitNumericInput();this.blur();}"
                   onchange="submitNumericInput()">
            <span id="unitSp" style="color:var(--text-muted); font-size:11px; font-weight:700;">A</span>
            <button class="sp-set-btn" id="spSetBtn" onclick="submitNumericInput()" data-i18n="btn_set">NASTAV</button>
          </div>
        </div>
        <input type="range" id="spSlider" min="0" max="1000" step="1" value="96"
               onmousedown="isDragging=true" ontouchstart="isDragging=true"
               onmouseup="isDragging=false" ontouchend="isDragging=false"
               oninput="onSliderMove(this.value)">
      </div>

      <!-- Rychlé předvolby -->
      <div class="presets-row">
        <span class="preset-label" data-i18n="lbl_presets">PŘEDVOLBY:</span>
        <div id="presetsBox" style="display:contents;"></div>
      </div>

      <!-- Statistiky kapacity pro režim baterie (BATT) -->
      <div id="battRow" style="display:none; font-size:11px; color:var(--text-dim); border-top:1px solid var(--border); padding-top:10px;">
        <span data-i18n="lbl_discharged">VYBITO:</span> <strong id="valMah" style="color:var(--text);">0</strong> mAh &bull; <strong id="valWh" style="color:var(--text);">0.00</strong> Wh &bull; <span data-i18n="lbl_cutoff">MEZNÍ NAPĚTÍ:</span> <strong id="valCut" style="color:var(--text);">3.00</strong> V
      </div>
    </div>

    <!-- Osciloskopický graticule log s přepínačem LIN/LOG stupnice -->
    <div class="graph-panel">
      <div class="graph-header">
        <div style="display:flex; align-items:center; gap:8px;">
          <span data-i18n="lbl_graph">OSCILOSKOPICKÝ ZÁZNAM [ 50 BODŮ ]</span>
          <span style="color:var(--text-dim);" data-i18n="lbl_graph_sub">&bull; U [BÍLÁ] &bull; I [ŠEDÁ]</span>
        </div>
        <div class="scale-selector">
          <span class="scale-label" data-i18n="lbl_yscale">MĚŘÍTKO Y:</span>
          <button class="scale-btn active" id="btnScaleLin" onclick="setPlotScale('lin')">LIN</button>
          <button class="scale-btn" id="btnScaleLog" onclick="setPlotScale('log')">LOG</button>
        </div>
      </div>
      <canvas id="cv"></canvas>
    </div>
  </div>

  <script>
    let mode = 0;
    let isOutput = false;
    let isAlarm = false;
    let inFlight = false;
    let isDragging = false;
    let plotScale = 'lin';    // 'lin' nebo 'log'
    let sliderCurve = 'lin';  // 'lin' nebo 'log'
    let currentSetpointVal = 0.500;
    let tOut = null;
    let currentLang = localStorage.getItem('load_lang') || 'cs';
    const history = [];
    const LIMIT = 50;

    const I18N = {
      cs: {
        lang_btn: '[ EN ]',
        header_title: 'DC ELEKTRONICKÁ ZÁTĚŽ // 30V 5A',
        header_sub: 'DIGITÁLNÍ REGULACE • 50 HZ SMYČKA',
        live: '● ŽIVĚ',
        offline: '○ ODPOJENO',
        full_enter: '[ ⛶ FULL ]',
        full_exit: '[ ✕ ZAVŘÍT ]',
        lbl_v: 'NAPĚTÍ',
        lbl_v_max: 'MAX 32V',
        unit_v: 'VOLTY DC',
        lbl_i: 'PROUD',
        lbl_i_max: 'MAX 5.2A',
        unit_i: 'AMPÉRY',
        lbl_p: 'VÝKON',
        lbl_p_max: 'MAX 65W',
        unit_p: 'WATTY',
        lbl_t: 'TEPLOTA',
        lbl_t_max: 'OTP 75°C',
        lbl_fan: 'VENT.',
        pwr_off: 'VÝSTUP: VYP',
        pwr_active: 'VÝSTUP: AKTIVNÍ',
        pwr_tripped: 'VÝSTUP: VYBAVENO',
        ann_off: '[ VYP ]',
        ann_on: '[ ZAP ]',
        ann_alarm: '[ ALARM ]',
        lbl_sp_cc: 'ŽÁDANÝ PROUD:',
        lbl_sp_cp: 'ŽÁDANÝ VÝKON:',
        lbl_sp_cr: 'ŽÁDANÝ ODPOR:',
        lbl_curve: 'PRŮBĚH:',
        btn_set: 'NASTAV',
        lbl_presets: 'PŘEDVOLBY:',
        lbl_discharged: 'VYBITO:',
        lbl_cutoff: 'MEZNÍ NAPĚTÍ:',
        lbl_graph: 'OSCILOSKOPICKÝ ZÁZNAM [ 50 BODŮ ]',
        lbl_graph_sub: '• U [BÍLÁ] • I [ŠEDÁ]',
        lbl_yscale: 'MĚŘÍTKO Y:',
        btn_ack: '[ POTVRDIT / RESET ]',
        alarm_default: 'SYSTÉMOVÝ ALARM: VYBAVENÍ OCHRANY',
        alarm_trip: 'OCHRANA: '
      },
      en: {
        lang_btn: '[ CZ ]',
        header_title: 'DC ELECTRONIC LOAD // 30V 5A',
        header_sub: 'DIGITAL REGULATION • 50 HZ LOOP',
        live: '● LIVE',
        offline: '○ OFFLINE',
        full_enter: '[ ⛶ FULL ]',
        full_exit: '[ ✕ EXIT ]',
        lbl_v: 'VOLTAGE',
        lbl_v_max: 'MAX 32V',
        unit_v: 'VOLTS DC',
        lbl_i: 'CURRENT',
        lbl_i_max: 'MAX 5.2A',
        unit_i: 'AMPERES',
        lbl_p: 'POWER',
        lbl_p_max: 'MAX 65W',
        unit_p: 'WATTS',
        lbl_t: 'THERMAL',
        lbl_t_max: 'OTP 75C',
        lbl_fan: 'FAN',
        pwr_off: 'OUTPUT: OFF',
        pwr_active: 'OUTPUT: ACTIVE',
        pwr_tripped: 'OUTPUT: TRIPPED',
        ann_off: '[ OFF ]',
        ann_on: '[ ON ]',
        ann_alarm: '[ ALARM ]',
        lbl_sp_cc: 'CURRENT SETPOINT:',
        lbl_sp_cp: 'POWER SETPOINT:',
        lbl_sp_cr: 'RESISTANCE SETPOINT:',
        lbl_curve: 'CURVE:',
        btn_set: 'SET',
        lbl_presets: 'PRESETS:',
        lbl_discharged: 'DISCHARGED:',
        lbl_cutoff: 'CUTOFF:',
        lbl_graph: 'GRATICULE REALTIME LOG [ 50 PTS ]',
        lbl_graph_sub: '• U [WHT] • I [GRY]',
        lbl_yscale: 'Y-SCALE:',
        btn_ack: '[ ACK / RESET ]',
        alarm_default: 'SYS_ALARM: TRIP OCCURRED',
        alarm_trip: 'TRIP: '
      }
    };

    function toggleLanguage() {
      currentLang = (currentLang === 'cs') ? 'en' : 'cs';
      localStorage.setItem('load_lang', currentLang);
      applyLanguage();
    }

    function applyLanguage() {
      const t = I18N[currentLang] || I18N.cs;
      document.querySelectorAll('[data-i18n]').forEach(el => {
        const k = el.getAttribute('data-i18n');
        if (t[k] !== undefined) el.innerText = t[k];
      });
      const langBtn = document.getElementById('langBtn');
      if (langBtn) langBtn.innerText = t.lang_btn;
      const fsBtn = document.getElementById('fsBtn');
      if (fsBtn) {
        fsBtn.innerText = document.fullscreenElement ? t.full_exit : t.full_enter;
      }
      syncModeConfig(mode);
      if (isAlarm) {
        const ann = document.getElementById('stateAnn');
        if (ann) ann.innerText = t.ann_alarm;
        const btn = document.getElementById('pwrBtn');
        if (btn) btn.innerText = t.pwr_tripped;
      } else {
        setOutputVisuals(isOutput);
      }
    }

    const cv = document.getElementById('cv');
    const ctx = cv.getContext('2d');

    function resize() {
      const containerW = cv.parentElement.clientWidth;
      cv.width = containerW > 36 ? containerW - 36 : containerW;
      cv.height = window.innerWidth < 680 ? 160 : 190;
      renderPlot();
    }
    window.addEventListener('resize', resize);
    window.addEventListener('orientationchange', () => setTimeout(resize, 200));
    resize();

    function toggleFullscreen() {
      if (!document.fullscreenElement) {
        document.documentElement.requestFullscreen().catch(() => {});
      } else {
        if (document.exitFullscreen) {
          document.exitFullscreen();
        }
      }
    }

    document.addEventListener('fullscreenchange', () => {
      const btn = document.getElementById('fsBtn');
      const t = I18N[currentLang] || I18N.cs;
      if (btn) {
        btn.innerText = document.fullscreenElement ? t.full_exit : t.full_enter;
      }
      setTimeout(resize, 150);
    });

    function setPlotScale(s) {
      plotScale = s;
      document.getElementById('btnScaleLin').classList.toggle('active', s === 'lin');
      document.getElementById('btnScaleLog').classList.toggle('active', s === 'log');
      renderPlot();
    }

    function setSliderCurve(c) {
      sliderCurve = c;
      document.getElementById('btnSliderLin').classList.toggle('active', c === 'lin');
      document.getElementById('btnSliderLog').classList.toggle('active', c === 'log');
      document.getElementById('spSlider').value = valueToSliderPos(currentSetpointVal);
    }

    function getLimitsForMode() {
      if (mode === 0 || mode === 3) return { minLin: 0.0, minLog: 0.010, max: 5.2, decimals: 3 };
      if (mode === 1) return { minLin: 0.0, minLog: 0.5, max: 65.0, decimals: 1 };
      if (mode === 2) return { minLin: 0.5, minLog: 0.5, max: 500.0, decimals: 1 };
      return { minLin: 0.0, minLog: 0.010, max: 5.2, decimals: 3 };
    }

    function sliderPosToValue(pos) {
      const lim = getLimitsForMode();
      const ratio = Math.max(0, Math.min(1000, pos)) / 1000.0;
      if (sliderCurve === 'lin') {
        return lim.minLin + ratio * (lim.max - lim.minLin);
      } else {
        return lim.minLog * Math.pow(lim.max / lim.minLog, ratio);
      }
    }

    function valueToSliderPos(val) {
      const lim = getLimitsForMode();
      if (sliderCurve === 'lin') {
        const r = (val - lim.minLin) / (lim.max - lim.minLin);
        return Math.round(Math.max(0, Math.min(1, r)) * 1000);
      } else {
        const clamped = Math.max(lim.minLog, Math.min(lim.max, val));
        const r = (Math.log10(clamped) - Math.log10(lim.minLog)) / (Math.log10(lim.max) - Math.log10(lim.minLog));
        return Math.round(Math.max(0, Math.min(1, r)) * 1000);
      }
    }

    /* --- VYKRESLOVÁNÍ GRAFU (LIN / LOG) --- */
    function renderPlot() {
      ctx.clearRect(0, 0, cv.width, cv.height);
      const w = cv.width;
      const h = cv.height;

      const pL = 34;      // Levý okraj pro napětí (U)
      const pR = w - 38;  // Pravý okraj pro proud (I)
      const pT = 12;      // Horní okraj
      const pB = h - 16;  // Dolní okraj
      const plotW = pR - pL;
      const plotH = pB - pT;

      // Ohraničení rastru
      ctx.strokeStyle = '#3e3e3e';
      ctx.lineWidth = 1;
      ctx.strokeRect(pL, pT, plotW, plotH);

      ctx.font = '10px monospace';
      ctx.textBaseline = 'middle';

      if (plotScale === 'lin') {
        // Lineární rastr (5 horizontálních dělení)
        const divs = 4;
        for (let i = 0; i <= divs; i++) {
          const y = pB - (i / divs) * plotH;
          if (i > 0 && i < divs) {
            ctx.strokeStyle = '#242424';
            ctx.beginPath(); ctx.moveTo(pL, y); ctx.lineTo(pR, y); ctx.stroke();
          }
          // Levá osa U (0, 8, 16, 24, 32 V)
          const vVal = (i / divs) * 32.0;
          ctx.fillStyle = '#e0e0e0';
          ctx.textAlign = 'right';
          ctx.fillText(vVal.toFixed(0) + 'V', pL - 5, y);

          // Pravá osa I (0, 1.3, 2.6, 3.9, 5.2 A)
          const iVal = (i / divs) * 5.2;
          ctx.fillStyle = '#b0b0b0';
          ctx.textAlign = 'left';
          ctx.fillText(iVal.toFixed(1) + 'A', pR + 5, y);
        }
      } else {
        // Logaritmický rastr s dekádami (LOG DECADES)
        // Napěťové dekády (0.1V, 1V, 10V, 32V)
        const logVmin = Math.log10(0.1);
        const logVmax = Math.log10(32.0);
        const vTicks = [
          { val: 0.1, label: '0.1V', major: true },
          { val: 0.5, label: '',     major: false },
          { val: 1.0, label: '1V',   major: true },
          { val: 2.0, label: '',     major: false },
          { val: 5.0, label: '',     major: false },
          { val: 10.0, label: '10V', major: true },
          { val: 20.0, label: '',    major: false },
          { val: 32.0, label: '32V', major: true }
        ];

        vTicks.forEach(t => {
          const norm = (Math.log10(t.val) - logVmin) / (logVmax - logVmin);
          const y = pB - norm * plotH;
          ctx.strokeStyle = t.major ? '#333333' : '#1a1a1a';
          ctx.beginPath(); ctx.moveTo(pL, y); ctx.lineTo(pR, y); ctx.stroke();

          if (t.label) {
            ctx.fillStyle = '#e0e0e0';
            ctx.textAlign = 'right';
            ctx.fillText(t.label, pL - 5, y);
          }
        });

        // Proudové dekády (1mA, 10mA, 100mA, 1A, 5.2A)
        const logImin = Math.log10(0.001);
        const logImax = Math.log10(5.2);
        const iTicks = [
          { val: 0.001, label: '1m' },
          { val: 0.010, label: '10m' },
          { val: 0.100, label: '100m' },
          { val: 1.000, label: '1.0A' },
          { val: 5.200, label: '5.2A' }
        ];

        iTicks.forEach(t => {
          const norm = (Math.log10(t.val) - logImin) / (logImax - logImin);
          const y = pB - norm * plotH;
          ctx.fillStyle = '#b0b0b0';
          ctx.textAlign = 'left';
          ctx.fillText(t.label, pR + 5, y);
        });
      }

      // Vertikální mřížka (čas)
      const xDivs = 8;
      for (let j = 1; j < xDivs; j++) {
        const x = pL + (plotW / xDivs) * j;
        ctx.strokeStyle = '#1a1a1a';
        ctx.beginPath(); ctx.moveTo(x, pT); ctx.lineTo(x, pB); ctx.stroke();
      }

      if (history.length < 2) return;

      // Kreslení signálů oříznutých uvnitř mřížky
      ctx.save();
      ctx.beginPath();
      ctx.rect(pL, pT, plotW, plotH);
      ctx.clip();

      function plotTrace(prop, isLog, minVal, maxVal, strokeStyle, width) {
        ctx.strokeStyle = strokeStyle;
        ctx.lineWidth = width;
        ctx.beginPath();

        const step = plotW / (LIMIT - 1);
        const startX = pR - (history.length - 1) * step;

        history.forEach((pt, idx) => {
          const x = startX + idx * step;
          let norm = 0;
          const raw = pt[prop];

          if (isLog) {
            if (raw > 0) {
              const clamped = Math.max(minVal, Math.min(maxVal, raw));
              norm = (Math.log10(clamped) - Math.log10(minVal)) / (Math.log10(maxVal) - Math.log10(minVal));
            } else {
              norm = 0;
            }
          } else {
            norm = Math.min(1, Math.max(0, raw / maxVal));
          }

          const y = pB - norm * plotH;
          if (idx === 0) ctx.moveTo(x, y);
          else ctx.lineTo(x, y);
        });
        ctx.stroke();
      }

      if (plotScale === 'lin') {
        plotTrace('u', false, 0, 32.0, '#ffffff', 2.0);
        plotTrace('i', false, 0, 5.2, '#aaaaaa', 1.8);
      } else {
        plotTrace('u', true, 0.1, 32.0, '#ffffff', 2.0);
        plotTrace('i', true, 0.001, 5.2, '#aaaaaa', 1.8);
      }

      ctx.restore();
    }

    function setOutputVisuals(active) {
      const t = I18N[currentLang] || I18N.cs;
      const ann = document.getElementById('stateAnn');
      const btn = document.getElementById('pwrBtn');
      if (isAlarm) return;
      if (active) {
        ann.className = 'annunciator ann-on';
        ann.innerText = t.ann_on;
        btn.className = 'power-btn engaged';
        btn.innerText = t.pwr_active;
      } else {
        ann.className = 'annunciator ann-off';
        ann.innerText = t.ann_off;
        btn.className = 'power-btn';
        btn.innerText = t.pwr_off;
      }
    }

    const PRESET_MAP = {
      0: [
        { label: '0.1A', val: 0.1 },
        { label: '0.5A (USB)', val: 0.5 },
        { label: '1.0A', val: 1.0 },
        { label: '2.0A (USB)', val: 2.0 },
        { label: '3.0A', val: 3.0 }
      ],
      1: [
        { label: '5W', val: 5 },
        { label: '10W', val: 10 },
        { label: '25W', val: 25 },
        { label: '50W', val: 50 },
        { label: '60W', val: 60 }
      ],
      2: [
        { label: '1Ω', val: 1 },
        { label: '2Ω', val: 2 },
        { label: '5Ω', val: 5 },
        { label: '10Ω', val: 10 },
        { label: '25Ω', val: 25 },
        { label: '50Ω', val: 50 },
        { label: '100Ω', val: 100 }
      ],
      3: [
        { label: '0.5A (NiMH 1.0V)', val: 0.5, cut: 1.0 },
        { label: '1.0A (LiFePO4 2.5V)', val: 1.0, cut: 2.5 },
        { label: '1.0A (Li-Ion 3.0V)', val: 1.0, cut: 3.0 },
        { label: '0.5A (Li-Ion 3.0V)', val: 0.5, cut: 3.0 },
        { label: '1.0A (Pb 10.5V)', val: 1.0, cut: 10.5 }
      ]
    };

    function renderPresets() {
      const box = document.getElementById('presetsBox');
      const list = PRESET_MAP[mode] || [];
      box.innerHTML = list.map((p, idx) => 
        `<button class="preset-btn" onclick="applyPresetIdx(${idx})">${p.label}</button>`
      ).join('');
    }

    function applyPresetIdx(idx) {
      const p = (PRESET_MAP[mode] || [])[idx];
      if (!p) return;
      currentSetpointVal = p.val;
      const lim = getLimitsForMode();
      document.getElementById('spInput').value = p.val.toFixed(lim.decimals);
      document.getElementById('spSlider').value = valueToSliderPos(p.val);
      if (typeof p.cut === 'number') {
        const cutEl = document.getElementById('valCut');
        if (cutEl) cutEl.innerText = p.cut.toFixed(2);
        pushSetpointAndCut(p.val, p.cut);
      } else {
        pushSetpoint(p.val);
      }
    }

    async function pushSetpointAndCut(val, cut) {
      try {
        const r = await fetch(`/api/control?mode=${mode}&setpoint=${val}&cut=${cut}`, { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {}
    }

    function syncModeConfig(m) {
      mode = m;
      document.querySelectorAll('.mode-btn').forEach((b, idx) => b.classList.toggle('active', idx === m));
      document.getElementById('battRow').style.display = (m === 3) ? 'block' : 'none';

      const t = I18N[currentLang] || I18N.cs;
      const inp = document.getElementById('spInput');
      if (m === 0 || m === 3) {
        document.getElementById('lblSp').innerText = t.lbl_sp_cc;
        document.getElementById('unitSp').innerText = 'A';
        inp.min = 0; inp.max = 5.2; inp.step = 0.025;
      } else if (m === 1) {
        document.getElementById('lblSp').innerText = t.lbl_sp_cp;
        document.getElementById('unitSp').innerText = 'W';
        inp.min = 0; inp.max = 65; inp.step = 0.5;
      } else if (m === 2) {
        document.getElementById('lblSp').innerText = t.lbl_sp_cr;
        document.getElementById('unitSp').innerText = 'Ω';
        inp.min = 0.5; inp.max = 500; inp.step = 0.5;
        // V režimu odporu (CR) nastavíme křivku posuvníku výchoze na LOG (0.5 až 500 Ω pokrývá 3 dekády)
        sliderCurve = 'log';
        document.getElementById('btnSliderLin').classList.remove('active');
        document.getElementById('btnSliderLog').classList.add('active');
      }
      renderPresets();
    }

    function setLinkStatus(ok) {
      const t = I18N[currentLang] || I18N.cs;
      const el = document.getElementById('linkStatus');
      if (!el) return;
      if (ok) {
        el.className = 'link-indicator link-ok';
        el.innerText = t.live;
      } else {
        el.className = 'link-indicator link-err';
        el.innerText = t.offline;
      }
    }

    function applyTelemetry(d) {
      setLinkStatus(true);
      document.getElementById('valV').innerText = d.voltage.toFixed(2);
      document.getElementById('valI').innerText = d.current.toFixed(3);
      document.getElementById('valP').innerText = d.power.toFixed(1);
      document.getElementById('valT').innerText = d.temperature.toFixed(1);
      document.getElementById('valFan').innerText = d.fan;
      document.getElementById('valMah').innerText = Math.round(d.discharged_mAh);
      document.getElementById('valWh').innerText = d.discharged_Wh.toFixed(2);
      if (typeof d.cutoff === 'number') {
        const cutEl = document.getElementById('valCut');
        if (cutEl) cutEl.innerText = d.cutoff.toFixed(2);
      }

      if (typeof d.mode === 'number' && d.mode !== mode) {
        syncModeConfig(d.mode);
      }

      const inp = document.getElementById('spInput');
      if (typeof d.setpoint === 'number' && !isDragging && !tOut && document.activeElement !== inp) {
        currentSetpointVal = d.setpoint;
        const lim = getLimitsForMode();
        inp.value = d.setpoint.toFixed(lim.decimals);
        document.getElementById('spSlider').value = valueToSliderPos(d.setpoint);
      }

      isOutput = d.output;
      isAlarm = (d.alarm !== 0);

      const ann = document.getElementById('stateAnn');
      const btn = document.getElementById('pwrBtn');
      const bar = document.getElementById('alarmBar');

      if (isAlarm) {
        const t = I18N[currentLang] || I18N.cs;
        ann.className = 'annunciator ann-alarm';
        ann.innerText = t.ann_alarm;
        bar.style.display = 'flex';
        document.getElementById('alarmReason').innerText = t.alarm_trip + d.alarmText;
        btn.className = 'power-btn';
        btn.innerText = t.pwr_tripped;
      } else {
        bar.style.display = 'none';
        setOutputVisuals(isOutput);
      }

      history.push({ u: d.voltage, i: d.current });
      if (history.length > LIMIT) history.shift();
      renderPlot();
    }

    async function poll() {
      if (inFlight) return;
      inFlight = true;
      try {
        const r = await fetch('/api/telemetry', { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        } else {
          setLinkStatus(false);
        }
      } catch (err) {
        setLinkStatus(false);
      } finally {
        inFlight = false;
      }
    }

    applyLanguage();
    renderPresets();
    poll();
    setInterval(poll, 300);

    async function selectMode(m) {
      syncModeConfig(m);
      try {
        const r = await fetch(`/api/control?mode=${m}`, { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {}
    }

    function onSliderMove(pos) {
      const val = sliderPosToValue(parseFloat(pos));
      currentSetpointVal = val;
      const lim = getLimitsForMode();
      const inp = document.getElementById('spInput');
      if (document.activeElement !== inp) {
        inp.value = val.toFixed(lim.decimals);
      }
      clearTimeout(tOut);
      tOut = setTimeout(() => {
        tOut = null;
        pushSetpoint(val);
      }, 80);
    }

    function submitNumericInput() {
      const inp = document.getElementById('spInput');
      let val = parseFloat(inp.value);
      if (isNaN(val)) return;

      const lim = getLimitsForMode();
      val = Math.max(lim.minLin, Math.min(lim.max, val));
      currentSetpointVal = val;
      inp.value = val.toFixed(lim.decimals);
      document.getElementById('spSlider').value = valueToSliderPos(val);
      pushSetpoint(val);
    }

    async function togglePower() {
      if (isAlarm) return;
      isOutput = !isOutput;
      setOutputVisuals(isOutput);

      try {
        const r = await fetch('/api/control?out=' + (isOutput ? '1' : '0'), { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {
        isOutput = !isOutput;
        setOutputVisuals(isOutput);
      }
    }

    async function ackAlarm() {
      try {
        const r = await fetch('/api/control?resetAlarm=1', { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {}
    }

    async function pushSetpoint(val) {
      try {
        const r = await fetch(`/api/control?mode=${mode}&setpoint=${val}`, { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {}
    }
  </script>
</body>
</html>
)rawliteral";

#endif // WEB_DASHBOARD_HTML_H