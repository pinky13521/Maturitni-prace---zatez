/**
 * @file web_dashboard_html.h
 * @brief Komplexní laboratorní rozhraní elektronické zátěže (DC Electronic Load HUD)
 *        Obsahuje záložky: Měření (Dashboard), Test baterií, Nastavení ochran/systému a Vzhled (Barevné motivy).
 */

#ifndef WEB_DASHBOARD_HTML_H
#define WEB_DASHBOARD_HTML_H

#include <Arduino.h>

const char DASHBOARD_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="cs">
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
      --panel-alt: #141414;
      --border: #2a2a2a;
      --border-focus: #555555;
      --text: #ffffff;
      --text-muted: #888888;
      --text-dim: #b8b8b8;
      --invert-bg: #ffffff;
      --invert-text: #000000;
      --alarm: #ff2a2a;
      --accent: #22ff66;
      --trace-u: #ffffff;
      --trace-i: #aaaaaa;
      --mono: ui-monospace, "SF Mono", Monaco, "Cascadia Code", "Roboto Mono", Consolas, monospace;
    }

    body.theme-amber {
      --bg: #0a0600;
      --panel: #140d02;
      --panel-alt: #201404;
      --border: #442a00;
      --border-focus: #885500;
      --text: #ffb400;
      --text-muted: #996a00;
      --text-dim: #c59835;
      --invert-bg: #ffb400;
      --invert-text: #000000;
      --alarm: #ff3b00;
      --accent: #ffb400;
      --trace-u: #ffcc00;
      --trace-i: #a67200;
    }

    body.theme-green {
      --bg: #000a02;
      --panel: #021706;
      --panel-alt: #04240a;
      --border: #004415;
      --border-focus: #00882b;
      --text: #2bff55;
      --text-muted: #158832;
      --text-dim: #44cc66;
      --invert-bg: #2bff55;
      --invert-text: #000000;
      --alarm: #ff2222;
      --accent: #2bff55;
      --trace-u: #55ff77;
      --trace-i: #189e37;
    }

    body.theme-cyan {
      --bg: #010912;
      --panel: #031526;
      --panel-alt: #062038;
      --border: #083c66;
      --border-focus: #0f6bb3;
      --text: #00e5ff;
      --text-muted: #007380;
      --text-dim: #44c8d9;
      --invert-bg: #00e5ff;
      --invert-text: #000000;
      --alarm: #ff2255;
      --accent: #00e5ff;
      --trace-u: #ffffff;
      --trace-i: #0099cc;
    }

    body.theme-light {
      --bg: #e8ebef;
      --panel: #ffffff;
      --panel-alt: #f4f6f8;
      --border: #c8d0d8;
      --border-focus: #7a8898;
      --text: #111822;
      --text-muted: #64748b;
      --text-dim: #475569;
      --invert-bg: #111822;
      --invert-text: #ffffff;
      --alarm: #e11d48;
      --accent: #0284c7;
      --trace-u: #0284c7;
      --trace-i: #64748b;
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
      transition: background 0.2s, color 0.2s;
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
      box-shadow: 0 10px 40px rgba(0,0,0,0.4);
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
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border-focus);
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
      background: var(--border-focus);
      color: var(--invert-text);
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
    .ann-alarm { background: #ff2222; color: #ffffff; border-color: #ff2222; animation: pulse 0.8s infinite alternate; }

    /* --- NAVIGAČNÍ ZÁLOŽKY (TABS) --- */
    .nav-bar {
      display: flex;
      background: var(--panel-alt);
      border-bottom: 1px solid var(--border);
      overflow-x: auto;
      scrollbar-width: none;
    }
    .nav-bar::-webkit-scrollbar { display: none; }

    .nav-btn {
      background: transparent;
      color: var(--text-muted);
      border: none;
      border-right: 1px solid var(--border);
      padding: 10px 18px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.1em;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 7px;
      white-space: nowrap;
      transition: background 0.1s, color 0.1s;
    }
    .nav-btn:hover {
      color: var(--text);
      background: var(--panel);
    }
    .nav-btn.active {
      color: var(--invert-text);
      background: var(--invert-bg);
    }

    .tab-content {
      display: none;
      flex-direction: column;
      gap: 1px;
    }
    .tab-content.active {
      display: flex;
    }

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
      color: var(--text-muted);
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
      color: var(--text);
      font-variant-numeric: tabular-nums;
    }

    .cell-unit {
      font-size: 11px;
      color: var(--text-muted);
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
      transition: background 0.1s, color 0.1s, transform 0.05s;
    }

    .power-btn:hover { border-color: var(--text); }
    .power-btn:active { transform: scale(0.99); }
    .power-btn.engaged {
      background: #ff2222;
      color: #ffffff;
      border-color: #ff2222;
    }

    /* POSUVNÍK + ČÍSELNÝ VSTUP */
    .slider-wrap {
      display: flex;
      flex-direction: column;
      gap: 8px;
    }

    .slider-legend {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 11px;
      letter-spacing: 0.05em;
    }

    .scale-selector {
      display: inline-flex;
      align-items: center;
      gap: 4px;
      border: 1px solid var(--border);
      padding: 2px 4px;
      background: var(--panel-alt);
    }

    .scale-label {
      font-size: 9px;
      color: var(--text-muted);
      letter-spacing: 0.08em;
      margin-right: 2px;
    }

    .scale-btn {
      background: transparent;
      color: var(--text-muted);
      border: none;
      font-size: 9px;
      font-weight: 700;
      padding: 2px 5px;
      cursor: pointer;
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
      color: #ffffff;
      border: 1px solid var(--border);
      padding: 5px 8px;
      font-size: 13px;
      font-weight: 700;
      width: 85px;
      text-align: right;
      outline: none;
    }
    .num-input:focus {
      border-color: var(--border-focus);
    }

    .sp-set-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 5px 10px;
      font-size: 10px;
      font-weight: 700;
      cursor: pointer;
    }
    .sp-set-btn:hover {
      background: var(--border-focus);
      color: var(--invert-text);
    }

    input[type=range] {
      -webkit-appearance: none;
      width: 100%;
      height: 6px;
      background: var(--border);
      outline: none;
    }
    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 18px;
      height: 22px;
      background: var(--invert-bg);
      cursor: pointer;
      border: 1px solid var(--border);
    }

    /* PŘEDVOLBY */
    .presets-row {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
    }

    .preset-label {
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.1em;
      text-transform: uppercase;
      font-weight: 700;
    }

    .preset-btn {
      background: var(--panel-alt);
      color: var(--text-dim);
      border: 1px solid var(--border);
      padding: 6px 12px;
      font-size: 11px;
      font-weight: 700;
      cursor: pointer;
      transition: background 0.1s, color 0.1s;
    }
    .preset-btn:hover {
      background: var(--border);
      color: var(--text);
    }

    /* --- GRAFICKÝ PANEL (OSCILLOSCOPE) --- */
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

    /* --- EXPORTNÍ LIŠTA A ZÁZNAM DAT --- */
    .export-bar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 10px;
      padding-top: 10px;
      border-top: 1px solid var(--border);
      flex-wrap: wrap;
    }

    .export-left {
      display: flex;
      align-items: center;
      gap: 10px;
    }

    .export-right {
      display: flex;
      align-items: center;
      gap: 6px;
      flex-wrap: wrap;
    }

    .rec-btn {
      background: #141414;
      color: #eeeeee;
      border: 1px solid #444444;
      padding: 6px 12px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.08em;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 7px;
      border-radius: 0;
      transition: background 0.15s, border-color 0.15s, color 0.15s;
    }
    .rec-btn:hover {
      background: #222222;
      border-color: #777777;
    }
    .rec-btn.recording {
      background: #2a0000;
      border-color: #ff3333;
      color: #ff5555;
    }

    .rec-dot {
      width: 7px;
      height: 7px;
      border-radius: 50%;
      background: #666666;
      display: inline-block;
    }
    .rec-btn.recording .rec-dot {
      background: #ff3333;
      box-shadow: 0 0 6px #ff3333;
      animation: pulse 1s infinite alternate;
    }

    @keyframes pulse {
      0% { opacity: 0.3; }
      100% { opacity: 1; }
    }

    .rec-status {
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.08em;
      font-variant-numeric: tabular-nums;
    }

    .exp-btn {
      background: #0d0d0d;
      color: #b0b0b0;
      border: 1px solid #383838;
      padding: 5px 10px;
      font-size: 10px;
      font-weight: 700;
      letter-spacing: 0.06em;
      cursor: pointer;
      border-radius: 0;
      transition: background 0.1s, color 0.1s, border-color 0.1s;
    }
    .exp-btn:hover {
      background: #1c1c1c;
      color: #ffffff;
      border-color: #666666;
    }
    .exp-btn:active {
      transform: scale(0.98);
    }
    .exp-clear:hover {
      color: #ff5555;
      border-color: #aa2222;
    }

    /* --- KARTY PRO NASTAVENÍ A TEST BATERIÍ --- */
    .card-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(290px, 1fr));
      gap: 1px;
      background: var(--border);
    }

    .info-card {
      background: var(--panel);
      padding: 18px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }

    .card-title {
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.12em;
      text-transform: uppercase;
      color: var(--text);
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding-bottom: 8px;
      border-bottom: 1px solid var(--border);
    }

    .card-badge {
      font-size: 9px;
      padding: 2px 6px;
      border: 1px solid var(--border-focus);
      color: var(--text-muted);
    }
    .badge-ok { color: #22ff66; border-color: #1a552a; background: #001a08; }
    .badge-warn { color: #ffb400; border-color: #885500; background: #221500; }

    .prop-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 11px;
      padding: 3px 0;
    }
    .prop-label {
      color: var(--text-muted);
    }
    .prop-val {
      font-weight: 700;
      font-variant-numeric: tabular-nums;
      color: var(--text);
    }

    /* BATERIOVÁ SEKCE */
    .batt-big-grid {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 1px;
      background: var(--border);
    }

    .batt-box {
      background: var(--panel);
      padding: 16px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      min-height: 85px;
    }

    /* TÉMATA (VZHLED) */
    .theme-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
      gap: 8px;
      margin-top: 6px;
    }

    .theme-card {
      background: var(--panel-alt);
      border: 1px solid var(--border);
      padding: 12px;
      cursor: pointer;
      display: flex;
      align-items: center;
      gap: 10px;
      transition: border-color 0.15s, background 0.15s;
    }
    .theme-card:hover {
      border-color: var(--border-focus);
    }
    .theme-card.active {
      border-color: var(--text);
      background: var(--panel);
    }

    .theme-swatch {
      width: 22px;
      height: 22px;
      border-radius: 50%;
      border: 1px solid rgba(255,255,255,0.2);
    }

    /* ALARM BANNER */
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
      .batt-big-grid { grid-template-columns: repeat(2, 1fr); }
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
      .export-bar { flex-direction: column; align-items: stretch; gap: 8px; }
      .export-left { justify-content: space-between; }
      .export-right { display: grid; grid-template-columns: repeat(2, 1fr); gap: 4px; }
      .exp-btn { text-align: center; padding: 8px 4px; }
    }
  </style>
</head>
<body class="theme-dark">

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

    <!-- TABOVÁ NAVIGACE -->
    <nav class="nav-bar">
      <button class="nav-btn active" id="btnTabMain" onclick="switchTab('main')">
        <span>📊</span> <span data-i18n="tab_main">MĚŘENÍ</span>
      </button>
      <button class="nav-btn" id="btnTabBatt" onclick="switchTab('batt')">
        <span>🔋</span> <span data-i18n="tab_batt">TEST BATERIÍ</span>
      </button>
      <button class="nav-btn" id="btnTabSettings" onclick="switchTab('settings')">
        <span>⚙️</span> <span data-i18n="tab_settings">NASTAVENÍ</span>
      </button>
      <button class="nav-btn" id="btnTabTheme" onclick="switchTab('theme')">
        <span>🎨</span> <span data-i18n="tab_theme">VZHLED</span>
      </button>
    </nav>

    <!-- ================================================================= -->
    <!-- ZÁLOŽKA 1: HLAVNÍ MĚŘENÍ A REGULACE (DASHBOARD)                   -->
    <!-- ================================================================= -->
    <div id="tabMain" class="tab-content active">
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

        <!-- Rychlý přehled pro režim baterie -->
        <div id="battRow" style="display:none; font-size:11px; color:var(--text-dim); border-top:1px solid var(--border); padding-top:10px;">
          <span data-i18n="lbl_discharged">VYBITO:</span> <strong id="valMah" style="color:var(--text);">0</strong> mAh &bull; <strong id="valWh" style="color:var(--text);">0.00</strong> Wh &bull; <span data-i18n="lbl_cutoff">MEZNÍ NAPĚTÍ:</span> <strong id="valCut" style="color:var(--text);">3.00</strong> V
        </div>
      </div>

      <!-- Osciloskopický graticule log s přepínačem LIN/LOG stupnice a exportem -->
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

        <!-- Panel záznamu měření a exportu dat (Excel CSV, JSON, PNG graf) -->
        <div class="export-bar">
          <div class="export-left">
            <button id="btnRec" class="rec-btn" onclick="toggleRecording()">
              <span class="rec-dot"></span> <span id="lblRecBtn" data-i18n="rec_start">ZÁZNAM</span>
            </button>
            <span id="recStatus" class="rec-status" data-i18n="rec_idle">ZÁZNAM: NEAKTIVNÍ</span>
          </div>
          <div class="export-right">
            <button class="exp-btn" onclick="exportCSV()" title="Exportovat data do Excelu (.csv)" data-i18n="btn_csv">📥 CSV / EXCEL</button>
            <button class="exp-btn" onclick="exportJSON()" title="Exportovat surová data (.json)" data-i18n="btn_json">📥 JSON</button>
            <button class="exp-btn" onclick="exportPNG()" title="Uložit snímek grafu jako obrázek (.png)" data-i18n="btn_png">📷 PNG GRAF</button>
            <button class="exp-btn exp-clear" onclick="clearLogData()" title="Smazat zaznamenaná data" data-i18n="btn_clear">✕ SMAZAT</button>
          </div>
        </div>
      </div>
    </div>

    <!-- ================================================================= -->
    <!-- ZÁLOŽKA 2: TESTOVÁNÍ BATERIÍ (BATT ANALYZER)                     -->
    <!-- ================================================================= -->
    <div id="tabBatt" class="tab-content">
      <div class="batt-big-grid">
        <div class="batt-box">
          <div class="cell-tag" data-i18n="batt_cap">KAPACITA</div>
          <div class="cell-num"><span id="battBigMah">0</span> <span style="font-size:14px;color:var(--text-muted);">mAh</span></div>
          <div class="cell-unit"><span id="battBigAh">0.000</span> Ah</div>
        </div>
        <div class="batt-box">
          <div class="cell-tag" data-i18n="batt_energy">ENERGIE</div>
          <div class="cell-num"><span id="battBigWh">0.00</span> <span style="font-size:14px;color:var(--text-muted);">Wh</span></div>
          <div class="cell-unit" data-i18n="batt_integral">INTEGRÁL U &bull; I &bull; dt</div>
        </div>
        <div class="batt-box">
          <div class="cell-tag" data-i18n="batt_time">DOBA TESTU</div>
          <div class="cell-num"><span id="battTimer">00:00:00</span></div>
          <div class="cell-unit" data-i18n="batt_elapsed">ČAS OD STARTU</div>
        </div>
        <div class="batt-box">
          <div class="cell-tag" data-i18n="batt_ri">VNITŘNÍ ODPOR (Ri)</div>
          <div class="cell-num"><span id="battRi">--</span> <span style="font-size:14px;color:var(--text-muted);">mΩ</span></div>
          <div class="cell-unit" data-i18n="batt_ri_calc">&Delta;U / &Delta;I ODHAD</div>
        </div>
      </div>

      <div class="control-deck">
        <div class="card-title">
          <span data-i18n="batt_cfg_title">KONFIGURACE TESTU VYBÍJENÍ AKUMULÁTORU</span>
          <span class="card-badge badge-ok" id="battStatusBadge" data-i18n="batt_ready">PŘIPRAVENO</span>
        </div>

        <div class="presets-row" style="margin-top:4px;">
          <span class="preset-label" data-i18n="batt_chem">CHEMIE ČLÁNKU:</span>
          <button class="preset-btn" onclick="applyBattPreset(3.0, 1.0)">Li-Ion / LiPo (3.0V)</button>
          <button class="preset-btn" onclick="applyBattPreset(2.5, 1.0)">LiFePO4 (2.5V)</button>
          <button class="preset-btn" onclick="applyBattPreset(1.0, 0.5)">NiMH / NiCd (1.0V)</button>
          <button class="preset-btn" onclick="applyBattPreset(10.5, 1.0)">Pb 12V (10.5V)</button>
        </div>

        <div class="deck-row" style="margin-top:8px;">
          <div style="display:flex; align-items:center; gap:14px; flex-wrap:wrap;">
            <div style="display:flex; align-items:center; gap:6px;">
              <span class="prop-label" data-i18n="batt_discharge_curr">VYBÍJECÍ PROUD:</span>
              <input type="number" id="battInputCurr" class="num-input" value="1.000" step="0.1" min="0.05" max="5.2">
              <span class="prop-val">A</span>
            </div>
            <div style="display:flex; align-items:center; gap:6px;">
              <span class="prop-label" data-i18n="batt_cutoff_volt">MEZNÍ VYPNUTÍ (CUTOFF):</span>
              <input type="number" id="battInputCut" class="num-input" value="3.00" step="0.1" min="0.5" max="30">
              <span class="prop-val">V</span>
            </div>
          </div>
          <button id="battTestBtn" class="power-btn" onclick="toggleBatteryTest()" data-i18n="batt_start_btn">▶ SPUSTIT TEST</button>
        </div>
      </div>
    </div>

    <!-- ================================================================= -->
    <!-- ZÁLOŽKA 3: NASTAVENÍ A BEZPEČNOST (SETTINGS & SYSTEM)             -->
    <!-- ================================================================= -->
    <div id="tabSettings" class="tab-content">
      <div class="card-grid">
        <!-- Karta 1: Bezpečnostní dohled -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="set_safety_title">BEZPEČNOSTNÍ DOHLED (SUPERVISOR)</span>
            <span class="card-badge badge-ok">AKTIVNÍ</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_ovp">Přepěťová ochrana (OVP):</span>
            <span class="prop-val">32.0 V</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_ocp">Nadproudová ochrana (OCP):</span>
            <span class="prop-val">5.20 A</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_opp">Výkonová ochrana (OPP):</span>
            <span class="prop-val">65.0 W (špičkově 100W)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_otp">Tepelná ochrana (OTP):</span>
            <span class="prop-val">75.0 °C</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_otp_warn">Výstraha ventilátoru (100%):</span>
            <span class="prop-val">65.0 °C</span>
          </div>
        </div>

        <!-- Karta 2: Chlazení a ventilátor -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="set_fan_title">CHLAZENÍ A VENTILÁTOR</span>
            <span class="card-badge badge-ok" id="setFanModeBadge">PWM AUTO</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_fan_freq">Frekvence PWM:</span>
            <span class="prop-val">25 kHz (Standard 4-pin PC)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_fan_min">Náběh chlazení (20%):</span>
            <span class="prop-val">32.0 °C</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_fan_max">Plné otáčky (100%):</span>
            <span class="prop-val">60.0 °C</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_fan_duty">Aktuální střída ventilátoru:</span>
            <span class="prop-val"><span id="setFanLive">20</span> %</span>
          </div>
        </div>

        <!-- Karta 3: Hardwarové konstanty -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="set_hw_title">HARDWAROVÉ KONSTANTY</span>
            <span class="card-badge">KALIBROVÁNO</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_shunt">Proudový bočník R_shunt:</span>
            <span class="prop-val">0.100 Ω (10W Kelvin 4-wire)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_divider">Napěťový dělič vstupu:</span>
            <span class="prop-val">1:11.00 (100k / 10k)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_adc">A/D Převodník:</span>
            <span class="prop-val">ADS1115 (16-bit I2C 0x48)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_dac">D/A Převodník:</span>
            <span class="prop-val">MCP4725 (12-bit I2C 0x60)</span>
          </div>
        </div>

        <!-- Karta 4: Systém a mikrokontrolér -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="set_mcu_title">SYSTÉM &amp; MIKROKONTROLÉR</span>
            <span class="card-badge">ESP32</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">MCU:</span>
            <span class="prop-val">ESP32 DevKit V1 (240 MHz)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_loop">Perioda regulační smyčky:</span>
            <span class="prop-val">20 ms (50 Hz)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_api">Telemetrické rozhraní:</span>
            <span class="prop-val">JSON HTTP REST (300 ms)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">Firmware:</span>
            <span class="prop-val">v2.5 Modular Edition</span>
          </div>
        </div>
      </div>
    </div>

    <!-- ================================================================= -->
    <!-- ZÁLOŽKA 4: VZHLED, MOTIVY A ZVUK (THEMES & CUSTOMIZATION)         -->
    <!-- ================================================================= -->
    <div id="tabTheme" class="tab-content">
      <div class="card-grid">
        <!-- Karta motivů -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="theme_select_title">BAREVNÝ MOTIV ROZHRANÍ (COLOR THEMES)</span>
            <span class="card-badge" id="themeActiveBadge">CYBER DARK</span>
          </div>

          <div class="theme-grid">
            <div class="theme-card active" id="thCardDark" onclick="applyTheme('dark')">
              <span class="theme-swatch" style="background:#000000;border-color:#ffffff;"></span>
              <div>
                <strong style="font-size:11px;">Monochrome Dark</strong>
                <div style="font-size:9px;color:var(--text-muted);">Černobílý laboratorní styl</div>
              </div>
            </div>

            <div class="theme-card" id="thCardAmber" onclick="applyTheme('amber')">
              <span class="theme-swatch" style="background:#ffb400;"></span>
              <div>
                <strong style="font-size:11px;">Keithley Amber</strong>
                <div style="font-size:9px;color:var(--text-muted);">Klasický jantarový HUD</div>
              </div>
            </div>

            <div class="theme-card" id="thCardGreen" onclick="applyTheme('green')">
              <span class="theme-swatch" style="background:#2bff55;"></span>
              <div>
                <strong style="font-size:11px;">CRT Phosphor</strong>
                <div style="font-size:9px;color:var(--text-muted);">Zelený osciloskop / Matrix</div>
              </div>
            </div>

            <div class="theme-card" id="thCardCyan" onclick="applyTheme('cyan')">
              <span class="theme-swatch" style="background:#00e5ff;"></span>
              <div>
                <strong style="font-size:11px;">Neon Cyan</strong>
                <div style="font-size:9px;color:var(--text-muted);">Kybernetická modrá</div>
              </div>
            </div>

            <div class="theme-card" id="thCardLight" onclick="applyTheme('light')">
              <span class="theme-swatch" style="background:#ffffff;border-color:#333333;"></span>
              <div>
                <strong style="font-size:11px;">Clean Paper</strong>
                <div style="font-size:9px;color:var(--text-muted);">Světlý tiskový protokol</div>
              </div>
            </div>
          </div>
        </div>

        <!-- Karta chování a zvuku -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="theme_opts_title">PŘIZPŮSOBENÍ ROZHRANÍ A ZVUKU</span>
            <span class="card-badge">WEB AUDIO</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="theme_sound">Zvuková odezva (Pípnutí / Beeper):</span>
            <button class="sp-set-btn" id="btnAudioToggle" onclick="toggleAudioFeedback()">ZAPNUTO</button>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="theme_trace_pts">Délka osciloskopické paměti:</span>
            <div class="scale-selector">
              <button class="scale-btn active" id="btnPts50" onclick="setPlotLimit(50)">50</button>
              <button class="scale-btn" id="btnPts100" onclick="setPlotLimit(100)">100</button>
              <button class="scale-btn" id="btnPts150" onclick="setPlotLimit(150)">150</button>
            </div>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="theme_lang">Jazyk rozhraní:</span>
            <button class="sp-set-btn" onclick="toggleLanguage()"><span id="themeLangLabel">ČEŠTINA (CZ)</span></button>
          </div>
        </div>
      </div>
    </div>
  </div>

  <script>
    let mode = 0;
    let isOutput = false;
    let isAlarm = false;
    let inFlight = false;
    let isDragging = false;
    let plotScale = 'lin';
    let sliderCurve = 'lin';
    let currentSetpointVal = 0.500;
    let tOut = null;
    let currentLang = localStorage.getItem('load_lang') || 'cs';
    let currentTheme = localStorage.getItem('load_theme') || 'dark';
    let soundEnabled = (localStorage.getItem('load_sound') !== 'false');
    let isRecording = false;
    let recordStartTime = null;
    let battTestStartTime = null;
    let lastVoltageUnloaded = 0.0;
    const logData = [];
    const history = [];
    let LIMIT = 50;

    // Inicializace Web Audio pro laboratorní pípnutí
    const audioCtx = (window.AudioContext || window.webkitAudioContext) ? new (window.AudioContext || window.webkitAudioContext)() : null;
    function playBeep(freq = 880, dur = 0.12, type = 'sine') {
      if (!soundEnabled || !audioCtx) return;
      try {
        if (audioCtx.state === 'suspended') audioCtx.resume();
        const osc = audioCtx.createOscillator();
        const g = audioCtx.createGain();
        osc.type = type;
        osc.frequency.value = freq;
        g.gain.setValueAtTime(0.04, audioCtx.currentTime);
        g.gain.exponentialRampToValueAtTime(0.0001, audioCtx.currentTime + dur);
        osc.connect(g);
        g.connect(audioCtx.destination);
        osc.start();
        osc.stop(audioCtx.currentTime + dur);
      } catch(e) {}
    }

    const I18N = {
      cs: {
        lang_btn: '[ EN ]',
        header_title: 'DC ELEKTRONICKÁ ZÁTĚŽ // 30V 5A',
        header_sub: 'DIGITÁLNÍ REGULACE • 50 HZ SMYČKA',
        live: '● ŽIVĚ',
        offline: '○ ODPOJENO',
        full_enter: '[ ⛶ FULL ]',
        full_exit: '[ ✕ ZAVŘÍT ]',
        tab_main: 'MĚŘENÍ',
        tab_batt: 'TEST BATERIÍ',
        tab_settings: 'NASTAVENÍ',
        tab_theme: 'VZHLED',
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
        lbl_graph: 'OSCILOSKOPICKÝ ZÁZNAM',
        lbl_graph_sub: '• U [BÍLÁ] • I [ŠEDÁ]',
        lbl_yscale: 'MĚŘÍTKO Y:',
        btn_ack: '[ POTVRDIT / RESET ]',
        alarm_default: 'SYSTÉMOVÝ ALARM: VYBAVENÍ OCHRANY',
        alarm_trip: 'OCHRANA: ',
        rec_start: 'ZÁZNAM',
        rec_stop: 'STOP',
        rec_idle: 'ZÁZNAM: NEAKTIVNÍ',
        rec_active: 'ZÁZNAM: ',
        rec_pts: ' vzorků',
        rec_saved: 'ULOŽENO: ',
        csv_header: 'Čas [s];Čas;Režim;Napětí [V];Proud [A];Výkon [W];Teplota [°C];Ventilátor [%];Kapacita [mAh];Energie [Wh]',
        btn_csv: '📥 CSV / EXCEL',
        btn_json: '📥 JSON',
        btn_png: '📷 PNG GRAF',
        btn_clear: '✕ SMAZAT',
        no_data: 'Žádná data k exportu! Nejdříve spusťte zátěž nebo zapněte ZÁZNAM.',
        confirm_clear: 'Opravdu chcete smazat všechna zaznamenaná data měření?',
        batt_cap: 'KAPACITA',
        batt_energy: 'ENERGIE',
        batt_time: 'DOBA TESTU',
        batt_elapsed: 'ČAS OD STARTU',
        batt_ri: 'VNITŘNÍ ODPOR (Ri)',
        batt_ri_calc: 'ΔU / ΔI ODHAD',
        batt_cfg_title: 'KONFIGURACE TESTU VYBÍJENÍ AKUMULÁTORU',
        batt_ready: 'PŘIPRAVENO',
        batt_running: 'VYBÍJENÍ PROBÍHÁ',
        batt_chem: 'CHEMIE ČLÁNKU:',
        batt_discharge_curr: 'VYBÍJECÍ PROUD:',
        batt_cutoff_volt: 'MEZNÍ VYPNUTÍ (CUTOFF):',
        batt_start_btn: '▶ SPUSTIT TEST',
        batt_stop_btn: '⏹ UKONČIT TEST',
        set_safety_title: 'BEZPEČNOSTNÍ DOHLED (SUPERVISOR)',
        set_ovp: 'Přepěťová ochrana (OVP):',
        set_ocp: 'Nadproudová ochrana (OCP):',
        set_opp: 'Výkonová ochrana (OPP):',
        set_otp: 'Tepelná ochrana (OTP):',
        set_otp_warn: 'Výstraha ventilátoru (100%):',
        set_fan_title: 'CHLAZENÍ A VENTILÁTOR',
        set_fan_freq: 'Frekvence PWM:',
        set_fan_min: 'Náběh chlazení (20%):',
        set_fan_max: 'Plné otáčky (100%):',
        set_fan_duty: 'Aktuální střída ventilátoru:',
        set_hw_title: 'HARDWAROVÉ KONSTANTY',
        set_shunt: 'Proudový bočník R_shunt:',
        set_divider: 'Napěťový dělič vstupu:',
        set_adc: 'A/D Převodník:',
        set_dac: 'D/A Převodník:',
        set_mcu_title: 'SYSTÉM & MIKROKONTROLÉR',
        set_loop: 'Perioda regulační smyčky:',
        set_api: 'Telemetrické rozhraní:',
        theme_select_title: 'BAREVNÝ MOTIV ROZHRANÍ (COLOR THEMES)',
        theme_opts_title: 'PŘIZPŮSOBENÍ ROZHRANÍ A ZVUKU',
        theme_sound: 'Zvuková odezva (Pípnutí / Beeper):',
        theme_trace_pts: 'Délka osciloskopické paměti:',
        theme_lang: 'Jazyk rozhraní:'
      },
      en: {
        lang_btn: '[ CZ ]',
        header_title: 'DC ELECTRONIC LOAD // 30V 5A',
        header_sub: 'DIGITAL REGULATION • 50 HZ LOOP',
        live: '● LIVE',
        offline: '○ OFFLINE',
        full_enter: '[ ⛶ FULL ]',
        full_exit: '[ ✕ EXIT ]',
        tab_main: 'MAIN',
        tab_batt: 'BATT TEST',
        tab_settings: 'SETTINGS',
        tab_theme: 'APPEARANCE',
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
        lbl_graph: 'GRATICULE REALTIME LOG',
        lbl_graph_sub: '• U [WHT] • I [GRY]',
        lbl_yscale: 'Y-SCALE:',
        btn_ack: '[ ACK / RESET ]',
        alarm_default: 'SYS_ALARM: TRIP OCCURRED',
        alarm_trip: 'TRIP: ',
        rec_start: 'REC',
        rec_stop: 'STOP',
        rec_idle: 'LOG: IDLE',
        rec_active: 'LOG: ',
        rec_pts: ' samples',
        rec_saved: 'SAVED: ',
        csv_header: 'Time [s];Time;Mode;Voltage [V];Current [A];Power [W];Temp [C];Fan [%];Capacity [mAh];Energy [Wh]',
        btn_csv: '📥 CSV / EXCEL',
        btn_json: '📥 JSON',
        btn_png: '📷 PNG PLOT',
        btn_clear: '✕ CLEAR',
        no_data: 'No data to export! Please run load or press REC first.',
        confirm_clear: 'Do you really want to clear all recorded measurement data?',
        batt_cap: 'CAPACITY',
        batt_energy: 'ENERGY',
        batt_time: 'TEST TIME',
        batt_elapsed: 'ELAPSED DURATION',
        batt_ri: 'INTERNAL RESISTANCE (Ri)',
        batt_ri_calc: 'ΔU / ΔI ESTIMATE',
        batt_cfg_title: 'BATTERY DISCHARGE TEST CONFIGURATION',
        batt_ready: 'READY',
        batt_running: 'DISCHARGING',
        batt_chem: 'CELL CHEMISTRY:',
        batt_discharge_curr: 'DISCHARGE CURRENT:',
        batt_cutoff_volt: 'CUTOFF VOLTAGE:',
        batt_start_btn: '▶ START TEST',
        batt_stop_btn: '⏹ STOP TEST',
        set_safety_title: 'SAFETY SUPERVISOR LIMITS',
        set_ovp: 'Over-Voltage Protection (OVP):',
        set_ocp: 'Over-Current Protection (OCP):',
        set_opp: 'Over-Power Protection (OPP):',
        set_otp: 'Over-Temperature Protection (OTP):',
        set_otp_warn: 'Fan Full Speed Warning (100%):',
        set_fan_title: 'COOLING & FAN PWM',
        set_fan_freq: 'PWM Frequency:',
        set_fan_min: 'Cooling Start (20%):',
        set_fan_max: 'Full Speed (100%):',
        set_fan_duty: 'Current Fan Duty:',
        set_hw_title: 'HARDWARE CALIBRATION CONSTANTS',
        set_shunt: 'Shunt Resistor R_shunt:',
        set_divider: 'Input Voltage Divider:',
        set_adc: 'ADC Converter:',
        set_dac: 'DAC Converter:',
        set_mcu_title: 'SYSTEM & MCU',
        set_loop: 'Regulation Loop Period:',
        set_api: 'Telemetry Interface:',
        theme_select_title: 'COLOR THEMES',
        theme_opts_title: 'CUSTOMIZATION & AUDIO',
        theme_sound: 'Audio Feedback (Beeper):',
        theme_trace_pts: 'Oscilloscope Memory Points:',
        theme_lang: 'Language:'
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
      const themeLang = document.getElementById('themeLangLabel');
      if (themeLang) themeLang.innerText = (currentLang === 'cs') ? 'ČEŠTINA (CZ)' : 'ENGLISH (EN)';
      const fsBtn = document.getElementById('fsBtn');
      if (fsBtn) {
        fsBtn.innerText = document.fullscreenElement ? t.full_exit : t.full_enter;
      }
      const lblRec = document.getElementById('lblRecBtn');
      if (lblRec) lblRec.innerText = isRecording ? t.rec_stop : t.rec_start;
      const bTestBtn = document.getElementById('battTestBtn');
      if (bTestBtn) bTestBtn.innerText = (mode === 3 && isOutput) ? t.batt_stop_btn : t.batt_start_btn;
      updateRecStatus();
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

    /* PŘEPÍNÁNÍ ZÁLOŽEK */
    function switchTab(tabId) {
      playBeep(1200, 0.05);
      document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));

      if (tabId === 'main') {
        document.getElementById('btnTabMain').classList.add('active');
        document.getElementById('tabMain').classList.add('active');
        setTimeout(resize, 60);
      } else if (tabId === 'batt') {
        document.getElementById('btnTabBatt').classList.add('active');
        document.getElementById('tabBatt').classList.add('active');
      } else if (tabId === 'settings') {
        document.getElementById('btnTabSettings').classList.add('active');
        document.getElementById('tabSettings').classList.add('active');
      } else if (tabId === 'theme') {
        document.getElementById('btnTabTheme').classList.add('active');
        document.getElementById('tabTheme').classList.add('active');
      }
    }

    /* BAREVNÉ MOTIVY */
    function applyTheme(th) {
      playBeep(900, 0.08);
      currentTheme = th;
      localStorage.setItem('load_theme', th);
      document.body.className = `theme-${th}`;

      document.querySelectorAll('.theme-card').forEach(c => c.classList.remove('active'));
      const activeCard = document.getElementById('thCard' + th.charAt(0).toUpperCase() + th.slice(1));
      if (activeCard) activeCard.classList.add('active');
      const badge = document.getElementById('themeActiveBadge');
      if (badge) badge.innerText = th.toUpperCase();
      renderPlot();
    }

    function toggleAudioFeedback() {
      soundEnabled = !soundEnabled;
      localStorage.setItem('load_sound', soundEnabled ? 'true' : 'false');
      const btn = document.getElementById('btnAudioToggle');
      if (btn) btn.innerText = soundEnabled ? (currentLang === 'cs' ? 'ZAPNUTO' : 'ENABLED') : (currentLang === 'cs' ? 'VYPNUTO' : 'DISABLED');
      if (soundEnabled) playBeep(1000, 0.1);
    }

    function setPlotLimit(pts) {
      LIMIT = pts;
      document.getElementById('btnPts50').classList.toggle('active', pts === 50);
      document.getElementById('btnPts100').classList.toggle('active', pts === 100);
      document.getElementById('btnPts150').classList.toggle('active', pts === 150);
      renderPlot();
    }

    const cv = document.getElementById('cv');
    const ctx = cv.getContext('2d');
    let dpr = window.devicePixelRatio || 1;
    let cssW = 900;
    let cssH = 190;

    function resize() {
      dpr = window.devicePixelRatio || 1;
      const rect = cv.getBoundingClientRect();
      cssW = rect.width;
      if (!cssW || cssW < 50) {
        const parentW = cv.parentElement ? cv.parentElement.clientWidth : 0;
        const pad = window.innerWidth < 680 ? 24 : 36;
        cssW = Math.max(200, parentW - pad);
      }
      cssH = window.innerWidth < 680 ? 160 : 190;
      cv.width = Math.round(cssW * dpr);
      cv.height = Math.round(cssH * dpr);
      renderPlot();
    }
    window.addEventListener('resize', resize);
    window.addEventListener('orientationchange', () => setTimeout(resize, 200));
    resize();

    function toggleFullscreen() {
      if (!document.fullscreenElement) {
        document.documentElement.requestFullscreen().catch(() => {});
      } else {
        if (document.exitFullscreen) document.exitFullscreen();
      }
    }

    document.addEventListener('fullscreenchange', () => {
      const btn = document.getElementById('fsBtn');
      const t = I18N[currentLang] || I18N.cs;
      if (btn) btn.innerText = document.fullscreenElement ? t.full_exit : t.full_enter;
      setTimeout(resize, 150);
    });

    function setPlotScale(s) {
      playBeep(1100, 0.04);
      plotScale = s;
      document.getElementById('btnScaleLin').classList.toggle('active', s === 'lin');
      document.getElementById('btnScaleLog').classList.toggle('active', s === 'log');
      renderPlot();
    }

    function setSliderCurve(c) {
      playBeep(1100, 0.04);
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

    /* --- VYKRESLOVÁNÍ GRAFU (LIN / LOG) S PODPOROU MOTIVŮ --- */
    function renderPlot() {
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
      ctx.clearRect(0, 0, cssW, cssH);
      const w = cssW;
      const h = cssH;

      const pL = 38;
      const pR = w - 42;
      const pT = 12;
      const pB = h - 16;
      const plotW = pR - pL;
      const plotH = pB - pT;

      const styles = getComputedStyle(document.body);
      const colBorder = styles.getPropertyValue('--border-focus').trim() || '#3e3e3e';
      const colGrid = styles.getPropertyValue('--border').trim() || '#222222';
      const colText = styles.getPropertyValue('--text').trim() || '#ffffff';
      const colMuted = styles.getPropertyValue('--text-muted').trim() || '#cccccc';
      const colTraceU = styles.getPropertyValue('--trace-u').trim() || '#ffffff';
      const colTraceI = styles.getPropertyValue('--trace-i').trim() || '#aaaaaa';

      // Ohraničení rastru
      ctx.strokeStyle = colBorder;
      ctx.lineWidth = 1;
      ctx.strokeRect(pL, pT, plotW, plotH);

      ctx.font = '600 11px ui-monospace, "SF Mono", Monaco, "Cascadia Code", "Roboto Mono", Consolas, monospace';
      ctx.textBaseline = 'middle';

      if (plotScale === 'lin') {
        const divs = 4;
        for (let i = 0; i <= divs; i++) {
          const y = pB - (i / divs) * plotH;
          if (i > 0 && i < divs) {
            ctx.strokeStyle = colGrid;
            ctx.beginPath(); ctx.moveTo(pL, y); ctx.lineTo(pR, y); ctx.stroke();
          }
          const vVal = (i / divs) * 32.0;
          ctx.fillStyle = colText;
          ctx.textAlign = 'right';
          ctx.fillText(vVal.toFixed(0) + 'V', pL - 6, y);

          const iVal = (i / divs) * 5.2;
          ctx.fillStyle = colMuted;
          ctx.textAlign = 'left';
          ctx.fillText(iVal.toFixed(1) + 'A', pR + 6, y);
        }
      } else {
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
          ctx.strokeStyle = t.major ? colBorder : colGrid;
          ctx.beginPath(); ctx.moveTo(pL, y); ctx.lineTo(pR, y); ctx.stroke();

          if (t.label) {
            ctx.fillStyle = colText;
            ctx.textAlign = 'right';
            ctx.fillText(t.label, pL - 6, y);
          }
        });

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
          ctx.fillStyle = colMuted;
          ctx.textAlign = 'left';
          ctx.fillText(t.label, pR + 6, y);
        });
      }

      // Vertikální mřížka
      const xDivs = 8;
      for (let j = 1; j < xDivs; j++) {
        const x = pL + (plotW / xDivs) * j;
        ctx.strokeStyle = colGrid;
        ctx.beginPath(); ctx.moveTo(x, pT); ctx.lineTo(x, pB); ctx.stroke();
      }

      if (history.length < 2) return;

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
        plotTrace('u', false, 0, 32.0, colTraceU, 2.0);
        plotTrace('i', false, 0, 5.2, colTraceI, 1.8);
      } else {
        plotTrace('u', true, 0.1, 32.0, colTraceU, 2.0);
        plotTrace('i', true, 0.001, 5.2, colTraceI, 1.8);
      }

      ctx.restore();
    }

    function setOutputVisuals(active) {
      const t = I18N[currentLang] || I18N.cs;
      const ann = document.getElementById('stateAnn');
      const btn = document.getElementById('pwrBtn');
      const bTestBtn = document.getElementById('battTestBtn');
      if (isAlarm) return;
      if (active) {
        ann.className = 'annunciator ann-on';
        ann.innerText = t.ann_on;
        btn.className = 'power-btn engaged';
        btn.innerText = t.pwr_active;
        if (bTestBtn) bTestBtn.innerText = t.batt_stop_btn;
      } else {
        ann.className = 'annunciator ann-off';
        ann.innerText = t.ann_off;
        btn.className = 'power-btn';
        btn.innerText = t.pwr_off;
        if (bTestBtn) bTestBtn.innerText = t.batt_start_btn;
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
      playBeep(1000, 0.05);
      const p = (PRESET_MAP[mode] || [])[idx];
      if (!p) return;
      currentSetpointVal = p.val;
      const lim = getLimitsForMode();
      document.getElementById('spInput').value = p.val.toFixed(lim.decimals);
      document.getElementById('spSlider').value = valueToSliderPos(p.val);
      if (typeof p.cut === 'number') {
        const cutEl = document.getElementById('valCut');
        if (cutEl) cutEl.innerText = p.cut.toFixed(2);
        const cutInp = document.getElementById('battInputCut');
        if (cutInp) cutInp.value = p.cut.toFixed(2);
        pushSetpointAndCut(p.val, p.cut);
      } else {
        pushSetpoint(p.val);
      }
    }

    function applyBattPreset(cutVal, currVal) {
      playBeep(1100, 0.06);
      document.getElementById('battInputCut').value = cutVal.toFixed(2);
      document.getElementById('battInputCurr').value = currVal.toFixed(3);
      const cutEl = document.getElementById('valCut');
      if (cutEl) cutEl.innerText = cutVal.toFixed(2);
      selectMode(3);
      pushSetpointAndCut(currVal, cutVal);
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
      const setFan = document.getElementById('setFanLive');
      if (setFan) setFan.innerText = d.fan;

      // Statistiky baterie
      const mah = Math.round(d.discharged_mAh);
      document.getElementById('valMah').innerText = mah;
      document.getElementById('valWh').innerText = d.discharged_Wh.toFixed(2);
      const bMah = document.getElementById('battBigMah');
      if (bMah) bMah.innerText = mah;
      const bAh = document.getElementById('battBigAh');
      if (bAh) bAh.innerText = (d.discharged_mAh / 1000.0).toFixed(3);
      const bWh = document.getElementById('battBigWh');
      if (bWh) bWh.innerText = d.discharged_Wh.toFixed(2);

      // Výpočet orientačního vnitřního odporu (Ri)
      if (!d.output && d.voltage > 0.5) {
        lastVoltageUnloaded = d.voltage;
      } else if (d.output && d.current > 0.2 && lastVoltageUnloaded > d.voltage) {
        const deltaU = lastVoltageUnloaded - d.voltage;
        const riMilliOhm = Math.round((deltaU / d.current) * 1000);
        const riEl = document.getElementById('battRi');
        if (riEl && riMilliOhm > 0 && riMilliOhm < 9999) {
          riEl.innerText = riMilliOhm;
        }
      }

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

      const wasOutput = isOutput;
      isOutput = d.output;
      isAlarm = (d.alarm !== 0);

      if (isAlarm && !wasOutput) {
        playBeep(440, 0.4, 'sawtooth');
      }

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

      if (isRecording && recordStartTime) {
        const now = new Date();
        const elapsed = ((now - recordStartTime) / 1000).toFixed(1);
        const modes = ['CC', 'CP', 'CR', 'BATT'];
        logData.push({
          tSec: elapsed,
          clock: now.toTimeString().split(' ')[0],
          mode: modes[mode] || 'CC',
          u: d.voltage.toFixed(2),
          i: d.current.toFixed(3),
          p: d.power.toFixed(1),
          temp: d.temperature.toFixed(1),
          fan: d.fan,
          mah: Math.round(d.discharged_mAh),
          wh: d.discharged_Wh.toFixed(2)
        });
        updateRecStatus();
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

    applyTheme(currentTheme);
    applyLanguage();
    renderPresets();
    poll();
    setInterval(poll, 300);

    // Stopky testu baterie
    setInterval(() => {
      const bTimer = document.getElementById('battTimer');
      if (!bTimer) return;
      if (mode === 3 && isOutput) {
        if (!battTestStartTime) battTestStartTime = new Date();
        const sec = Math.floor((new Date() - battTestStartTime) / 1000);
        const h = String(Math.floor(sec / 3600)).padStart(2, '0');
        const m = String(Math.floor((sec % 3600) / 60)).padStart(2, '0');
        const s = String(sec % 60).padStart(2, '0');
        bTimer.innerText = `${h}:${m}:${s}`;
      } else if (!isOutput) {
        battTestStartTime = null;
      }
    }, 1000);

    async function selectMode(m) {
      playBeep(950, 0.05);
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
      playBeep(1100, 0.05);
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
      playBeep(isOutput ? 600 : 1200, 0.1);
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

    async function toggleBatteryTest() {
      if (mode !== 3) {
        await selectMode(3);
      }
      const cut = parseFloat(document.getElementById('battInputCut').value);
      const curr = parseFloat(document.getElementById('battInputCurr').value);
      if (!isNaN(cut) && !isNaN(curr)) {
        await pushSetpointAndCut(curr, cut);
      }
      await togglePower();
    }

    async function ackAlarm() {
      playBeep(1000, 0.1);
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

    /* --- ZÁZNAM A EXPORT DAT (CSV, JSON, PNG) --- */
    function toggleRecording() {
      playBeep(isRecording ? 700 : 1300, 0.08);
      const t = I18N[currentLang] || I18N.cs;
      const btn = document.getElementById('btnRec');
      const lbl = document.getElementById('lblRecBtn');
      isRecording = !isRecording;
      if (isRecording) {
        if (!recordStartTime || logData.length === 0) {
          recordStartTime = new Date();
        }
        btn.classList.add('recording');
        if (lbl) lbl.innerText = t.rec_stop;
      } else {
        btn.classList.remove('recording');
        if (lbl) lbl.innerText = t.rec_start;
      }
      updateRecStatus();
    }

    function updateRecStatus() {
      const t = I18N[currentLang] || I18N.cs;
      const el = document.getElementById('recStatus');
      if (!el) return;
      if (isRecording && recordStartTime) {
        const sec = Math.floor((new Date() - recordStartTime) / 1000);
        const m = String(Math.floor(sec / 60)).padStart(2, '0');
        const s = String(sec % 60).padStart(2, '0');
        el.innerText = `${t.rec_active}${m}:${s} [ ${logData.length}${t.rec_pts} ]`;
      } else if (logData.length > 0) {
        el.innerText = `${t.rec_saved}${logData.length}${t.rec_pts}`;
      } else {
        el.innerText = t.rec_idle;
      }
    }

    function clearLogData() {
      playBeep(600, 0.1);
      const t = I18N[currentLang] || I18N.cs;
      if (logData.length === 0) return;
      if (confirm(t.confirm_clear)) {
        logData.length = 0;
        recordStartTime = null;
        if (isRecording) toggleRecording();
        updateRecStatus();
      }
    }

    function getTimestampStr() {
      const d = new Date();
      const pad = n => String(n).padStart(2, '0');
      return `${d.getFullYear()}${pad(d.getMonth() + 1)}${pad(d.getDate())}_${pad(d.getHours())}${pad(d.getMinutes())}${pad(d.getSeconds())}`;
    }

    function downloadBlob(content, fileName, mimeType) {
      playBeep(1400, 0.1);
      const blob = new Blob([content], { type: mimeType });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = fileName;
      document.body.appendChild(a);
      a.click();
      setTimeout(() => {
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
      }, 200);
    }

    function getExportRows() {
      if (logData.length > 0) return logData;
      if (history.length > 0) {
        const modes = ['CC', 'CP', 'CR', 'BATT'];
        const vT = document.getElementById('valT') ? document.getElementById('valT').innerText : '25.0';
        const vFan = document.getElementById('valFan') ? document.getElementById('valFan').innerText : '0';
        const vMah = document.getElementById('valMah') ? document.getElementById('valMah').innerText : '0';
        const vWh = document.getElementById('valWh') ? document.getElementById('valWh').innerText : '0.00';
        return history.map((pt, idx) => ({
          tSec: (idx * 0.3).toFixed(1),
          clock: '--:--:--',
          mode: modes[mode] || 'CC',
          u: pt.u.toFixed(2),
          i: pt.i.toFixed(3),
          p: (pt.u * pt.i).toFixed(1),
          temp: vT,
          fan: vFan,
          mah: vMah,
          wh: vWh
        }));
      }
      return [];
    }

    function exportCSV() {
      const t = I18N[currentLang] || I18N.cs;
      const rows = getExportRows();
      if (rows.length === 0) {
        alert(t.no_data);
        return;
      }
      const sep = ';';
      let csv = '\uFEFF' + t.csv_header + '\r\n';
      rows.forEach(r => {
        csv += `${r.tSec}${sep}${r.clock}${sep}${r.mode}${sep}${r.u}${sep}${r.i}${sep}${r.p}${sep}${r.temp}${sep}${r.fan}${sep}${r.mah}${sep}${r.wh}\r\n`;
      });
      downloadBlob(csv, `zatez_mereni_${getTimestampStr()}.csv`, 'text/csv;charset=utf-8;');
    }

    function exportJSON() {
      const t = I18N[currentLang] || I18N.cs;
      const rows = getExportRows();
      if (rows.length === 0) {
        alert(t.no_data);
        return;
      }
      const obj = {
        device: 'DC Electronic Load 30V/5A',
        exported_at: new Date().toISOString(),
        active_mode: ['CC', 'CP', 'CR', 'BATT'][mode],
        samples_count: rows.length,
        samples: rows
      };
      downloadBlob(JSON.stringify(obj, null, 2), `zatez_mereni_${getTimestampStr()}.json`, 'application/json');
    }

    function exportPNG() {
      try {
        playBeep(1400, 0.1);
        const url = cv.toDataURL('image/png');
        const a = document.createElement('a');
        a.href = url;
        a.download = `zatez_graf_${getTimestampStr()}.png`;
        document.body.appendChild(a);
        a.click();
        setTimeout(() => document.body.removeChild(a), 200);
      } catch (err) {
        console.error(err);
      }
    }
  </script>
</body>
</html>
)rawliteral";

#endif // WEB_DASHBOARD_HTML_H
