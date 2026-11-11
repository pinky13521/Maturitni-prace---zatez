/**
 * @file web_dashboard_html.h
 * @brief Čisté technické webové rozhraní elektronické zátěže (Industrial Minimalist HUD)
 *        Obsahuje vysouvací postranní menu (☰) pro přepínání sekcí a nastavení.
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
  <title>DC ELEKTRONICKÁ ZÁTĚŽ // 30V 5A</title>
  <style>
    :root {
      --bg: #050505;
      --panel: #0c0c0c;
      --panel-alt: #151515;
      --border: #2c2c2c;
      --border-focus: #6e6e6e;
      --text: #ffffff;
      --text-muted: #9e9e9e;
      --text-dim: #cccccc;
      --invert-bg: #ffffff;
      --invert-text: #000000;
      --alarm: #ff2a2a;
      --ok: #22ff66;
      --trace-u: #ffffff;
      --trace-i: #8c8c8c;
      --mono: ui-monospace, "SF Mono", Monaco, "Cascadia Code", "Roboto Mono", Consolas, monospace;
    }

    body.theme-amber {
      --bg: #080500;
      --panel: #110b01;
      --panel-alt: #1c1203;
      --border: #442b00;
      --border-focus: #996300;
      --text: #ffb400;
      --text-muted: #b37e00;
      --text-dim: #e0a000;
      --invert-bg: #ffb400;
      --invert-text: #000000;
      --alarm: #ff3b00;
      --ok: #ffb400;
      --trace-u: #ffb400;
      --trace-i: #8c6200;
    }

    body.theme-green {
      --bg: #000802;
      --panel: #021205;
      --panel-alt: #041d09;
      --border: #004414;
      --border-focus: #00882a;
      --text: #2bff55;
      --text-muted: #1ca638;
      --text-dim: #55ff77;
      --invert-bg: #2bff55;
      --invert-text: #000000;
      --alarm: #ff2222;
      --ok: #2bff55;
      --trace-u: #2bff55;
      --trace-i: #147a28;
    }

    body.theme-light {
      --bg: #e4e7ec;
      --panel: #ffffff;
      --panel-alt: #f2f4f7;
      --border: #c0c6d0;
      --border-focus: #556070;
      --text: #111111;
      --text-muted: #555e6d;
      --text-dim: #333b48;
      --invert-bg: #111111;
      --invert-text: #ffffff;
      --alarm: #d91b1b;
      --ok: #15803d;
      --trace-u: #111111;
      --trace-i: #737b8c;
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
      -webkit-font-smoothing: antialiased;
    }

    body {
      display: flex;
      justify-content: center;
      padding: 14px;
    }

    /* HLAVNÍ RÁMEC PŘÍSTROJE (1px mřížka jako v původním návrhu) */
    .frame {
      width: 100%;
      max-width: 980px;
      display: flex;
      flex-direction: column;
      gap: 1px;
      background: var(--border);
      border: 1px solid var(--border);
    }

    /* ================================================================= */
    /* VYSOUVACÍ POSTRANNÍ PANEL (☰ SIDEBAR)                             */
    /* ================================================================= */
    .sidebar-backdrop {
      position: fixed;
      inset: 0;
      background: rgba(0, 0, 0, 0.7);
      z-index: 998;
      opacity: 0;
      pointer-events: none;
      transition: opacity 0.2s ease;
    }
    .sidebar-backdrop.open {
      opacity: 1;
      pointer-events: auto;
    }

    .sidebar-drawer {
      position: fixed;
      top: 0;
      left: 0;
      bottom: 0;
      width: 300px;
      max-width: 85vw;
      background: var(--panel);
      border-right: 1px solid var(--border-focus);
      z-index: 999;
      display: flex;
      flex-direction: column;
      transform: translateX(-102%);
      transition: transform 0.22s ease-out;
    }
    .sidebar-drawer.open {
      transform: translateX(0);
    }

    .drawer-header {
      padding: 14px 16px;
      border-bottom: 1px solid var(--border);
      display: flex;
      justify-content: space-between;
      align-items: center;
      background: var(--panel-alt);
    }

    .drawer-title {
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.1em;
      text-transform: uppercase;
    }

    .drawer-close {
      background: transparent;
      color: var(--text-muted);
      border: 1px solid var(--border);
      padding: 4px 8px;
      font-size: 11px;
      font-weight: 700;
      cursor: pointer;
    }
    .drawer-close:hover {
      color: var(--text);
      border-color: var(--text);
    }

    /* Mini stavový řádek v postranním panelu */
    .drawer-status {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 1px;
      background: var(--border);
      border-bottom: 1px solid var(--border);
    }
    .ds-cell {
      background: var(--panel);
      padding: 8px 6px;
      text-align: center;
    }
    .ds-lbl {
      font-size: 9px;
      color: var(--text-muted);
      display: block;
    }
    .ds-val {
      font-size: 12px;
      font-weight: 700;
      margin-top: 2px;
      display: block;
    }

    .drawer-body {
      flex: 1;
      overflow-y: auto;
      padding: 12px 0;
    }

    .drawer-sec-label {
      font-size: 10px;
      font-weight: 700;
      color: var(--text-muted);
      letter-spacing: 0.12em;
      text-transform: uppercase;
      padding: 10px 16px 6px 16px;
    }

    .drawer-item {
      width: 100%;
      background: transparent;
      color: var(--text-dim);
      border: none;
      border-left: 3px solid transparent;
      padding: 10px 16px;
      font-size: 12px;
      font-weight: 600;
      letter-spacing: 0.04em;
      text-align: left;
      cursor: pointer;
      display: flex;
      justify-content: space-between;
      align-items: center;
      transition: background 0.1s, color 0.1s;
    }
    .drawer-item:hover {
      background: var(--panel-alt);
      color: var(--text);
    }
    .drawer-item.active {
      background: var(--panel-alt);
      color: var(--text);
      border-left-color: var(--text);
      font-weight: 700;
    }

    .drawer-num {
      color: var(--text-muted);
      font-size: 11px;
      margin-right: 10px;
    }

    .drawer-tag {
      font-size: 10px;
      color: var(--text-muted);
      border: 1px solid var(--border);
      padding: 1px 5px;
    }

    .drawer-footer {
      padding: 12px 16px;
      border-top: 1px solid var(--border);
      background: var(--panel-alt);
      display: flex;
      flex-direction: column;
      gap: 8px;
    }

    .drawer-footer-row {
      display: flex;
      gap: 6px;
    }
    .drawer-fbtn {
      flex: 1;
      background: var(--panel);
      color: var(--text-dim);
      border: 1px solid var(--border);
      padding: 6px 8px;
      font-size: 10px;
      font-weight: 700;
      cursor: pointer;
      text-align: center;
    }
    .drawer-fbtn:hover {
      border-color: var(--border-focus);
      color: var(--text);
    }

    /* ================================================================= */
    /* HORNÍ LIŠTA (HEADER)                                              */
    /* ================================================================= */
    .header-bar {
      background: var(--panel);
      padding: 12px 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 12px;
    }

    .header-left-wrap {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    /* Tlačítko 3 čárky (☰) v čistém hranatém stylu */
    .menu-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border-focus);
      width: 34px;
      height: 32px;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      outline: none;
    }
    .menu-btn:hover {
      background: var(--invert-bg);
      color: var(--invert-text);
    }

    .header-title {
      font-size: 13px;
      letter-spacing: 0.12em;
      text-transform: uppercase;
      font-weight: 700;
    }

    .header-sub {
      font-size: 11px;
      color: var(--text-muted);
      letter-spacing: 0.06em;
      margin-top: 2px;
    }

    .header-right {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
    }

    .link-indicator {
      font-size: 11px;
      letter-spacing: 0.08em;
      font-weight: 700;
      padding: 5px 9px;
      border: 1px solid var(--border);
    }
    .link-ok { color: var(--ok); border-color: #1f5c30; background: #031c0a; }
    .link-err { color: var(--alarm); border-color: #882222; background: #220505; }

    .fs-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 5px 9px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.06em;
      cursor: pointer;
      outline: none;
    }
    .fs-btn:hover {
      background: var(--invert-bg);
      color: var(--invert-text);
    }

    .annunciator {
      font-size: 11px;
      font-weight: 800;
      letter-spacing: 0.1em;
      padding: 5px 10px;
      border: 1px solid var(--border-focus);
    }
    .ann-on { background: var(--invert-bg); color: var(--invert-text); border-color: var(--invert-bg); }
    .ann-off { background: var(--panel); color: var(--text-muted); border-color: var(--border); }
    .ann-alarm { background: var(--alarm); color: #ffffff; border-color: var(--alarm); animation: pulse 0.8s infinite alternate; }

    /* ================================================================= */
    /* OBSAHOVÉ SEKCE                                                    */
    /* ================================================================= */
    .tab-content {
      display: none;
      flex-direction: column;
      gap: 1px;
      background: var(--border);
    }
    .tab-content.active {
      display: flex;
    }

    /* Sekční lišta pro podstránky (když není otevřen hlavní dashboard) */
    .subpage-bar {
      background: var(--panel-alt);
      padding: 10px 16px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.08em;
    }
    .back-btn {
      background: var(--panel);
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 4px 10px;
      font-size: 11px;
      font-weight: 700;
      cursor: pointer;
    }
    .back-btn:hover {
      background: var(--invert-bg);
      color: var(--invert-text);
    }

    /* --- 4 DIGITÁLNÍ MĚŘIDLA (READOUTS) --- */
    .readouts {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 1px;
      background: var(--border);
    }

    .readout-cell {
      background: var(--panel);
      padding: 16px 18px;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      min-height: 105px;
    }

    .cell-tag {
      font-size: 11px;
      color: var(--text-muted);
      font-weight: 700;
      letter-spacing: 0.1em;
      text-transform: uppercase;
      display: flex;
      justify-content: space-between;
    }

    .cell-num {
      font-size: clamp(28px, 4.2vw, 40px);
      font-weight: 800;
      letter-spacing: -0.02em;
      line-height: 1.1;
      margin: 10px 0 6px 0;
      color: var(--text);
      font-variant-numeric: tabular-nums;
    }

    .cell-unit {
      font-size: 11px;
      color: var(--text-muted);
      font-weight: 700;
      letter-spacing: 0.06em;
    }

    /* --- OVLÁDACÍ PANEL (CONTROL DECK) --- */
    .control-deck {
      background: var(--panel);
      padding: 18px;
      display: flex;
      flex-direction: column;
      gap: 16px;
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
      border: 1px solid var(--border-focus);
      gap: 1px;
      background: var(--border-focus);
    }

    .mode-btn {
      background: var(--panel);
      color: var(--text-dim);
      border: none;
      padding: 9px 18px;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.1em;
      cursor: pointer;
      outline: none;
    }
    .mode-btn:hover {
      color: var(--text);
      background: var(--panel-alt);
    }
    .mode-btn.active {
      background: var(--invert-bg);
      color: var(--invert-text);
    }

    .power-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 9px 24px;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.12em;
      text-transform: uppercase;
      cursor: pointer;
      outline: none;
    }
    .power-btn:hover {
      border-color: var(--text);
    }
    .power-btn.engaged {
      background: var(--alarm);
      color: #ffffff;
      border-color: var(--alarm);
    }

    /* POSUVNÍK A ČÍSELNÉ ZADÁNÍ */
    .slider-wrap {
      display: flex;
      flex-direction: column;
      gap: 10px;
    }

    .slider-legend {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.05em;
      flex-wrap: wrap;
      gap: 8px;
    }

    .scale-selector {
      display: inline-flex;
      align-items: center;
      border: 1px solid var(--border);
      background: var(--panel-alt);
    }

    .scale-label {
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.08em;
      padding: 0 8px;
    }

    .scale-btn {
      background: transparent;
      color: var(--text-muted);
      border: none;
      border-left: 1px solid var(--border);
      font-size: 10px;
      font-weight: 700;
      padding: 4px 8px;
      cursor: pointer;
    }
    .scale-btn.active {
      background: var(--invert-bg);
      color: var(--invert-text);
    }

    .sp-input-box {
      display: inline-flex;
      align-items: center;
      gap: 8px;
    }

    .num-input {
      background: var(--bg);
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 6px 10px;
      font-size: 14px;
      font-weight: 700;
      width: 92px;
      text-align: right;
      outline: none;
    }
    .num-input:focus {
      border-color: var(--text);
    }

    .sp-set-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 6px 12px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.08em;
      cursor: pointer;
    }
    .sp-set-btn:hover {
      background: var(--invert-bg);
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
      width: 16px;
      height: 24px;
      background: var(--invert-bg);
      cursor: pointer;
      border: 1px solid var(--bg);
    }

    /* PŘEDVOLBY */
    .presets-row {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
    }

    .preset-label {
      font-size: 11px;
      color: var(--text-muted);
      letter-spacing: 0.08em;
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
    }
    .preset-btn:hover {
      border-color: var(--border-focus);
      color: var(--text);
    }

    /* --- OSCILOSKOP (GRAPH PANEL) --- */
    .graph-panel {
      background: var(--panel);
      padding: 16px 18px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }

    .graph-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 10px;
      flex-wrap: wrap;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.08em;
      color: var(--text-muted);
    }

    canvas {
      display: block;
      width: 100%;
      height: 220px;
      background: var(--bg);
      border: 1px solid var(--border);
    }

    /* EXPORTNÍ LIŠTA */
    .export-bar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding-top: 10px;
      border-top: 1px solid var(--border);
      gap: 10px;
      flex-wrap: wrap;
    }

    .export-left, .export-right {
      display: flex;
      align-items: center;
      gap: 6px;
      flex-wrap: wrap;
    }

    .rec-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border-focus);
      padding: 5px 12px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.08em;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }
    .rec-btn.recording {
      background: var(--alarm);
      border-color: var(--alarm);
      color: #ffffff;
    }

    .rec-dot {
      width: 8px;
      height: 8px;
      background: var(--alarm);
      display: inline-block;
    }
    .rec-btn.recording .rec-dot {
      background: #ffffff;
      animation: pulse 0.6s infinite alternate;
    }

    .rec-status {
      font-size: 11px;
      color: var(--text-muted);
      font-weight: 700;
      margin-left: 4px;
    }

    .exp-btn {
      background: var(--panel-alt);
      color: var(--text-dim);
      border: 1px solid var(--border);
      padding: 5px 10px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.05em;
      cursor: pointer;
    }
    .exp-btn:hover {
      border-color: var(--border-focus);
      color: var(--text);
    }

    /* --- KARTY NASTAVENÍ A BATERIÍ --- */
    .card-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
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
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.1em;
      text-transform: uppercase;
      color: var(--text);
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding-bottom: 10px;
      border-bottom: 1px solid var(--border);
    }

    .card-badge {
      font-size: 10px;
      font-weight: 700;
      letter-spacing: 0.08em;
      padding: 2px 6px;
      border: 1px solid var(--border);
      color: var(--text-muted);
    }

    .prop-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 12px;
      padding: 3px 0;
    }

    .prop-label {
      color: var(--text-muted);
    }

    .prop-val {
      font-weight: 700;
      color: var(--text);
    }

    /* VÝBĚR MOTIVU */
    .theme-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 8px;
    }

    .theme-card {
      background: var(--panel-alt);
      border: 1px solid var(--border);
      padding: 10px 12px;
      display: flex;
      align-items: center;
      gap: 10px;
      cursor: pointer;
    }
    .theme-card:hover {
      border-color: var(--border-focus);
    }
    .theme-card.active {
      border-color: var(--text);
      background: var(--bg);
    }

    .theme-swatch {
      width: 18px;
      height: 18px;
      border: 1px solid var(--border-focus);
      flex-shrink: 0;
    }

    /* --- ALARM BANNER --- */
    .alarm-banner {
      display: none;
      background: var(--alarm);
      padding: 10px 16px;
      color: #ffffff;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.08em;
      justify-content: space-between;
      align-items: center;
    }
    .btn-reset {
      background: #000000;
      color: #ffffff;
      border: 1px solid #ffffff;
      padding: 5px 12px;
      font-size: 11px;
      font-weight: 700;
      cursor: pointer;
    }

    @keyframes pulse {
      0% { opacity: 0.5; }
      100% { opacity: 1; }
    }

    @media (max-width: 720px) {
      .readouts, .card-grid, .theme-grid {
        grid-template-columns: repeat(2, 1fr);
      }
      .card-grid {
        grid-template-columns: 1fr;
      }
      .header-bar {
        flex-direction: column;
        align-items: flex-start;
      }
      .header-right {
        width: 100%;
        justify-content: space-between;
      }
    }
  </style>
</head>
<body class="theme-dark">

  <!-- =================================================================== -->
  <!-- VYSOUVACÍ POSTRANNÍ PANEL (☰)                                       -->
  <!-- =================================================================== -->
  <div id="sidebarBackdrop" class="sidebar-backdrop" onclick="closeSidebar()"></div>

  <aside id="sidebarDrawer" class="sidebar-drawer">
    <div class="drawer-header">
      <span class="drawer-title" data-i18n="drawer_title">NABÍDKA SYSTÉMU</span>
      <button class="drawer-close" onclick="closeSidebar()">[ ✕ ]</button>
    </div>

    <div class="drawer-status">
      <div class="ds-cell">
        <span class="ds-lbl">U [V]</span>
        <span class="ds-val" id="sideV">0.00</span>
      </div>
      <div class="ds-cell">
        <span class="ds-lbl">I [A]</span>
        <span class="ds-val" id="sideI">0.000</span>
      </div>
      <div class="ds-cell">
        <span class="ds-lbl">P [W]</span>
        <span class="ds-val" id="sideP">0.0</span>
      </div>
      <div class="ds-cell">
        <span class="ds-lbl">T [°C]</span>
        <span class="ds-val" id="sideT">25.0</span>
      </div>
    </div>

    <div class="drawer-body">
      <div class="drawer-sec-label" data-i18n="nav_sec_ops">PROVOZ A MĚŘENÍ</div>
      <button class="drawer-item active" id="navMain" onclick="navigateFromDrawer('main')">
        <span><span class="drawer-num">01</span><span data-i18n="nav_dashboard">HLAVNÍ PANEL MĚŘENÍ</span></span>
        <span class="drawer-tag">HUD</span>
      </button>
      <button class="drawer-item" id="navBatt" onclick="navigateFromDrawer('batt')">
        <span><span class="drawer-num">02</span><span data-i18n="nav_battery">TEST KAPACITY BATERIÍ</span></span>
        <span class="drawer-tag">BATT</span>
      </button>
      <button class="drawer-item" id="navExport" onclick="navigateFromDrawer('export')">
        <span><span class="drawer-num">03</span><span data-i18n="nav_export">ZÁZNAM A EXPORT DAT</span></span>
        <span class="drawer-tag">CSV</span>
      </button>

      <div class="drawer-sec-label" data-i18n="nav_sec_cfg">NASTAVENÍ A SYSTÉM</div>
      <button class="drawer-item" id="navSafety" onclick="navigateFromDrawer('safety')">
        <span><span class="drawer-num">04</span><span data-i18n="nav_safety">BEZPEČNOSTNÍ OCHRANY</span></span>
        <span class="drawer-tag">LIM</span>
      </button>
      <button class="drawer-item" id="navCooling" onclick="navigateFromDrawer('cooling')">
        <span><span class="drawer-num">05</span><span data-i18n="nav_cooling">CHLAZENÍ A VENTILÁTOR</span></span>
        <span class="drawer-tag">PWM</span>
      </button>
      <button class="drawer-item" id="navHw" onclick="navigateFromDrawer('hw')">
        <span><span class="drawer-num">06</span><span data-i18n="nav_hw">KALIBRACE A HARDWARE</span></span>
        <span class="drawer-tag">HW</span>
      </button>
      <button class="drawer-item" id="navMcu" onclick="navigateFromDrawer('mcu')">
        <span><span class="drawer-num">07</span><span data-i18n="nav_mcu">DIAGNOSTIKA ESP32</span></span>
        <span class="drawer-tag">MCU</span>
      </button>
      <button class="drawer-item" id="navSettingsAll" onclick="navigateFromDrawer('settings')">
        <span><span class="drawer-num">08</span><span data-i18n="nav_all_settings">PŘEHLED VŠECH NASTAVENÍ</span></span>
        <span class="drawer-tag">VŠE</span>
      </button>

      <div class="drawer-sec-label" data-i18n="nav_sec_ui">ROZHRANÍ</div>
      <button class="drawer-item" id="navTheme" onclick="navigateFromDrawer('theme')">
        <span><span class="drawer-num">09</span><span data-i18n="nav_themes">NASTAVENÍ VZHLEDU A UI</span></span>
        <span class="drawer-tag">UI</span>
      </button>
    </div>

    <div class="drawer-footer">
      <div class="drawer-footer-row">
        <button class="drawer-fbtn" onclick="toggleAudioFeedback()" id="drawerAudioBtn">[ ZVUK: ZAP ]</button>
        <button class="drawer-fbtn" onclick="toggleLanguage()">[ CZ / EN ]</button>
      </div>
    </div>
  </aside>

  <div class="frame">
    <!-- Havarijní výstraha -->
    <div id="alarmBar" class="alarm-banner">
      <span id="alarmReason" data-i18n="alarm_default">SYSTÉMOVÝ ALARM: VYBAVENÍ OCHRANY</span>
      <button class="btn-reset" onclick="ackAlarm()" data-i18n="btn_ack">[ POTVRDIT / RESET ]</button>
    </div>

    <!-- Hlavní horní lišta s tlačítkem 3 čárky [ ☰ ] -->
    <div class="header-bar">
      <div class="header-left-wrap">
        <button class="menu-btn" id="menuBtn" onclick="toggleSidebar()" title="Menu">
          <svg width="16" height="16" viewBox="0 0 16 16" fill="currentColor">
            <path d="M1 2.75h14v1.5H1v-1.5zm0 4.5h14v1.5H1v-1.5zm0 4.5h14v1.5H1v-1.5z"/>
          </svg>
        </button>
        <div>
          <div class="header-title" data-i18n="header_title">DC ELEKTRONICKÁ ZÁTĚŽ // 30V 5A</div>
          <div class="header-sub" data-i18n="header_sub">DIGITÁLNÍ REGULACE &bull; 50 HZ SMYČKA</div>
        </div>
      </div>
      <div class="header-right">
        <span id="linkStatus" class="link-indicator link-ok">● ŽIVĚ</span>
        <button id="langBtn" class="fs-btn" onclick="toggleLanguage()">[ EN ]</button>
        <button id="fsBtn" class="fs-btn" onclick="toggleFullscreen()">[ ⛶ FULL ]</button>
        <span id="stateAnn" class="annunciator ann-off">[ VYP ]</span>
      </div>
    </div>

    <!-- ================================================================= -->
    <!-- SEKCE 1: HLAVNÍ MĚŘENÍ A REGULACE (DASHBOARD)                     -->
    <!-- ================================================================= -->
    <div id="tabMain" class="tab-content active">
      <!-- 4 digitální měřáky -->
      <div class="readouts">
        <div class="readout-cell">
          <div class="cell-tag"><span data-i18n="lbl_v">NAPĚTÍ</span> <span data-i18n="lbl_v_max">MAX 32V</span></div>
          <div class="cell-num"><span id="valV">0.00</span></div>
          <div class="cell-unit" data-i18n="unit_v">VOLTY DC</div>
        </div>

        <div class="readout-cell">
          <div class="cell-tag"><span data-i18n="lbl_i">PROUD</span> <span data-i18n="lbl_i_max">MAX 5.2A</span></div>
          <div class="cell-num"><span id="valI">0.000</span></div>
          <div class="cell-unit" data-i18n="unit_i">AMPÉRY</div>
        </div>

        <div class="readout-cell">
          <div class="cell-tag"><span data-i18n="lbl_p">VÝKON</span> <span data-i18n="lbl_p_max">MAX 65W</span></div>
          <div class="cell-num"><span id="valP">0.0</span></div>
          <div class="cell-unit" data-i18n="unit_p">WATTY</div>
        </div>

        <div class="readout-cell">
          <div class="cell-tag"><span data-i18n="lbl_t">TEPLOTA</span> <span data-i18n="lbl_t_max">OTP 75°C</span></div>
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

        <!-- Nastavení žádané hodnoty -->
        <div class="slider-wrap">
          <div class="slider-legend">
            <div style="display:flex; align-items:center; gap:10px;">
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
              <span id="unitSp" style="color:var(--text-muted); font-size:12px; font-weight:700;">A</span>
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
        <div id="battRow" style="display:none; font-size:12px; color:var(--text-dim); border-top:1px solid var(--border); padding-top:10px;">
          <span data-i18n="lbl_discharged">VYBITO:</span> <strong id="valMah" style="color:var(--text);">0</strong> mAh &bull; <strong id="valWh" style="color:var(--text);">0.00</strong> Wh &bull; <span data-i18n="lbl_cutoff">MEZNÍ NAPĚTÍ:</span> <strong id="valCut" style="color:var(--text);">3.00</strong> V
        </div>
      </div>

      <!-- Osciloskopický záznam -->
      <div class="graph-panel" id="exportPanelCard">
        <div class="graph-header">
          <div>
            <span data-i18n="lbl_graph">OSCILOSKOPICKÝ ZÁZNAM [ 50 BODŮ ]</span>
            <span data-i18n="lbl_graph_sub">&bull; U [BÍLÁ] &bull; I [ŠEDÁ]</span>
          </div>
          <div class="scale-selector">
            <span class="scale-label" data-i18n="lbl_yscale">MĚŘÍTKO Y:</span>
            <button class="scale-btn active" id="btnScaleLin" onclick="setPlotScale('lin')">LIN</button>
            <button class="scale-btn" id="btnScaleLog" onclick="setPlotScale('log')">LOG</button>
          </div>
        </div>
        <canvas id="cv"></canvas>

        <!-- Panel záznamu měření a exportu dat -->
        <div class="export-bar">
          <div class="export-left">
            <button id="btnRec" class="rec-btn" onclick="toggleRecording()">
              <span class="rec-dot"></span> <span id="lblRecBtn" data-i18n="rec_start">ZÁZNAM</span>
            </button>
            <span id="recStatus" class="rec-status" data-i18n="rec_idle">ZÁZNAM: NEAKTIVNÍ</span>
          </div>
          <div class="export-right">
            <button class="exp-btn" onclick="exportCSV()" data-i18n="btn_csv">[ CSV / EXCEL ]</button>
            <button class="exp-btn" onclick="exportJSON()" data-i18n="btn_json">[ JSON ]</button>
            <button class="exp-btn" onclick="exportPNG()" data-i18n="btn_png">[ PNG GRAF ]</button>
            <button class="exp-btn" onclick="clearLogData()" data-i18n="btn_clear">[ SMAZAT ]</button>
          </div>
        </div>
      </div>
    </div>

    <!-- ================================================================= -->
    <!-- SEKCE 2: TESTOVÁNÍ BATERIÍ                                        -->
    <!-- ================================================================= -->
    <div id="tabBatt" class="tab-content">
      <div class="subpage-bar">
        <span data-i18n="nav_battery">02 // TEST KAPACITY BATERIÍ</span>
        <button class="back-btn" onclick="navigateFromDrawer('main')" data-i18n="btn_back">[ ← ZPĚT NA MĚŘENÍ ]</button>
      </div>

      <div class="readouts">
        <div class="readout-cell">
          <div class="cell-tag" data-i18n="batt_cap">KAPACITA</div>
          <div class="cell-num"><span id="battBigMah">0</span></div>
          <div class="cell-unit">mAh // <span id="battBigAh">0.000</span> Ah</div>
        </div>
        <div class="readout-cell">
          <div class="cell-tag" data-i18n="batt_energy">ENERGIE</div>
          <div class="cell-num"><span id="battBigWh">0.00</span></div>
          <div class="cell-unit" data-i18n="batt_integral">Wh (U &bull; I &bull; dt)</div>
        </div>
        <div class="readout-cell">
          <div class="cell-tag" data-i18n="batt_time">DOBA TESTU</div>
          <div class="cell-num"><span id="battTimer">00:00:00</span></div>
          <div class="cell-unit" data-i18n="batt_elapsed">ČAS OD STARTU</div>
        </div>
        <div class="readout-cell">
          <div class="cell-tag" data-i18n="batt_ri">VNITŘNÍ ODPOR (Ri)</div>
          <div class="cell-num"><span id="battRi">--</span></div>
          <div class="cell-unit" data-i18n="batt_ri_calc">mΩ (&Delta;U / &Delta;I)</div>
        </div>
      </div>

      <div class="control-deck">
        <div class="card-title">
          <span data-i18n="batt_cfg_title">PARAMETRY VYBÍJECÍHO TESTU</span>
          <span class="card-badge" id="battStatusBadge" data-i18n="batt_ready">PŘIPRAVENO</span>
        </div>

        <div class="presets-row">
          <span class="preset-label" data-i18n="batt_chem">PŘEDVOLBY:</span>
          <button class="preset-btn" onclick="applyBattPreset(3.0, 1.0)">Li-Ion / LiPo (3.0V)</button>
          <button class="preset-btn" onclick="applyBattPreset(2.5, 1.0)">LiFePO4 (2.5V)</button>
          <button class="preset-btn" onclick="applyBattPreset(1.0, 0.5)">NiMH / NiCd (1.0V)</button>
          <button class="preset-btn" onclick="applyBattPreset(10.5, 1.0)">Pb 12V (10.5V)</button>
        </div>

        <div class="deck-row">
          <div style="display:flex; align-items:center; gap:16px; flex-wrap:wrap;">
            <div style="display:flex; align-items:center; gap:8px;">
              <span class="prop-label" data-i18n="batt_discharge_curr">VYBÍJECÍ PROUD:</span>
              <input type="number" id="battInputCurr" class="num-input" value="1.000" step="0.1" min="0.05" max="5.2">
              <span class="prop-val">A</span>
            </div>
            <div style="display:flex; align-items:center; gap:8px;">
              <span class="prop-label" data-i18n="batt_cutoff_volt">MEZNÍ VYPNUTÍ (CUTOFF):</span>
              <input type="number" id="battInputCut" class="num-input" value="3.00" step="0.1" min="0.5" max="30">
              <span class="prop-val">V</span>
            </div>
          </div>
          <button id="battTestBtn" class="power-btn" onclick="toggleBatteryTest()" data-i18n="batt_start_btn">[ SPUSTIT TEST ]</button>
        </div>
      </div>
    </div>

    <!-- ================================================================= -->
    <!-- SEKCE 3: NASTAVENÍ A SYSTÉM                                       -->
    <!-- ================================================================= -->
    <div id="tabSettings" class="tab-content">
      <div class="subpage-bar">
        <span id="settingsHeaderTitle">NASTAVENÍ A KONFIGURACE SYSTÉMU</span>
        <div style="display:flex; gap:6px;">
          <button class="back-btn" id="btnShowAllSettings" onclick="filterSettings('all')" style="display:none;">[ ZOBRAZIT VŠE ]</button>
          <button class="back-btn" onclick="navigateFromDrawer('main')" data-i18n="btn_back">[ ← ZPĚT NA MĚŘENÍ ]</button>
        </div>
      </div>

      <div class="card-grid" id="settingsGrid">
        <!-- Karta 1: Bezpečnostní dohled -->
        <div class="info-card" id="cardSafety">
          <div class="card-title">
            <span data-i18n="set_safety_title">BEZPEČNOSTNÍ DOHLED (SUPERVISOR)</span>
            <span class="card-badge">AKTIVNÍ</span>
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
        <div class="info-card" id="cardCooling">
          <div class="card-title">
            <span data-i18n="set_fan_title">CHLAZENÍ A VENTILÁTOR</span>
            <span class="card-badge" id="setFanModeBadge">PWM AUTO</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_fan_freq">Frekvence PWM:</span>
            <span class="prop-val">25 kHz (4-pin PC)</span>
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
            <span class="prop-label" data-i18n="set_fan_duty">Aktuální výkon ventilátoru:</span>
            <span class="prop-val"><span id="setFanLive">20</span> %</span>
          </div>
        </div>

        <!-- Karta 3: Hardwarové konstanty -->
        <div class="info-card" id="cardHw">
          <div class="card-title">
            <span data-i18n="set_hw_title">HARDWAROVÉ KONSTANTY</span>
            <span class="card-badge">KALIBROVÁNO</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_shunt">Proudový bočník R_shunt:</span>
            <span class="prop-val">0.100 Ω (10W Kelvin)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_divider">Napěťový dělič vstupu:</span>
            <span class="prop-val">1:11.00 (100k / 10k)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_adc">A/D Převodník:</span>
            <span class="prop-val">ADS1115 (16-bit 0x48)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_dac">D/A Převodník:</span>
            <span class="prop-val">MCP4725 (12-bit 0x60)</span>
          </div>
        </div>

        <!-- Karta 4: Systém a mikrokontrolér -->
        <div class="info-card" id="cardMcu">
          <div class="card-title">
            <span data-i18n="set_mcu_title">SYSTÉM &amp; MIKROKONTROLÉR</span>
            <span class="card-badge">ESP32</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">MCU:</span>
            <span class="prop-val">ESP32 DevKit V1 (240 MHz)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">Volná paměť RAM (Heap):</span>
            <span class="prop-val" id="diagHeap">-- KB</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">Doba běhu (Uptime):</span>
            <span class="prop-val" id="diagUptime">--</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_loop">Perioda regulační smyčky:</span>
            <span class="prop-val">20 ms (50 Hz)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">Firmware:</span>
            <span class="prop-val">v2.5</span>
          </div>
        </div>
      </div>
    </div>

    <!-- ================================================================= -->
    <!-- SEKCE 4: VZHLED A NASTAVENÍ ROZHRANÍ                              -->
    <!-- ================================================================= -->
    <div id="tabTheme" class="tab-content">
      <div class="subpage-bar">
        <span data-i18n="nav_themes">09 // NASTAVENÍ VZHLEDU A ROZHRANÍ</span>
        <button class="back-btn" onclick="navigateFromDrawer('main')" data-i18n="btn_back">[ ← ZPĚT NA MĚŘENÍ ]</button>
      </div>

      <div class="card-grid">
        <!-- Karta motivů -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="theme_select_title">BAREVNÝ MOTIV ROZHRANÍ</span>
            <span class="card-badge" id="themeActiveBadge">MONOCHROME DARK</span>
          </div>

          <div class="theme-grid">
            <div class="theme-card active" id="thCardDark" onclick="applyTheme('dark')">
              <span class="theme-swatch" style="background:#000000;border-color:#ffffff;"></span>
              <div>
                <strong style="font-size:11px;">Monochrome Dark</strong>
                <div style="font-size:10px;color:var(--text-muted);">Černobílý přístrojový</div>
              </div>
            </div>

            <div class="theme-card" id="thCardAmber" onclick="applyTheme('amber')">
              <span class="theme-swatch" style="background:#ffb400;"></span>
              <div>
                <strong style="font-size:11px;">Keithley Amber</strong>
                <div style="font-size:10px;color:var(--text-muted);">Jantarový displej</div>
              </div>
            </div>

            <div class="theme-card" id="thCardGreen" onclick="applyTheme('green')">
              <span class="theme-swatch" style="background:#2bff55;"></span>
              <div>
                <strong style="font-size:11px;">CRT Phosphor</strong>
                <div style="font-size:10px;color:var(--text-muted);">Zelený osciloskop</div>
              </div>
            </div>

            <div class="theme-card" id="thCardLight" onclick="applyTheme('light')">
              <span class="theme-swatch" style="background:#ffffff;border-color:#333333;"></span>
              <div>
                <strong style="font-size:11px;">Clean Paper</strong>
                <div style="font-size:10px;color:var(--text-muted);">Světlý režim pro tisk</div>
              </div>
            </div>
          </div>
        </div>

        <!-- Karta chování a zvuku -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="theme_opts_title">PARAMETRY ROZHRANÍ</span>
            <span class="card-badge">KONFIGURACE</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="theme_sound">Zvuková odezva (Pípnutí):</span>
            <button class="sp-set-btn" id="btnAudioToggle" onclick="toggleAudioFeedback()">ZAPNUTO</button>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="theme_trace_pts">Délka historie grafu:</span>
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
    let lastVoltageUnloaded = 0.0;
    const logData = [];
    const history = [];
    let LIMIT = 50;

    const audioCtx = (window.AudioContext || window.webkitAudioContext) ? new (window.AudioContext || window.webkitAudioContext)() : null;
    function playBeep(freq = 880, dur = 0.08, type = 'sine') {
      if (!soundEnabled || !audioCtx) return;
      try {
        if (audioCtx.state === 'suspended') audioCtx.resume();
        const osc = audioCtx.createOscillator();
        const g = audioCtx.createGain();
        osc.type = type;
        osc.frequency.value = freq;
        g.gain.setValueAtTime(0.03, audioCtx.currentTime);
        g.gain.exponentialRampToValueAtTime(0.0001, audioCtx.currentTime + dur);
        osc.connect(g);
        g.connect(audioCtx.destination);
        osc.start();
        osc.stop(audioCtx.currentTime + dur);
      } catch(e) {}
    }

    /* ================================================================= */
    /* OVLÁDÁNÍ POSTRANNÍHO PANELU (☰ SIDEBAR)                           */
    /* ================================================================= */
    function toggleSidebar() {
      const dr = document.getElementById('sidebarDrawer');
      const bd = document.getElementById('sidebarBackdrop');
      if (dr.classList.contains('open')) {
        closeSidebar();
      } else {
        playBeep(780, 0.04);
        dr.classList.add('open');
        bd.classList.add('open');
      }
    }

    function closeSidebar() {
      document.getElementById('sidebarDrawer').classList.remove('open');
      document.getElementById('sidebarBackdrop').classList.remove('open');
    }

    document.addEventListener('keydown', (e) => {
      if (e.key === 'Escape') closeSidebar();
    });

    function setActiveDrawerItem(id) {
      document.querySelectorAll('.drawer-item').forEach(el => el.classList.remove('active'));
      const target = document.getElementById(id);
      if (target) target.classList.add('active');
    }

    function filterSettings(sec) {
      const grid = document.getElementById('settingsGrid');
      const btnAll = document.getElementById('btnShowAllSettings');
      const cards = {
        safety: document.getElementById('cardSafety'),
        cooling: document.getElementById('cardCooling'),
        hw: document.getElementById('cardHw'),
        mcu: document.getElementById('cardMcu')
      };

      if (sec === 'all') {
        grid.style.gridTemplateColumns = '';
        if (btnAll) btnAll.style.display = 'none';
        Object.values(cards).forEach(c => { if (c) c.style.display = 'flex'; });
      } else {
        grid.style.gridTemplateColumns = '1fr';
        if (btnAll) btnAll.style.display = 'inline-block';
        Object.entries(cards).forEach(([k, c]) => {
          if (c) c.style.display = (k === sec) ? 'flex' : 'none';
        });
      }
    }

    function switchTab(tabId) {
      document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
      if (tabId === 'main') {
        document.getElementById('tabMain').classList.add('active');
        setTimeout(resize, 50);
      } else if (tabId === 'batt') {
        document.getElementById('tabBatt').classList.add('active');
      } else if (tabId === 'settings') {
        document.getElementById('tabSettings').classList.add('active');
      } else if (tabId === 'theme') {
        document.getElementById('tabTheme').classList.add('active');
      }
    }

    function navigateFromDrawer(dest) {
      playBeep(720, 0.04);
      closeSidebar();

      if (dest === 'main') {
        switchTab('main');
        setActiveDrawerItem('navMain');
      } else if (dest === 'batt') {
        switchTab('batt');
        setActiveDrawerItem('navBatt');
      } else if (dest === 'export') {
        switchTab('main');
        setActiveDrawerItem('navExport');
        const expCard = document.getElementById('exportPanelCard');
        if (expCard) expCard.scrollIntoView({ behavior: 'smooth', block: 'center' });
      } else if (dest === 'safety') {
        switchTab('settings');
        filterSettings('safety');
        setActiveDrawerItem('navSafety');
      } else if (dest === 'cooling') {
        switchTab('settings');
        filterSettings('cooling');
        setActiveDrawerItem('navCooling');
      } else if (dest === 'hw') {
        switchTab('settings');
        filterSettings('hw');
        setActiveDrawerItem('navHw');
      } else if (dest === 'mcu') {
        switchTab('settings');
        filterSettings('mcu');
        setActiveDrawerItem('navMcu');
      } else if (dest === 'settings') {
        switchTab('settings');
        filterSettings('all');
        setActiveDrawerItem('navSettingsAll');
      } else if (dest === 'theme') {
        switchTab('theme');
        setActiveDrawerItem('navTheme');
      }
    }

    const I18N = {
      cs: {
        lang_btn: '[ EN ]',
        drawer_title: 'NABÍDKA SYSTÉMU',
        nav_sec_ops: 'PROVOZ A MĚŘENÍ',
        nav_dashboard: 'HLAVNÍ PANEL MĚŘENÍ',
        nav_battery: 'TEST KAPACITY BATERIÍ',
        nav_export: 'ZÁZNAM A EXPORT DAT',
        nav_sec_cfg: 'NASTAVENÍ A SYSTÉM',
        nav_safety: 'BEZPEČNOSTNÍ OCHRANY',
        nav_cooling: 'CHLAZENÍ A VENTILÁTOR',
        nav_hw: 'KALIBRACE A HARDWARE',
        nav_mcu: 'DIAGNOSTIKA ESP32',
        nav_all_settings: 'PŘEHLED VŠECH NASTAVENÍ',
        nav_sec_ui: 'ROZHRANÍ',
        nav_themes: 'NASTAVENÍ VZHLEDU A UI',
        btn_back: '[ ← ZPĚT NA MĚŘENÍ ]',
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
        alarm_default: 'SYSTÉMOVÝ ALARM: VYBAVENÍ OCHRANY',
        alarm_trip: 'OCHRANA VYBAVENA: ',
        btn_ack: '[ POTVRDIT / RESET ]',
        rec_start: 'ZÁZNAM',
        rec_stop: 'STOP',
        rec_idle: 'ZÁZNAM: NEAKTIVNÍ',
        rec_recording: 'ZÁZNAM BĚŽÍ: ',
        rec_stopped: 'ULOŽENO VZORKŮ: ',
        rec_pts: ' vzorků',
        btn_csv: '[ CSV / EXCEL ]',
        btn_json: '[ JSON ]',
        btn_png: '[ PNG GRAF ]',
        btn_clear: '[ SMAZAT ]',
        batt_cap: 'KAPACITA',
        batt_energy: 'ENERGIE',
        batt_integral: 'Wh (U • I • dt)',
        batt_time: 'DOBA TESTU',
        batt_elapsed: 'ČAS OD STARTU',
        batt_ri: 'VNITŘNÍ ODPOR (Ri)',
        batt_ri_calc: 'mΩ (ΔU / ΔI)',
        batt_cfg_title: 'PARAMETRY VYBÍJECÍHO TESTU',
        batt_ready: 'PŘIPRAVENO',
        batt_running: 'PROBÍHÁ VYBÍJENÍ',
        batt_chem: 'PŘEDVOLBY:',
        batt_discharge_curr: 'VYBÍJECÍ PROUD:',
        batt_cutoff_volt: 'MEZNÍ VYPNUTÍ (CUTOFF):',
        batt_start_btn: '[ SPUSTIT TEST ]',
        batt_stop_btn: '[ ZASTAVIT TEST ]',
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
        set_fan_duty: 'Aktuální výkon ventilátoru:',
        set_hw_title: 'HARDWAROVÉ KONSTANTY',
        set_shunt: 'Proudový bočník R_shunt:',
        set_divider: 'Napěťový dělič vstupu:',
        set_adc: 'A/D Převodník:',
        set_dac: 'D/A Převodník:',
        set_mcu_title: 'SYSTÉM & MIKROKONTROLÉR',
        set_loop: 'Perioda regulační smyčky:',
        theme_select_title: 'BAREVNÝ MOTIV ROZHRANÍ',
        theme_opts_title: 'PARAMETRY ROZHRANÍ',
        theme_sound: 'Zvuková odezva (Pípnutí):',
        theme_trace_pts: 'Délka historie grafu:',
        theme_lang: 'Jazyk rozhraní:',
        sound_on: 'ZAPNUTO',
        sound_off: 'VYPNUTO'
      },
      en: {
        lang_btn: '[ CZ ]',
        drawer_title: 'SYSTEM MENU',
        nav_sec_ops: 'OPERATION & MEASUREMENT',
        nav_dashboard: 'MAIN MEASUREMENT PANEL',
        nav_battery: 'BATTERY CAPACITY TEST',
        nav_export: 'DATA LOGGER & EXPORT',
        nav_sec_cfg: 'CONFIGURATION & SYSTEM',
        nav_safety: 'SAFETY PROTECTIONS',
        nav_cooling: 'COOLING & PWM FAN',
        nav_hw: 'HARDWARE & CALIBRATION',
        nav_mcu: 'ESP32 DIAGNOSTICS',
        nav_all_settings: 'ALL SETTINGS OVERVIEW',
        nav_sec_ui: 'INTERFACE',
        nav_themes: 'APPEARANCE & UI SETTINGS',
        btn_back: '[ ← BACK TO DASHBOARD ]',
        header_title: 'DC ELECTRONIC LOAD // 30V 5A',
        header_sub: 'DIGITAL CONTROL • 50 HZ LOOP',
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
        lbl_t: 'TEMPERATURE',
        lbl_t_max: 'OTP 75°C',
        lbl_fan: 'FAN',
        pwr_off: 'OUTPUT: OFF',
        pwr_active: 'OUTPUT: ACTIVE',
        pwr_tripped: 'OUTPUT: TRIPPED',
        ann_off: '[ OFF ]',
        ann_on: '[ ON ]',
        ann_alarm: '[ ALARM ]',
        lbl_sp_cc: 'TARGET CURRENT:',
        lbl_sp_cp: 'TARGET POWER:',
        lbl_sp_cr: 'TARGET RESISTANCE:',
        lbl_curve: 'CURVE:',
        btn_set: 'SET',
        lbl_presets: 'PRESETS:',
        lbl_discharged: 'DISCHARGED:',
        lbl_cutoff: 'CUT-OFF:',
        lbl_graph: 'OSCILLOSCOPE TRACE [ 50 PTS ]',
        lbl_graph_sub: '• U [WHITE] • I [GREY]',
        lbl_yscale: 'Y-SCALE:',
        alarm_default: 'SYSTEM ALARM: PROTECTION TRIPPED',
        alarm_trip: 'PROTECTION TRIPPED: ',
        btn_ack: '[ ACKNOWLEDGE / RESET ]',
        rec_start: 'RECORD',
        rec_stop: 'STOP',
        rec_idle: 'LOGGER: IDLE',
        rec_recording: 'RECORDING: ',
        rec_stopped: 'SAVED SAMPLES: ',
        rec_pts: ' samples',
        btn_csv: '[ CSV / EXCEL ]',
        btn_json: '[ JSON ]',
        btn_png: '[ PNG CHART ]',
        btn_clear: '[ CLEAR ]',
        batt_cap: 'CAPACITY',
        batt_energy: 'ENERGY',
        batt_integral: 'Wh (U • I • dt)',
        batt_time: 'TEST TIME',
        batt_elapsed: 'ELAPSED TIME',
        batt_ri: 'INTERNAL RES. (Ri)',
        batt_ri_calc: 'mΩ (ΔU / ΔI)',
        batt_cfg_title: 'DISCHARGE TEST PARAMETERS',
        batt_ready: 'READY',
        batt_running: 'DISCHARGING',
        batt_chem: 'PRESETS:',
        batt_discharge_curr: 'DISCHARGE CURRENT:',
        batt_cutoff_volt: 'CUT-OFF VOLTAGE:',
        batt_start_btn: '[ START TEST ]',
        batt_stop_btn: '[ STOP TEST ]',
        set_safety_title: 'SAFETY SUPERVISOR',
        set_ovp: 'Over-Voltage Protection (OVP):',
        set_ocp: 'Over-Current Protection (OCP):',
        set_opp: 'Over-Power Protection (OPP):',
        set_otp: 'Over-Temperature Protection (OTP):',
        set_otp_warn: 'Fan 100% Warning Threshold:',
        set_fan_title: 'COOLING & FAN CONTROL',
        set_fan_freq: 'PWM Frequency:',
        set_fan_min: 'Cooling Start (20%):',
        set_fan_max: 'Full Speed (100%):',
        set_fan_duty: 'Current Fan Duty:',
        set_hw_title: 'HARDWARE CONSTANTS',
        set_shunt: 'Current Sense Shunt R_shunt:',
        set_divider: 'Input Voltage Divider:',
        set_adc: 'A/D Converter:',
        set_dac: 'D/A Converter:',
        set_mcu_title: 'SYSTEM & MICROCONTROLLER',
        set_loop: 'Control Loop Period:',
        theme_select_title: 'INTERFACE COLOR THEME',
        theme_opts_title: 'INTERFACE PARAMETERS',
        theme_sound: 'Audio Feedback (Beeper):',
        theme_trace_pts: 'Oscilloscope Buffer Length:',
        theme_lang: 'Interface Language:',
        sound_on: 'ENABLED',
        sound_off: 'DISABLED'
      }
    };

    const PRESETS = {
      0: [
        { label: '0.1A', val: 0.1 },
        { label: '0.5A (USB)', val: 0.5 },
        { label: '1.0A', val: 1.0 },
        { label: '2.0A (USB)', val: 2.0 },
        { label: '3.0A', val: 3.0 }
      ],
      1: [
        { label: '5W', val: 5.0 },
        { label: '10W', val: 10.0 },
        { label: '25W', val: 25.0 },
        { label: '50W', val: 50.0 }
      ],
      2: [
        { label: '5Ω', val: 5.0 },
        { label: '10Ω', val: 10.0 },
        { label: '24Ω', val: 24.0 },
        { label: '100Ω', val: 100.0 }
      ],
      3: [
        { label: '0.2A', val: 0.2 },
        { label: '0.5A', val: 0.5 },
        { label: '1.0A', val: 1.0 },
        { label: '2.0A', val: 2.0 }
      ]
    };

    function applyTheme(th) {
      playBeep(900, 0.06);
      currentTheme = th;
      localStorage.setItem('load_theme', th);
      document.body.className = 'theme-' + th;

      document.querySelectorAll('.theme-card').forEach(c => c.classList.remove('active'));
      const mapId = { dark: 'thCardDark', amber: 'thCardAmber', green: 'thCardGreen', light: 'thCardLight' };
      const names = { dark: 'MONOCHROME DARK', amber: 'KEITHLEY AMBER', green: 'CRT PHOSPHOR', light: 'CLEAN PAPER' };
      const el = document.getElementById(mapId[th] || 'thCardDark');
      if (el) el.classList.add('active');
      const bdg = document.getElementById('themeActiveBadge');
      if (bdg) bdg.innerText = names[th] || 'MONOCHROME DARK';
      renderPlot();
    }

    function toggleAudioFeedback() {
      soundEnabled = !soundEnabled;
      localStorage.setItem('load_sound', soundEnabled ? 'true' : 'false');
      if (soundEnabled) playBeep(1040, 0.08);
      updateAudioBtnText();
    }

    function updateAudioBtnText() {
      const t = I18N[currentLang] || I18N.cs;
      const btn = document.getElementById('btnAudioToggle');
      if (btn) btn.innerText = soundEnabled ? t.sound_on : t.sound_off;
      const dBtn = document.getElementById('drawerAudioBtn');
      if (dBtn) dBtn.innerText = soundEnabled ? '[ ZVUK: ZAP ]' : '[ ZVUK: VYP ]';
    }

    function setPlotLimit(pts) {
      playBeep(800, 0.05);
      LIMIT = pts;
      document.getElementById('btnPts50').classList.toggle('active', pts === 50);
      document.getElementById('btnPts100').classList.toggle('active', pts === 100);
      document.getElementById('btnPts150').classList.toggle('active', pts === 150);
      while (history.length > LIMIT) history.shift();
      renderPlot();
    }

    function applyBattPreset(cutV, currA) {
      playBeep(960, 0.06);
      document.getElementById('battInputCut').value = cutV.toFixed(2);
      document.getElementById('battInputCurr').value = currA.toFixed(3);
      pushSetpointAndCut(currA, cutV);
    }

    function applyLanguage() {
      const t = I18N[currentLang] || I18N.cs;
      document.documentElement.lang = currentLang;
      document.getElementById('langBtn').innerText = t.lang_btn;
      const thLang = document.getElementById('themeLangLabel');
      if (thLang) thLang.innerText = (currentLang === 'cs') ? 'ČEŠTINA (CZ)' : 'ENGLISH (EN)';

      document.querySelectorAll('[data-i18n]').forEach(el => {
        const key = el.getAttribute('data-i18n');
        if (t[key]) el.innerText = t[key];
      });

      syncModeConfig(mode);
      setOutputVisuals(isOutput);
      updateFsBtnText();
      updateRecordUI();
      updateAudioBtnText();
      renderPlot();
    }

    function toggleLanguage() {
      playBeep(840, 0.05);
      currentLang = (currentLang === 'cs') ? 'en' : 'cs';
      localStorage.setItem('load_lang', currentLang);
      applyLanguage();
    }

    function updateFsBtnText() {
      const t = I18N[currentLang] || I18N.cs;
      const btn = document.getElementById('fsBtn');
      const isFs = document.fullscreenElement || document.webkitFullscreenElement;
      btn.innerText = isFs ? t.full_exit : t.full_enter;
    }

    function toggleFullscreen() {
      const doc = window.document;
      const docEl = doc.documentElement;
      const requestFs = docEl.requestFullscreen || docEl.webkitRequestFullScreen || docEl.mozRequestFullScreen || docEl.msRequestFullscreen;
      const exitFs = doc.exitFullscreen || doc.webkitExitFullscreen || doc.mozCancelFullScreen || doc.msExitFullscreen;

      if (!doc.fullscreenElement && !doc.webkitFullscreenElement) {
        if (requestFs) requestFs.call(docEl);
      } else {
        if (exitFs) exitFs.call(doc);
      }
    }
    document.addEventListener('fullscreenchange', updateFsBtnText);
    document.addEventListener('webkitfullscreenchange', updateFsBtnText);

    const cv = document.getElementById('cv');
    const ctx = cv.getContext('2d');
    let cssW = 800;
    let cssH = 220;
    let dpr = 1;

    function resize() {
      const r = cv.getBoundingClientRect();
      if (r.width === 0) return;
      dpr = window.devicePixelRatio || 1;
      cssW = r.width;
      cssH = r.height;
      cv.width = Math.round(cssW * dpr);
      cv.height = Math.round(cssH * dpr);
      renderPlot();
    }
    window.addEventListener('resize', resize);
    setTimeout(resize, 50);

    function getLimitsForMode() {
      if (mode === 1) return { minLin: 0.0, minLog: 0.1, max: 65.0, decimals: 1, step: 0.5 };
      if (mode === 2) return { minLin: 0.5, minLog: 0.5, max: 500.0, decimals: 1, step: 0.5 };
      return { minLin: 0.0, minLog: 0.005, max: 5.2, decimals: 3, step: 0.025 };
    }

    function sliderPosToValue(pos) {
      const lim = getLimitsForMode();
      const norm = Math.max(0, Math.min(1, pos / 1000.0));
      if (sliderCurve === 'lin') {
        return lim.minLin + norm * (lim.max - lim.minLin);
      } else {
        if (norm <= 0.001) return lim.minLin;
        const minL = Math.log10(lim.minLog);
        const maxL = Math.log10(lim.max);
        return Math.pow(10, minL + norm * (maxL - minL));
      }
    }

    function valueToSliderPos(val) {
      const lim = getLimitsForMode();
      if (val <= lim.minLin) return 0;
      if (sliderCurve === 'lin') {
        const r = (val - lim.minLin) / (lim.max - lim.minLin);
        return Math.round(Math.max(0, Math.min(1, r)) * 1000);
      } else {
        const clamped = Math.max(lim.minLog, Math.min(lim.max, val));
        const r = (Math.log10(clamped) - Math.log10(lim.minLog)) / (Math.log10(lim.max) - Math.log10(lim.minLog));
        return Math.round(Math.max(0, Math.min(1, r)) * 1000);
      }
    }

    /* --- VYKRESLOVÁNÍ GRAFU (OSTRÉ ČÍSLICE, ČISTÝ RASTROVÝ STYL) --- */
    function renderPlot() {
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
      ctx.clearRect(0, 0, cssW, cssH);
      const w = cssW;
      const h = cssH;

      const pL = 44;
      const pR = w - 46;
      const pT = 14;
      const pB = h - 16;
      const plotW = pR - pL;
      const plotH = pB - pT;

      const styles = getComputedStyle(document.body);
      const colBorder = styles.getPropertyValue('--border-focus').trim() || '#555555';
      const colGrid = styles.getPropertyValue('--border').trim() || '#262626';
      const colText = styles.getPropertyValue('--text').trim() || '#ffffff';
      const colMuted = styles.getPropertyValue('--text-muted').trim() || '#9e9e9e';
      const colTraceU = styles.getPropertyValue('--trace-u').trim() || '#ffffff';
      const colTraceI = styles.getPropertyValue('--trace-i').trim() || '#888888';

      ctx.strokeStyle = colBorder;
      ctx.lineWidth = 1;
      ctx.strokeRect(pL, pT, plotW, plotH);

      ctx.font = '700 12px ui-monospace, "SF Mono", "Roboto Mono", Consolas, monospace';
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
          ctx.fillText(vVal.toFixed(0) + 'V', pL - 7, y);

          const iVal = (i / divs) * 5.2;
          ctx.fillStyle = colMuted;
          ctx.textAlign = 'left';
          ctx.fillText(iVal.toFixed(1) + 'A', pR + 7, y);
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
            ctx.fillText(t.label, pL - 7, y);
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
          ctx.fillText(t.label, pR + 7, y);
        });
      }

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

    function setPlotScale(sc) {
      plotScale = sc;
      document.getElementById('btnScaleLin').classList.toggle('active', sc === 'lin');
      document.getElementById('btnScaleLog').classList.toggle('active', sc === 'log');
      renderPlot();
    }

    function setSliderCurve(curve) {
      sliderCurve = curve;
      document.getElementById('btnSliderLin').classList.toggle('active', curve === 'lin');
      document.getElementById('btnSliderLog').classList.toggle('active', curve === 'log');
      document.getElementById('spSlider').value = valueToSliderPos(currentSetpointVal);
    }

    function renderPresets() {
      const box = document.getElementById('presetsBox');
      box.innerHTML = '';
      (PRESETS[mode] || []).forEach(p => {
        const b = document.createElement('button');
        b.className = 'preset-btn';
        b.innerText = p.label;
        b.onclick = () => applyValue(p.val);
        box.appendChild(b);
      });
    }

    function syncModeConfig(m) {
      mode = m;
      const t = I18N[currentLang] || I18N.cs;
      for (let i = 0; i < 4; i++) {
        document.getElementById('m' + i).classList.toggle('active', i === m);
      }

      const lbl = document.getElementById('lblSp');
      const unit = document.getElementById('unitSp');
      const inp = document.getElementById('spInput');
      const battRow = document.getElementById('battRow');
      const lim = getLimitsForMode();

      inp.min = lim.minLin;
      inp.max = lim.max;
      inp.step = lim.step;

      if (m === 0 || m === 3) {
        lbl.innerText = t.lbl_sp_cc;
        unit.innerText = 'A';
      } else if (m === 1) {
        lbl.innerText = t.lbl_sp_cp;
        unit.innerText = 'W';
      } else if (m === 2) {
        lbl.innerText = t.lbl_sp_cr;
        unit.innerText = 'Ω';
      }

      battRow.style.display = (m === 3) ? 'block' : 'none';
      document.getElementById('spSlider').value = valueToSliderPos(currentSetpointVal);
      renderPresets();
    }

    function setOutputVisuals(on) {
      isOutput = on;
      const t = I18N[currentLang] || I18N.cs;
      const btn = document.getElementById('pwrBtn');
      const ann = document.getElementById('stateAnn');
      const bBtn = document.getElementById('battTestBtn');
      const bBdg = document.getElementById('battStatusBadge');

      if (on) {
        btn.className = 'power-btn engaged';
        btn.innerText = t.pwr_active;
        ann.className = 'annunciator ann-on';
        ann.innerText = t.ann_on;
        if (bBtn) { bBtn.className = 'power-btn engaged'; bBtn.innerText = t.batt_stop_btn; }
        if (bBdg) { bBdg.innerText = t.batt_running; }
      } else {
        btn.className = 'power-btn';
        btn.innerText = t.pwr_off;
        ann.className = 'annunciator ann-off';
        ann.innerText = t.ann_off;
        if (bBtn) { bBtn.className = 'power-btn'; bBtn.innerText = t.batt_start_btn; }
        if (bBdg) { bBdg.innerText = t.batt_ready; }
      }
    }

    function setLinkStatus(ok) {
      const t = I18N[currentLang] || I18N.cs;
      const el = document.getElementById('linkStatus');
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
      const vStr = d.voltage.toFixed(2);
      const iStr = d.current.toFixed(3);
      const pStr = d.power.toFixed(1);
      const tStr = d.temperature.toFixed(1);

      document.getElementById('valV').innerText = vStr;
      document.getElementById('valI').innerText = iStr;
      document.getElementById('valP').innerText = pStr;
      document.getElementById('valT').innerText = tStr;
      document.getElementById('valFan').innerText = d.fan;

      const sV = document.getElementById('sideV'); if (sV) sV.innerText = vStr;
      const sI = document.getElementById('sideI'); if (sI) sI.innerText = iStr;
      const sP = document.getElementById('sideP'); if (sP) sP.innerText = pStr;
      const sT = document.getElementById('sideT'); if (sT) sT.innerText = tStr;

      const setFan = document.getElementById('setFanLive');
      if (setFan) setFan.innerText = d.fan;

      const mah = Math.round(d.discharged_mAh);
      document.getElementById('valMah').innerText = mah;
      document.getElementById('valWh').innerText = d.discharged_Wh.toFixed(2);
      const bMah = document.getElementById('battBigMah');
      if (bMah) bMah.innerText = mah;
      const bAh = document.getElementById('battBigAh');
      if (bAh) bAh.innerText = (d.discharged_mAh / 1000.0).toFixed(3);
      const bWh = document.getElementById('battBigWh');
      if (bWh) bWh.innerText = d.discharged_Wh.toFixed(2);

      if (typeof d.ri === 'number' && d.ri > 0) {
        const riEl = document.getElementById('battRi');
        if (riEl) riEl.innerText = Math.round(d.ri * 1000);
      } else if (!d.output && d.voltage > 0.5) {
        lastVoltageUnloaded = d.voltage;
      } else if (d.output && d.current > 0.2 && lastVoltageUnloaded > d.voltage) {
        const deltaU = lastVoltageUnloaded - d.voltage;
        const riMilliOhm = Math.round((deltaU / d.current) * 1000);
        const riEl = document.getElementById('battRi');
        if (riEl && riMilliOhm > 0 && riMilliOhm < 9999) {
          riEl.innerText = riMilliOhm;
        }
      }

      if (typeof d.duration === 'number') {
        const sec = d.duration;
        const h = String(Math.floor(sec / 3600)).padStart(2, '0');
        const m = String(Math.floor((sec % 3600) / 60)).padStart(2, '0');
        const s = String(sec % 60).padStart(2, '0');
        const bTimer = document.getElementById('battTimer');
        if (bTimer) bTimer.innerText = `${h}:${m}:${s}`;
      }

      if (typeof d.heap === 'number') {
        const elHeap = document.getElementById('diagHeap');
        if (elHeap) elHeap.innerText = Math.round(d.heap / 1024) + ' KB';
      }
      if (typeof d.uptime === 'number') {
        const elUp = document.getElementById('diagUptime');
        if (elUp) {
          const s = d.uptime;
          const h = Math.floor(s / 3600);
          const m = Math.floor((s % 3600) / 60);
          const sc = s % 60;
          elUp.innerText = `${h}h ${m}m ${sc}s`;
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
        playBeep(440, 0.3, 'sawtooth');
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
          time_iso: now.toISOString(),
          elapsed_s: parseFloat(elapsed),
          mode: modes[d.mode] || 'CC',
          output: d.output ? 1 : 0,
          voltage_V: d.voltage,
          current_A: d.current,
          power_W: d.power,
          temp_C: d.temperature,
          fan_pct: d.fan,
          setpoint: d.setpoint,
          mah: d.discharged_mAh,
          wh: d.discharged_Wh
        });
        updateRecordUI();
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

    async function selectMode(m) {
      playBeep(950, 0.04);
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
        pushSetpoint(val);
        tOut = null;
      }, 60);
    }

    function submitNumericInput() {
      const inp = document.getElementById('spInput');
      let val = parseFloat(inp.value);
      const lim = getLimitsForMode();
      if (isNaN(val)) val = lim.minLin;
      val = Math.max(lim.minLin, Math.min(lim.max, val));
      applyValue(val);
    }

    function applyValue(val) {
      playBeep(880, 0.04);
      const lim = getLimitsForMode();
      currentSetpointVal = val;
      document.getElementById('spInput').value = val.toFixed(lim.decimals);
      document.getElementById('spSlider').value = valueToSliderPos(val);
      pushSetpoint(val);
    }

    async function togglePower() {
      if (isAlarm) return;
      const next = !isOutput;
      playBeep(next ? 1200 : 600, 0.06);
      setOutputVisuals(next);
      try {
        const r = await fetch(`/api/control?out=${next ? 1 : 0}`, { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {}
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
      playBeep(1000, 0.08);
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

    async function pushSetpointAndCut(val, cut) {
      try {
        const r = await fetch(`/api/control?mode=${mode}&setpoint=${val}&cut=${cut}`, { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {}
    }

    function updateRecordUI() {
      const t = I18N[currentLang] || I18N.cs;
      const btn = document.getElementById('btnRec');
      const lbl = document.getElementById('lblRecBtn');
      const st = document.getElementById('recStatus');

      if (isRecording) {
        btn.classList.add('recording');
        lbl.innerText = t.rec_stop;
        st.innerText = t.rec_recording + logData.length + t.rec_pts;
      } else {
        btn.classList.remove('recording');
        lbl.innerText = t.rec_start;
        if (logData.length > 0) {
          st.innerText = t.rec_stopped + logData.length;
        } else {
          st.innerText = t.rec_idle;
        }
      }
    }

    function toggleRecording() {
      playBeep(isRecording ? 660 : 1100, 0.06);
      if (!isRecording) {
        if (logData.length === 0 || !recordStartTime) {
          recordStartTime = new Date();
        }
        isRecording = true;
      } else {
        isRecording = false;
      }
      updateRecordUI();
    }

    function clearLogData() {
      isRecording = false;
      logData.length = 0;
      recordStartTime = null;
      updateRecordUI();
    }

    function getExportDataSnapshot() {
      if (logData.length > 0) return logData;
      const modes = ['CC', 'CP', 'CR', 'BATT'];
      return history.map((pt, idx) => ({
        time_iso: new Date().toISOString(),
        elapsed_s: parseFloat((idx * 0.3).toFixed(1)),
        mode: modes[mode] || 'CC',
        output: isOutput ? 1 : 0,
        voltage_V: pt.u,
        current_A: pt.i,
        power_W: parseFloat((pt.u * pt.i).toFixed(2)),
        temp_C: parseFloat(document.getElementById('valT').innerText) || 25.0,
        fan_pct: parseInt(document.getElementById('valFan').innerText) || 20,
        setpoint: currentSetpointVal,
        mah: parseFloat(document.getElementById('valMah').innerText) || 0,
        wh: parseFloat(document.getElementById('valWh').innerText) || 0
      }));
    }

    function getTimestampFilename(ext) {
      const d = new Date();
      const pad = n => String(n).padStart(2, '0');
      const ts = `${d.getFullYear()}-${pad(d.getMonth()+1)}-${pad(d.getDate())}_${pad(d.getHours())}-${pad(d.getMinutes())}-${pad(d.getSeconds())}`;
      return `elektronicka_zatez_${ts}.${ext}`;
    }

    function triggerDownload(blob, filename) {
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = filename;
      document.body.appendChild(a);
      a.click();
      setTimeout(() => {
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
      }, 150);
    }

    function exportCSV() {
      const rows = getExportDataSnapshot();
      const isCz = (currentLang === 'cs');
      const fmt = num => isCz ? String(num).replace('.', ',') : String(num);

      const header = isCz
        ? 'Cas_ISO;Cas_od_startu_s;Rezim;Vystup_ZAP;Napeti_V;Proud_A;Vykon_W;Teplota_C;Ventilator_pct;Zadana_hodnota;Vybito_mAh;Energie_Wh\r\n'
        : 'Time_ISO;Elapsed_s;Mode;Output_ON;Voltage_V;Current_A;Power_W;Temp_C;Fan_pct;Setpoint;Discharged_mAh;Energy_Wh\r\n';

      const lines = rows.map(r => [
        r.time_iso,
        fmt(r.elapsed_s.toFixed(1)),
        r.mode,
        r.output,
        fmt(r.voltage_V.toFixed(3)),
        fmt(r.current_A.toFixed(4)),
        fmt(r.power_W.toFixed(2)),
        fmt(r.temp_C.toFixed(1)),
        r.fan_pct,
        fmt(r.setpoint.toFixed(3)),
        fmt(r.mah.toFixed(1)),
        fmt(r.wh.toFixed(2))
      ].join(';'));

      const csvContent = '\uFEFF' + header + lines.join('\r\n');
      const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
      triggerDownload(blob, getTimestampFilename('csv'));
    }

    function exportJSON() {
      const rows = getExportDataSnapshot();
      const payload = {
        device: 'DC Electronic Load 30V/5A',
        exported_at: new Date().toISOString(),
        sample_count: rows.length,
        samples: rows
      };
      const blob = new Blob([JSON.stringify(payload, null, 2)], { type: 'application/json;charset=utf-8;' });
      triggerDownload(blob, getTimestampFilename('json'));
    }

    function exportPNG() {
      try {
        const exportCanvas = document.createElement('canvas');
        exportCanvas.width = cv.width;
        exportCanvas.height = cv.height;
        const eCtx = exportCanvas.getContext('2d');
        const bgCol = getComputedStyle(document.body).getPropertyValue('--bg').trim() || '#050505';
        eCtx.fillStyle = bgCol;
        eCtx.fillRect(0, 0, exportCanvas.width, exportCanvas.height);
        eCtx.drawImage(cv, 0, 0);

        exportCanvas.toBlob(blob => {
          if (blob) triggerDownload(blob, getTimestampFilename('png'));
        }, 'image/png');
      } catch (err) {
        console.error(err);
      }
    }
  </script>
</body>
</html>
)rawliteral";

#endif // WEB_DASHBOARD_HTML_H
