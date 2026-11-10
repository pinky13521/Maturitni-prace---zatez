/**
 * @file web_dashboard_html.h
 * @brief Komplexní laboratorní rozhraní elektronické zátěže (DC Electronic Load HUD)
 *        Obsahuje vysouvací postranní panel (GitHub-style Drawer Menu ☰),
 *        záložky Měření, Test baterií, Bezpečnostní ochrany, Chlazení, Kalibrace, Diagnostika a Vzhled.
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
  <meta name="theme-color" content="#0b0f17">
  <title>DC ELEKTRONICKÁ ZÁTĚŽ 30V / 5A</title>
  <style>
    :root {
      --bg: #0b0f17;
      --panel: #131a26;
      --panel-alt: #1a2333;
      --panel-hover: #212c40;
      --border: #263349;
      --border-focus: #3b82f6;
      --text: #f8fafc;
      --text-muted: #94a3b8;
      --text-dim: #cbd5e1;
      --invert-bg: #3b82f6;
      --invert-text: #ffffff;
      --alarm: #ef4444;
      --accent: #22c55e;
      --trace-u: #38bdf8;
      --trace-i: #4ade80;
      --val-u: #38bdf8;
      --val-i: #4ade80;
      --val-p: #fbbf24;
      --val-t: #f87171;
      --font-sans: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      --font-mono: ui-monospace, "SF Mono", Monaco, "Cascadia Code", "Roboto Mono", Consolas, monospace;
    }

    body.theme-amber {
      --bg: #0d0800;
      --panel: #171003;
      --panel-alt: #241905;
      --panel-hover: #302208;
      --border: #422d0c;
      --border-focus: #ffb703;
      --text: #ffb703;
      --text-muted: #b37e02;
      --text-dim: #ffcf54;
      --invert-bg: #ffb703;
      --invert-text: #000000;
      --alarm: #ff3b00;
      --accent: #ffb703;
      --trace-u: #ffe066;
      --trace-i: #ff9f1c;
      --val-u: #ffe066;
      --val-i: #ffb703;
      --val-p: #ff9f1c;
      --val-t: #ff6b35;
    }

    body.theme-green {
      --bg: #010e05;
      --panel: #031c0a;
      --panel-alt: #072a11;
      --panel-hover: #0c3818;
      --border: #0e4c20;
      --border-focus: #22c55e;
      --text: #4ade80;
      --text-muted: #15803d;
      --text-dim: #86efac;
      --invert-bg: #22c55e;
      --invert-text: #000000;
      --alarm: #ef4444;
      --accent: #4ade80;
      --trace-u: #86efac;
      --trace-i: #22c55e;
      --val-u: #86efac;
      --val-i: #4ade80;
      --val-p: #22c55e;
      --val-t: #a7f3d0;
    }

    body.theme-cyan {
      --bg: #030e1a;
      --panel: #06192e;
      --panel-alt: #0b2440;
      --panel-hover: #103054;
      --border: #133c66;
      --border-focus: #00e5ff;
      --text: #00e5ff;
      --text-muted: #0d8299;
      --text-dim: #67e8f9;
      --invert-bg: #00e5ff;
      --invert-text: #000000;
      --alarm: #f43f5e;
      --accent: #00e5ff;
      --trace-u: #ffffff;
      --trace-i: #00e5ff;
      --val-u: #ffffff;
      --val-i: #00e5ff;
      --val-p: #38bdf8;
      --val-t: #67e8f9;
    }

    body.theme-light {
      --bg: #f1f5f9;
      --panel: #ffffff;
      --panel-alt: #f8fafc;
      --panel-hover: #e2e8f0;
      --border: #cbd5e1;
      --border-focus: #2563eb;
      --text: #0f172a;
      --text-muted: #64748b;
      --text-dim: #334155;
      --invert-bg: #0f172a;
      --invert-text: #ffffff;
      --alarm: #dc2626;
      --accent: #0284c7;
      --trace-u: #0284c7;
      --trace-i: #16a34a;
      --val-u: #0284c7;
      --val-i: #16a34a;
      --val-p: #d97706;
      --val-t: #dc2626;
    }

    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
      font-family: var(--font-sans);
      -webkit-tap-highlight-color: transparent;
    }

    html, body {
      background: var(--bg);
      color: var(--text);
      min-height: 100vh;
      min-height: -webkit-fill-available;
      -webkit-font-smoothing: antialiased;
      -moz-osx-font-smoothing: grayscale;
      transition: background 0.2s, color 0.2s;
    }

    body {
      display: flex;
      justify-content: center;
      padding: 16px 12px;
    }

    .frame {
      width: 100%;
      max-width: 1020px;
      display: flex;
      flex-direction: column;
      gap: 12px;
    }

    /* ================================================================= */
    /* GITHUB-STYLE SLIDE-OUT SIDEBAR DRAWER (☰)                         */
    /* ================================================================= */
    .sidebar-backdrop {
      position: fixed;
      inset: 0;
      background: rgba(0, 0, 0, 0.65);
      backdrop-filter: blur(3px);
      -webkit-backdrop-filter: blur(3px);
      z-index: 998;
      opacity: 0;
      pointer-events: none;
      transition: opacity 0.25s ease;
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
      width: 320px;
      max-width: 86vw;
      background: var(--panel);
      border-right: 1px solid var(--border);
      box-shadow: 8px 0 32px rgba(0, 0, 0, 0.5);
      z-index: 999;
      display: flex;
      flex-direction: column;
      transform: translateX(-104%);
      transition: transform 0.28s cubic-bezier(0.16, 1, 0.3, 1);
    }
    .sidebar-drawer.open {
      transform: translateX(0);
    }

    .drawer-header {
      padding: 16px 18px;
      border-bottom: 1px solid var(--border);
      display: flex;
      justify-content: space-between;
      align-items: center;
      background: var(--panel-alt);
    }

    .drawer-brand {
      display: flex;
      align-items: center;
      gap: 10px;
    }

    .drawer-logo {
      width: 32px;
      height: 32px;
      border-radius: 8px;
      background: var(--invert-bg);
      color: var(--invert-text);
      display: flex;
      align-items: center;
      justify-content: center;
      font-weight: 900;
      font-size: 15px;
      box-shadow: 0 2px 8px rgba(0,0,0,0.25);
    }

    .drawer-brand-text {
      display: flex;
      flex-direction: column;
    }
    .drawer-brand-title {
      font-size: 13px;
      font-weight: 800;
      letter-spacing: 0.03em;
      color: var(--text);
    }
    .drawer-brand-sub {
      font-size: 11px;
      color: var(--text-muted);
      font-weight: 500;
    }

    .drawer-close-btn {
      background: transparent;
      color: var(--text-muted);
      border: 1px solid transparent;
      width: 32px;
      height: 32px;
      border-radius: 6px;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      font-size: 16px;
      transition: all 0.15s;
    }
    .drawer-close-btn:hover {
      background: var(--panel-hover);
      color: var(--text);
      border-color: var(--border);
    }

    /* Mini živý stavový pás v menu */
    .drawer-live-strip {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 4px;
      padding: 10px 14px;
      background: var(--bg);
      border-bottom: 1px solid var(--border);
      font-family: var(--font-mono);
      font-size: 11px;
    }
    .strip-item {
      display: flex;
      flex-direction: column;
      align-items: center;
      padding: 4px 2px;
      border-radius: 5px;
      background: var(--panel);
      border: 1px solid var(--border);
    }
    .strip-lbl {
      font-size: 9px;
      color: var(--text-muted);
      font-weight: 700;
    }
    .strip-val {
      font-weight: 700;
      font-size: 11px;
      margin-top: 1px;
    }

    .drawer-body {
      flex: 1;
      overflow-y: auto;
      padding: 12px 10px;
      display: flex;
      flex-direction: column;
      gap: 16px;
    }

    .drawer-section {
      display: flex;
      flex-direction: column;
      gap: 3px;
    }

    .drawer-section-title {
      font-size: 10px;
      font-weight: 800;
      text-transform: uppercase;
      letter-spacing: 0.08em;
      color: var(--text-muted);
      padding: 4px 10px 6px 10px;
    }

    .drawer-item {
      background: transparent;
      color: var(--text-dim);
      border: 1px solid transparent;
      border-radius: 7px;
      padding: 9px 12px;
      font-size: 13px;
      font-weight: 600;
      cursor: pointer;
      display: flex;
      align-items: center;
      justify-content: space-between;
      text-align: left;
      transition: all 0.12s ease;
    }
    .drawer-item-left {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .drawer-icon {
      font-size: 15px;
      width: 20px;
      text-align: center;
    }
    .drawer-item:hover {
      background: var(--panel-alt);
      color: var(--text);
    }
    .drawer-item.active {
      background: rgba(59, 130, 246, 0.14);
      color: var(--text);
      border-color: rgba(59, 130, 246, 0.35);
      font-weight: 700;
    }
    .drawer-item.active::before {
      content: '';
      width: 4px;
      height: 18px;
      background: var(--invert-bg);
      border-radius: 2px;
      margin-right: -4px;
    }

    .drawer-badge {
      font-family: var(--font-mono);
      font-size: 10px;
      font-weight: 700;
      padding: 2px 6px;
      border-radius: 4px;
      background: var(--panel-alt);
      border: 1px solid var(--border);
      color: var(--text-muted);
    }

    .drawer-footer {
      padding: 14px 16px;
      border-top: 1px solid var(--border);
      background: var(--panel-alt);
      display: flex;
      flex-direction: column;
      gap: 10px;
    }

    .drawer-theme-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
    }
    .drawer-theme-dots {
      display: flex;
      gap: 8px;
    }
    .theme-dot {
      width: 22px;
      height: 22px;
      border-radius: 50%;
      cursor: pointer;
      border: 2px solid var(--border);
      transition: transform 0.15s, border-color 0.15s;
    }
    .theme-dot:hover {
      transform: scale(1.15);
      border-color: var(--text);
    }

    .drawer-quick-actions {
      display: flex;
      gap: 6px;
    }
    .drawer-qbtn {
      flex: 1;
      background: var(--panel);
      color: var(--text-dim);
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 6px 8px;
      font-size: 11px;
      font-weight: 700;
      cursor: pointer;
      text-align: center;
      transition: all 0.15s;
    }
    .drawer-qbtn:hover {
      border-color: var(--border-focus);
      color: var(--text);
    }

    /* --- ZÁHLAVÍ (HEADER) --- */
    .header-bar {
      background: var(--panel);
      padding: 12px 16px;
      border-radius: 10px;
      border: 1px solid var(--border);
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 12px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.15);
    }

    .header-left-wrap {
      display: flex;
      align-items: center;
      gap: 12px;
    }

    /* Tlačítko 3 čárky (GitHub Hamburger Button) */
    .hamburger-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border);
      width: 38px;
      height: 38px;
      border-radius: 8px;
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      transition: all 0.15s ease;
      flex-shrink: 0;
    }
    .hamburger-btn:hover {
      background: var(--panel-hover);
      border-color: var(--border-focus);
      transform: translateY(-1px);
    }
    .hamburger-btn:active {
      transform: translateY(0);
    }

    .header-left {
      display: flex;
      flex-direction: column;
    }

    .header-title-row {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
    }

    .header-title {
      font-size: 14px;
      letter-spacing: 0.05em;
      text-transform: uppercase;
      font-weight: 800;
    }

    .breadcrumb-pill {
      font-size: 11px;
      font-weight: 700;
      padding: 2px 8px;
      border-radius: 5px;
      background: var(--panel-alt);
      border: 1px solid var(--border);
      color: var(--val-u);
    }

    .header-sub {
      font-size: 11px;
      color: var(--text-muted);
      letter-spacing: 0.02em;
      margin-top: 2px;
      font-weight: 500;
    }

    .header-right {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
    }

    .link-indicator {
      font-size: 11px;
      letter-spacing: 0.05em;
      font-weight: 700;
      padding: 6px 10px;
      border-radius: 6px;
      border: 1px solid var(--border);
    }
    .link-ok { color: #22c55e; border-color: rgba(34,197,94,0.35); background: rgba(34,197,94,0.12); }
    .link-err { color: #ef4444; border-color: rgba(239,68,68,0.35); background: rgba(239,68,68,0.12); }

    .fs-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border);
      padding: 6px 10px;
      border-radius: 6px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.04em;
      cursor: pointer;
      outline: none;
      transition: all 0.15s ease;
    }
    .fs-btn:hover {
      background: var(--border-focus);
      color: #ffffff;
      border-color: var(--border-focus);
    }

    .annunciator {
      font-size: 11px;
      font-weight: 800;
      letter-spacing: 0.08em;
      padding: 6px 12px;
      border-radius: 6px;
      border: 1px solid transparent;
    }

    .ann-on { background: #22c55e; color: #000000; font-weight: 800; box-shadow: 0 0 12px rgba(34,197,94,0.3); }
    .ann-off { background: var(--panel-alt); color: var(--text-muted); border-color: var(--border); }
    .ann-alarm { background: #ef4444; color: #ffffff; animation: pulse 0.8s infinite alternate; box-shadow: 0 0 14px rgba(239,68,68,0.4); }

    /* --- RYCHLÁ NAVIGAČNÍ LIŠTA (TABS) --- */
    .nav-bar {
      display: flex;
      background: var(--panel);
      border-radius: 10px;
      border: 1px solid var(--border);
      padding: 4px;
      gap: 4px;
      overflow-x: auto;
      scrollbar-width: none;
      box-shadow: 0 4px 12px rgba(0,0,0,0.12);
    }
    .nav-bar::-webkit-scrollbar { display: none; }

    .nav-btn {
      flex: 1;
      background: transparent;
      color: var(--text-muted);
      border: none;
      padding: 9px 14px;
      border-radius: 7px;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.03em;
      cursor: pointer;
      display: inline-flex;
      justify-content: center;
      align-items: center;
      gap: 8px;
      white-space: nowrap;
      transition: all 0.15s ease;
    }
    .nav-btn:hover {
      color: var(--text);
      background: var(--panel-alt);
    }
    .nav-btn.active {
      color: var(--invert-text);
      background: var(--invert-bg);
      box-shadow: 0 2px 8px rgba(0,0,0,0.2);
    }

    .tab-content {
      display: none;
      flex-direction: column;
      gap: 12px;
    }
    .tab-content.active {
      display: flex;
    }

    /* --- MĚŘENÉ HODNOTY (READOUTS) --- */
    .readouts {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 10px;
    }

    .readout-cell {
      background: var(--panel);
      padding: 16px 18px;
      border-radius: 10px;
      border: 1px solid var(--border);
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      min-height: 112px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.15);
      position: relative;
      overflow: hidden;
    }

    .readout-cell::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      height: 3px;
      background: var(--border);
    }
    .readout-v::before { background: var(--val-u); }
    .readout-i::before { background: var(--val-i); }
    .readout-p::before { background: var(--val-p); }
    .readout-t::before { background: var(--val-t); }

    .cell-tag {
      font-size: 11px;
      color: var(--text-muted);
      font-weight: 700;
      letter-spacing: 0.06em;
      text-transform: uppercase;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }

    .cell-num {
      font-family: var(--font-mono);
      font-size: clamp(28px, 4.2vw, 42px);
      font-weight: 700;
      letter-spacing: -0.02em;
      line-height: 1.15;
      margin: 8px 0 4px 0;
      color: var(--text);
      font-variant-numeric: tabular-nums;
    }

    .readout-v .cell-num { color: var(--val-u); text-shadow: 0 0 20px rgba(56,189,248,0.2); }
    .readout-i .cell-num { color: var(--val-i); text-shadow: 0 0 20px rgba(74,222,128,0.2); }
    .readout-p .cell-num { color: var(--val-p); text-shadow: 0 0 20px rgba(251,191,36,0.2); }
    .readout-t .cell-num { color: var(--val-t); text-shadow: 0 0 20px rgba(248,113,113,0.2); }

    .cell-unit {
      font-size: 12px;
      color: var(--text-muted);
      font-weight: 600;
      letter-spacing: 0.03em;
    }

    /* --- OVLÁDACÍ PANEL (CONTROL DECK) --- */
    .control-deck {
      background: var(--panel);
      padding: 18px 20px;
      border-radius: 10px;
      border: 1px solid var(--border);
      display: flex;
      flex-direction: column;
      gap: 16px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.15);
    }

    .deck-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 14px;
      flex-wrap: wrap;
    }

    .btn-group {
      display: inline-flex;
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 3px;
      gap: 3px;
      background: var(--panel-alt);
    }

    .mode-btn {
      background: transparent;
      color: var(--text-muted);
      border: none;
      padding: 8px 16px;
      border-radius: 6px;
      font-size: 12px;
      font-weight: 700;
      letter-spacing: 0.04em;
      cursor: pointer;
      outline: none;
      transition: all 0.15s ease;
    }

    .mode-btn:hover { color: var(--text); background: rgba(255,255,255,0.05); }
    .mode-btn.active {
      background: var(--invert-bg);
      color: var(--invert-text);
      box-shadow: 0 2px 6px rgba(0,0,0,0.2);
    }

    .power-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border);
      padding: 10px 24px;
      border-radius: 8px;
      font-size: 13px;
      font-weight: 800;
      letter-spacing: 0.06em;
      text-transform: uppercase;
      cursor: pointer;
      outline: none;
      transition: all 0.15s ease;
    }

    .power-btn:hover {
      border-color: var(--border-focus);
      transform: translateY(-1px);
    }
    .power-btn:active { transform: translateY(1px); }
    .power-btn.engaged {
      background: #dc2626;
      color: #ffffff;
      border-color: #ef4444;
      box-shadow: 0 0 16px rgba(220, 38, 38, 0.45);
    }

    /* POSUVNÍK + ČÍSELNÝ VSTUP */
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
      font-weight: 600;
      flex-wrap: wrap;
      gap: 8px;
    }

    .scale-selector {
      display: inline-flex;
      align-items: center;
      gap: 4px;
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 3px 6px;
      background: var(--panel-alt);
    }

    .scale-label {
      font-size: 10px;
      color: var(--text-muted);
      letter-spacing: 0.05em;
      margin-right: 2px;
    }

    .scale-btn {
      background: transparent;
      color: var(--text-muted);
      border: none;
      font-size: 10px;
      font-weight: 700;
      padding: 3px 7px;
      border-radius: 4px;
      cursor: pointer;
      transition: all 0.1s ease;
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
      font-family: var(--font-mono);
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border);
      border-radius: 7px;
      padding: 7px 10px;
      font-size: 14px;
      font-weight: 700;
      width: 95px;
      text-align: right;
      outline: none;
      transition: border 0.15s;
    }
    .num-input:focus {
      border-color: var(--border-focus);
      box-shadow: 0 0 0 2px rgba(59,130,246,0.2);
    }

    .sp-set-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border);
      border-radius: 7px;
      padding: 7px 12px;
      font-size: 11px;
      font-weight: 700;
      cursor: pointer;
      transition: all 0.15s ease;
    }
    .sp-set-btn:hover {
      background: var(--border-focus);
      color: #ffffff;
      border-color: var(--border-focus);
    }

    input[type=range] {
      -webkit-appearance: none;
      width: 100%;
      height: 8px;
      border-radius: 4px;
      background: var(--panel-alt);
      border: 1px solid var(--border);
      outline: none;
    }
    input[type=range]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 22px;
      height: 22px;
      border-radius: 50%;
      background: var(--invert-bg);
      cursor: pointer;
      border: 2px solid #ffffff;
      box-shadow: 0 2px 6px rgba(0,0,0,0.35);
      transition: transform 0.1s ease;
    }
    input[type=range]::-webkit-slider-thumb:hover {
      transform: scale(1.15);
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
      letter-spacing: 0.05em;
      text-transform: uppercase;
      font-weight: 700;
    }

    .preset-btn {
      background: var(--panel-alt);
      color: var(--text-dim);
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 7px 14px;
      font-size: 12px;
      font-weight: 700;
      cursor: pointer;
      transition: all 0.15s ease;
    }
    .preset-btn:hover {
      background: var(--border);
      color: var(--text);
      border-color: var(--border-focus);
      transform: translateY(-1px);
    }

    /* --- GRAFICKÝ PANEL (OSCILLOSCOPE) --- */
    .graph-panel {
      background: var(--panel);
      padding: 16px 18px;
      border-radius: 10px;
      border: 1px solid var(--border);
      display: flex;
      flex-direction: column;
      gap: 12px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.15);
      transition: box-shadow 0.3s, border-color 0.3s;
    }
    .graph-panel.highlight-pulse, .info-card.highlight-pulse {
      border-color: var(--border-focus);
      box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.35);
    }

    .graph-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 10px;
      flex-wrap: wrap;
      font-size: 12px;
      font-weight: 700;
    }

    canvas {
      display: block;
      width: 100%;
      height: 230px;
      background: rgba(0, 0, 0, 0.22);
      border-radius: 8px;
      border: 1px solid var(--border);
    }

    /* EXPORTNÍ LIŠTA */
    .export-bar {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding-top: 12px;
      border-top: 1px solid var(--border);
      gap: 10px;
      flex-wrap: wrap;
    }

    .export-left, .export-right {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
    }

    .rec-btn {
      background: var(--panel-alt);
      color: var(--text);
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 7px 14px;
      font-size: 11px;
      font-weight: 800;
      letter-spacing: 0.05em;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 7px;
      transition: all 0.15s;
    }
    .rec-btn:hover {
      border-color: #ef4444;
    }
    .rec-btn.recording {
      background: rgba(239, 68, 68, 0.18);
      border-color: #ef4444;
      color: #ef4444;
    }

    .rec-dot {
      width: 9px;
      height: 9px;
      border-radius: 50%;
      background: #ef4444;
      display: inline-block;
    }
    .rec-btn.recording .rec-dot {
      box-shadow: 0 0 8px #ef4444;
      animation: pulse 0.6s infinite alternate;
    }

    .rec-status {
      font-family: var(--font-mono);
      font-size: 11px;
      color: var(--text-muted);
      font-weight: 600;
    }

    .exp-btn {
      background: var(--panel-alt);
      color: var(--text-dim);
      border: 1px solid var(--border);
      border-radius: 6px;
      padding: 7px 12px;
      font-size: 11px;
      font-weight: 700;
      letter-spacing: 0.03em;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 5px;
      transition: all 0.15s ease;
    }
    .exp-btn:hover {
      background: var(--border);
      color: var(--text);
      border-color: var(--border-focus);
      transform: translateY(-1px);
    }
    .exp-clear:hover {
      border-color: #ef4444;
      color: #ef4444;
    }

    /* --- TEST BATERIÍ --- */
    .batt-big-grid {
      display: grid;
      grid-template-columns: repeat(4, 1fr);
      gap: 10px;
    }

    .batt-box {
      background: var(--panel);
      padding: 16px 18px;
      border-radius: 10px;
      border: 1px solid var(--border);
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      min-height: 108px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.15);
    }

    /* --- KARTY NASTAVENÍ A SYSTÉMU --- */
    .settings-filter-bar {
      display: flex;
      gap: 6px;
      flex-wrap: wrap;
      background: var(--panel);
      padding: 10px 14px;
      border-radius: 10px;
      border: 1px solid var(--border);
      align-items: center;
    }
    .filter-chip {
      background: var(--panel-alt);
      color: var(--text-muted);
      border: 1px solid var(--border);
      padding: 6px 12px;
      border-radius: 6px;
      font-size: 12px;
      font-weight: 700;
      cursor: pointer;
      transition: all 0.15s;
    }
    .filter-chip:hover {
      color: var(--text);
      border-color: var(--border-focus);
    }
    .filter-chip.active {
      background: var(--invert-bg);
      color: var(--invert-text);
      border-color: var(--invert-bg);
    }

    .card-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      gap: 12px;
    }

    .info-card {
      background: var(--panel);
      padding: 18px 20px;
      border-radius: 10px;
      border: 1px solid var(--border);
      display: flex;
      flex-direction: column;
      gap: 12px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.15);
    }

    .card-title {
      font-size: 12px;
      font-weight: 800;
      letter-spacing: 0.05em;
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
      letter-spacing: 0.05em;
      padding: 3px 8px;
      border-radius: 5px;
      background: var(--panel-alt);
      border: 1px solid var(--border);
      color: var(--text-dim);
    }
    .badge-ok {
      background: rgba(34,197,94,0.15);
      border-color: rgba(34,197,94,0.35);
      color: #22c55e;
    }

    .prop-row {
      display: flex;
      justify-content: space-between;
      align-items: center;
      font-size: 13px;
      gap: 8px;
      padding: 2px 0;
    }

    .prop-label {
      color: var(--text-muted);
      font-weight: 500;
    }

    .prop-val {
      font-family: var(--font-mono);
      font-weight: 700;
      color: var(--text);
      text-align: right;
    }

    /* --- BAREVNÉ MOTIVY (THEME CARDS) --- */
    .theme-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(190px, 1fr));
      gap: 10px;
    }

    .theme-card {
      background: var(--panel-alt);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 12px;
      display: flex;
      align-items: center;
      gap: 12px;
      cursor: pointer;
      transition: all 0.15s ease;
    }
    .theme-card:hover {
      border-color: var(--border-focus);
      transform: translateY(-1px);
    }
    .theme-card.active {
      border-color: var(--invert-bg);
      box-shadow: 0 0 0 2px rgba(59, 130, 246, 0.25);
    }

    .theme-swatch {
      width: 28px;
      height: 28px;
      border-radius: 6px;
      border: 2px solid rgba(255,255,255,0.2);
      flex-shrink: 0;
    }

    /* --- ALARM BANNER --- */
    .alarm-banner {
      display: none;
      background: rgba(239, 68, 68, 0.16);
      border: 1px solid #ef4444;
      border-radius: 10px;
      padding: 12px 18px;
      color: #ffffff;
      font-size: 13px;
      font-weight: 700;
      justify-content: space-between;
      align-items: center;
      gap: 12px;
      box-shadow: 0 0 20px rgba(239, 68, 68, 0.3);
    }
    .btn-reset {
      background: #ef4444;
      color: #ffffff;
      border: none;
      padding: 7px 14px;
      border-radius: 6px;
      font-size: 11px;
      font-weight: 800;
      cursor: pointer;
    }
    .btn-reset:hover {
      background: #dc2626;
    }

    @keyframes pulse {
      0% { opacity: 0.55; }
      100% { opacity: 1; }
    }

    @media (max-width: 720px) {
      .readouts, .batt-big-grid {
        grid-template-columns: repeat(2, 1fr);
      }
      .header-bar {
        flex-direction: column;
        align-items: stretch;
      }
      .header-right {
        justify-content: space-between;
      }
    }
  </style>
</head>
<body class="theme-dark">

  <!-- =================================================================== -->
  <!-- VYSOUVACÍ POSTRANNÍ PANEL (GITHUB-STYLE DRAWER MENU ☰)              -->
  <!-- =================================================================== -->
  <div id="sidebarBackdrop" class="sidebar-backdrop" onclick="closeSidebar()"></div>

  <aside id="sidebarDrawer" class="sidebar-drawer" aria-label="Hlavní navigace a nastavení">
    <div class="drawer-header">
      <div class="drawer-brand">
        <div class="drawer-logo">⚡</div>
        <div class="drawer-brand-text">
          <span class="drawer-brand-title" data-i18n="drawer_title">ELEKTRONICKÁ ZÁTĚŽ</span>
          <span class="drawer-brand-sub">30V / 5A / 65W &bull; FW v2.5</span>
        </div>
      </div>
      <button class="drawer-close-btn" onclick="closeSidebar()" title="Zavřít panel (Esc)">✕</button>
    </div>

    <!-- Živý stavový přehled přímo v postranním panelu -->
    <div class="drawer-live-strip">
      <div class="strip-item">
        <span class="strip-lbl">U [V]</span>
        <span class="strip-val" style="color:var(--val-u);" id="sideV">0.00</span>
      </div>
      <div class="strip-item">
        <span class="strip-lbl">I [A]</span>
        <span class="strip-val" style="color:var(--val-i);" id="sideI">0.000</span>
      </div>
      <div class="strip-item">
        <span class="strip-lbl">P [W]</span>
        <span class="strip-val" style="color:var(--val-p);" id="sideP">0.0</span>
      </div>
      <div class="strip-item">
        <span class="strip-lbl">T [°C]</span>
        <span class="strip-val" style="color:var(--val-t);" id="sideT">25.0</span>
      </div>
    </div>

    <div class="drawer-body">
      <!-- Sekce 1: Měření a provoz -->
      <div class="drawer-section">
        <div class="drawer-section-title" data-i18n="nav_sec_ops">Provoz a měření</div>
        <button class="drawer-item active" id="navMain" onclick="navigateFromDrawer('main')">
          <span class="drawer-item-left"><span class="drawer-icon">📊</span> <span data-i18n="nav_dashboard">Hlavní měřicí panel</span></span>
          <span class="drawer-badge">HUD</span>
        </button>
        <button class="drawer-item" id="navBatt" onclick="navigateFromDrawer('batt')">
          <span class="drawer-item-left"><span class="drawer-icon">🔋</span> <span data-i18n="nav_battery">Test kapacity baterií</span></span>
          <span class="drawer-badge">mAh / Ri</span>
        </button>
        <button class="drawer-item" id="navExport" onclick="navigateFromDrawer('export')">
          <span class="drawer-item-left"><span class="drawer-icon">📥</span> <span data-i18n="nav_export">Záznam a export dat</span></span>
          <span class="drawer-badge">CSV / PNG</span>
        </button>
      </div>

      <!-- Sekce 2: Nastavení, ochrany a hardware -->
      <div class="drawer-section">
        <div class="drawer-section-title" data-i18n="nav_sec_cfg">Konfigurace a systém</div>
        <button class="drawer-item" id="navSafety" onclick="navigateFromDrawer('safety')">
          <span class="drawer-item-left"><span class="drawer-icon">🛡️</span> <span data-i18n="nav_safety">Bezpečnostní ochrany</span></span>
          <span class="drawer-badge">OVP/OCP</span>
        </button>
        <button class="drawer-item" id="navCooling" onclick="navigateFromDrawer('cooling')">
          <span class="drawer-item-left"><span class="drawer-icon">❄️</span> <span data-i18n="nav_cooling">Chlazení a ventilátor</span></span>
          <span class="drawer-badge">25 kHz</span>
        </button>
        <button class="drawer-item" id="navHw" onclick="navigateFromDrawer('hw')">
          <span class="drawer-item-left"><span class="drawer-icon">🔧</span> <span data-i18n="nav_hw">Kalibrace a hardware</span></span>
          <span class="drawer-badge">0.100 Ω</span>
        </button>
        <button class="drawer-item" id="navMcu" onclick="navigateFromDrawer('mcu')">
          <span class="drawer-item-left"><span class="drawer-icon">💻</span> <span data-i18n="nav_mcu">Diagnostika ESP32</span></span>
          <span class="drawer-badge">50 Hz</span>
        </button>
        <button class="drawer-item" id="navSettingsAll" onclick="navigateFromDrawer('settings')">
          <span class="drawer-item-left"><span class="drawer-icon">⚙️</span> <span data-i18n="nav_all_settings">Všechna nastavení</span></span>
          <span class="drawer-badge">4 KARTY</span>
        </button>
      </div>

      <!-- Sekce 3: Vzhled a přizpůsobení -->
      <div class="drawer-section">
        <div class="drawer-section-title" data-i18n="nav_sec_ui">Vzhled a rozhraní</div>
        <button class="drawer-item" id="navTheme" onclick="navigateFromDrawer('theme')">
          <span class="drawer-item-left"><span class="drawer-icon">🎨</span> <span data-i18n="nav_themes">Barevné motivy a zvuk</span></span>
          <span class="drawer-badge">5 TÉMAT</span>
        </button>
      </div>
    </div>

    <!-- Patička postranního panelu s rychlou volbou témat -->
    <div class="drawer-footer">
      <div class="drawer-theme-row">
        <span style="font-size:11px;font-weight:700;color:var(--text-muted);" data-i18n="drawer_quick_theme">RYCHLÝ MOTIV:</span>
        <div class="drawer-theme-dots">
          <span class="theme-dot" style="background:#131a26;border-color:#38bdf8;" title="Cyber Dark" onclick="applyTheme('dark')"></span>
          <span class="theme-dot" style="background:#ffb703;" title="Keithley Amber" onclick="applyTheme('amber')"></span>
          <span class="theme-dot" style="background:#22c55e;" title="CRT Green" onclick="applyTheme('green')"></span>
          <span class="theme-dot" style="background:#00e5ff;" title="Neon Cyan" onclick="applyTheme('cyan')"></span>
          <span class="theme-dot" style="background:#ffffff;border-color:#64748b;" title="Clean Light" onclick="applyTheme('light')"></span>
        </div>
      </div>
      <div class="drawer-quick-actions">
        <button class="drawer-qbtn" onclick="toggleAudioFeedback()" id="drawerAudioBtn">🔊 ZVUK: ZAP</button>
        <button class="drawer-qbtn" onclick="toggleLanguage()" id="drawerLangBtn">🌐 CZ / EN</button>
      </div>
    </div>
  </aside>

  <div class="frame">
    <!-- Havarijní výstraha -->
    <div id="alarmBar" class="alarm-banner">
      <span id="alarmReason" data-i18n="alarm_default">SYSTÉMOVÝ ALARM: VYBAVENÍ OCHRANY</span>
      <button class="btn-reset" onclick="ackAlarm()" data-i18n="btn_ack">[ POTVRDIT / RESET ]</button>
    </div>

    <!-- Hlavní horní lišta s tlačítkem 3 čárky (☰) -->
    <div class="header-bar">
      <div class="header-left-wrap">
        <button class="hamburger-btn" id="menuBtn" onclick="toggleSidebar()" title="Otevřít postranní panel (Menu)">
          <svg width="20" height="20" viewBox="0 0 16 16" fill="currentColor">
            <path d="M1 2.75A.75.75 0 0 1 1.75 2h12.5a.75.75 0 0 1 0 1.5H1.75A.75.75 0 0 1 1 2.75Zm0 5A.75.75 0 0 1 1.75 7h12.5a.75.75 0 0 1 0 1.5H1.75A.75.75 0 0 1 1 7.75ZM1.75 12h12.5a.75.75 0 0 1 0 1.5H1.75a.75.75 0 0 1 0-1.5Z"></path>
          </svg>
        </button>
        <div class="header-left">
          <div class="header-title-row">
            <span class="header-title" data-i18n="header_title">DC ELEKTRONICKÁ ZÁTĚŽ // 30V 5A</span>
            <span class="breadcrumb-pill" id="breadcrumbBadge">📊 MĚŘENÍ</span>
          </div>
          <div class="header-sub" data-i18n="header_sub">DIGITÁLNÍ REGULACE &bull; 50 HZ SMYČKA</div>
        </div>
      </div>
      <div class="header-right">
        <span id="linkStatus" class="link-indicator link-ok">● ŽIVĚ</span>
        <button id="langBtn" class="fs-btn" onclick="toggleLanguage()" title="Přepnout jazyk / Toggle language">[ EN ]</button>
        <button id="fsBtn" class="fs-btn" onclick="toggleFullscreen()" title="Fullscreen">[ ⛶ FULL ]</button>
        <span id="stateAnn" class="annunciator ann-off">[ VYP ]</span>
      </div>
    </div>

    <!-- RYCHLÁ TABOVÁ NAVIGACE -->
    <nav class="nav-bar">
      <button class="nav-btn active" id="btnTabMain" onclick="switchTab('main')">
        <span>📊</span> <span data-i18n="tab_main">MĚŘENÍ</span>
      </button>
      <button class="nav-btn" id="btnTabBatt" onclick="switchTab('batt')">
        <span>🔋</span> <span data-i18n="tab_batt">TEST BATERIÍ</span>
      </button>
      <button class="nav-btn" id="btnTabSettings" onclick="switchTab('settings')">
        <span>⚙️</span> <span data-i18n="tab_settings">NASTAVENÍ &amp; SYSTÉM</span>
      </button>
      <button class="nav-btn" id="btnTabTheme" onclick="switchTab('theme')">
        <span>🎨</span> <span data-i18n="tab_theme">VZHLED &amp; MOTIVY</span>
      </button>
    </nav>

    <!-- ================================================================= -->
    <!-- ZÁLOŽKA 1: HLAVNÍ MĚŘENÍ A REGULACE (DASHBOARD)                   -->
    <!-- ================================================================= -->
    <div id="tabMain" class="tab-content active">
      <!-- 4 barevně odlišené digitální měřáky -->
      <div class="readouts">
        <div class="readout-cell readout-v">
          <div class="cell-tag"><span data-i18n="lbl_v">NAPĚTÍ</span> <span class="cell-unit" data-i18n="lbl_v_max">MAX 32V</span></div>
          <div class="cell-num"><span id="valV">00.00</span></div>
          <div class="cell-unit" data-i18n="unit_v">VOLTY DC</div>
        </div>

        <div class="readout-cell readout-i">
          <div class="cell-tag"><span data-i18n="lbl_i">PROUD</span> <span class="cell-unit" data-i18n="lbl_i_max">MAX 5.2A</span></div>
          <div class="cell-num"><span id="valI">0.000</span></div>
          <div class="cell-unit" data-i18n="unit_i">AMPÉRY</div>
        </div>

        <div class="readout-cell readout-p">
          <div class="cell-tag"><span data-i18n="lbl_p">VÝKON</span> <span class="cell-unit" data-i18n="lbl_p_max">MAX 65W</span></div>
          <div class="cell-num"><span id="valP">00.0</span></div>
          <div class="cell-unit" data-i18n="unit_p">WATTY</div>
        </div>

        <div class="readout-cell readout-t">
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

      <!-- Osciloskopický graticule log s přepínačem LIN/LOG stupnice a exportem -->
      <div class="graph-panel" id="exportPanelCard">
        <div class="graph-header">
          <div style="display:flex; align-items:center; gap:8px; flex-wrap:wrap;">
            <span data-i18n="lbl_graph">OSCILOSKOPICKÝ ZÁZNAM</span>
            <span style="color:var(--trace-u);">● U [V]</span>
            <span style="color:var(--trace-i);">● I [A]</span>
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
          <div class="cell-num" style="color:var(--val-i);"><span id="battBigMah">0</span> <span style="font-size:14px;color:var(--text-muted);">mAh</span></div>
          <div class="cell-unit"><span id="battBigAh">0.000</span> Ah</div>
        </div>
        <div class="batt-box">
          <div class="cell-tag" data-i18n="batt_energy">ENERGIE</div>
          <div class="cell-num" style="color:var(--val-p);"><span id="battBigWh">0.00</span> <span style="font-size:14px;color:var(--text-muted);">Wh</span></div>
          <div class="cell-unit" data-i18n="batt_integral">INTEGRÁL U &bull; I &bull; dt</div>
        </div>
        <div class="batt-box">
          <div class="cell-tag" data-i18n="batt_time">DOBA TESTU</div>
          <div class="cell-num" style="color:var(--val-u);"><span id="battTimer">00:00:00</span></div>
          <div class="cell-unit" data-i18n="batt_elapsed">ČAS OD STARTU</div>
        </div>
        <div class="batt-box">
          <div class="cell-tag" data-i18n="batt_ri">VNITŘNÍ ODPOR (Ri)</div>
          <div class="cell-num" style="color:var(--val-t);"><span id="battRi">--</span> <span style="font-size:14px;color:var(--text-muted);">mΩ</span></div>
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
      <!-- Filtrovací lišta sekcí nastavení -->
      <div class="settings-filter-bar">
        <span style="font-size:11px;font-weight:800;color:var(--text-muted);margin-right:4px;">SEKCE:</span>
        <button class="filter-chip active" id="fltAll" onclick="filterSettings('all')">⚙️ Všechny karty</button>
        <button class="filter-chip" id="fltSafety" onclick="filterSettings('safety')">🛡️ Bezpečnostní ochrany</button>
        <button class="filter-chip" id="fltCooling" onclick="filterSettings('cooling')">❄️ Chlazení &amp; PWM</button>
        <button class="filter-chip" id="fltHw" onclick="filterSettings('hw')">🔧 Kalibrace &amp; HW</button>
        <button class="filter-chip" id="fltMcu" onclick="filterSettings('mcu')">💻 Diagnostika ESP32</button>
      </div>

      <div class="card-grid">
        <!-- Karta 1: Bezpečnostní dohled -->
        <div class="info-card" id="cardSafety">
          <div class="card-title">
            <span data-i18n="set_safety_title">🛡️ BEZPEČNOSTNÍ DOHLED (SUPERVISOR)</span>
            <span class="card-badge badge-ok">AKTIVNÍ</span>
          </div>
          <div class="prop-row">
            <span class="prop-label" data-i18n="set_ovp">Přepěťová ochrana (OVP):</span>
            <span class="prop-val">32.0 V (Kritická 34.0 V)</span>
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
            <span data-i18n="set_fan_title">❄️ CHLAZENÍ A VENTILÁTOR</span>
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
        <div class="info-card" id="cardHw">
          <div class="card-title">
            <span data-i18n="set_hw_title">🔧 HARDWAROVÉ KONSTANTY</span>
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
        <div class="info-card" id="cardMcu">
          <div class="card-title">
            <span data-i18n="set_mcu_title">💻 SYSTÉM &amp; MIKROKONTROLÉR</span>
            <span class="card-badge">ESP32</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">MCU:</span>
            <span class="prop-val">ESP32 DevKit V1 (240 MHz)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">Volná RAM (Heap):</span>
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
            <span class="prop-label" data-i18n="set_api">Telemetrické rozhraní:</span>
            <span class="prop-val">JSON HTTP REST (300 ms)</span>
          </div>
          <div class="prop-row">
            <span class="prop-label">Firmware:</span>
            <span class="prop-val">v2.5 Optimized Pro</span>
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
            <span data-i18n="theme_select_title">🎨 BAREVNÝ MOTIV ROZHRANÍ (COLOR THEMES)</span>
            <span class="card-badge" id="themeActiveBadge">CYBER DARK</span>
          </div>

          <div class="theme-grid">
            <div class="theme-card active" id="thCardDark" onclick="applyTheme('dark')">
              <span class="theme-swatch" style="background:#131a26;border-color:#38bdf8;"></span>
              <div>
                <strong style="font-size:12px;">Cyber Dark</strong>
                <div style="font-size:10px;color:var(--text-muted);">Moderní laboratorní HUD</div>
              </div>
            </div>

            <div class="theme-card" id="thCardAmber" onclick="applyTheme('amber')">
              <span class="theme-swatch" style="background:#ffb400;"></span>
              <div>
                <strong style="font-size:12px;">Keithley Amber</strong>
                <div style="font-size:10px;color:var(--text-muted);">Klasický jantarový displej</div>
              </div>
            </div>

            <div class="theme-card" id="thCardGreen" onclick="applyTheme('green')">
              <span class="theme-swatch" style="background:#2bff55;"></span>
              <div>
                <strong style="font-size:12px;">CRT Phosphor</strong>
                <div style="font-size:10px;color:var(--text-muted);">Zelený osciloskop</div>
              </div>
            </div>

            <div class="theme-card" id="thCardCyan" onclick="applyTheme('cyan')">
              <span class="theme-swatch" style="background:#00e5ff;"></span>
              <div>
                <strong style="font-size:12px;">Neon Cyan</strong>
                <div style="font-size:10px;color:var(--text-muted);">Kybernetická modrá</div>
              </div>
            </div>

            <div class="theme-card" id="thCardLight" onclick="applyTheme('light')">
              <span class="theme-swatch" style="background:#ffffff;border-color:#333333;"></span>
              <div>
                <strong style="font-size:12px;">Clean Paper</strong>
                <div style="font-size:10px;color:var(--text-muted);">Světlý tiskový protokol</div>
              </div>
            </div>
          </div>
        </div>

        <!-- Karta chování a zvuku -->
        <div class="info-card">
          <div class="card-title">
            <span data-i18n="theme_opts_title">🔊 PŘIZPŮSOBENÍ ROZHRANÍ A ZVUKU</span>
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

    /* ================================================================= */
    /* OVLÁDÁNÍ POSTRANNÍHO PANELU (SIDEBAR DRAWER)                      */
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
      document.querySelectorAll('.filter-chip').forEach(el => el.classList.remove('active'));
      const cards = {
        safety: document.getElementById('cardSafety'),
        cooling: document.getElementById('cardCooling'),
        hw: document.getElementById('cardHw'),
        mcu: document.getElementById('cardMcu')
      };

      if (sec === 'all') {
        document.getElementById('fltAll').classList.add('active');
        Object.values(cards).forEach(c => { if (c) c.style.display = 'flex'; });
      } else {
        const chipMap = { safety: 'fltSafety', cooling: 'fltCooling', hw: 'fltHw', mcu: 'fltMcu' };
        const ch = document.getElementById(chipMap[sec]);
        if (ch) ch.classList.add('active');
        Object.entries(cards).forEach(([k, c]) => {
          if (c) c.style.display = (k === sec) ? 'flex' : 'none';
        });
      }
    }

    function navigateFromDrawer(dest) {
      closeSidebar();
      const bc = document.getElementById('breadcrumbBadge');

      if (dest === 'main') {
        switchTab('main');
        setActiveDrawerItem('navMain');
        if (bc) bc.innerText = '📊 MĚŘENÍ';
      } else if (dest === 'batt') {
        switchTab('batt');
        setActiveDrawerItem('navBatt');
        if (bc) bc.innerText = '🔋 TEST BATERIÍ';
      } else if (dest === 'export') {
        switchTab('main');
        setActiveDrawerItem('navExport');
        if (bc) bc.innerText = '📥 EXPORT DAT';
        const expCard = document.getElementById('exportPanelCard');
        if (expCard) {
          expCard.scrollIntoView({ behavior: 'smooth', block: 'center' });
          expCard.classList.add('highlight-pulse');
          setTimeout(() => expCard.classList.remove('highlight-pulse'), 1600);
        }
      } else if (dest === 'safety') {
        switchTab('settings');
        filterSettings('safety');
        setActiveDrawerItem('navSafety');
        if (bc) bc.innerText = '🛡️ OCHRANY';
      } else if (dest === 'cooling') {
        switchTab('settings');
        filterSettings('cooling');
        setActiveDrawerItem('navCooling');
        if (bc) bc.innerText = '❄️ CHLAZENÍ';
      } else if (dest === 'hw') {
        switchTab('settings');
        filterSettings('hw');
        setActiveDrawerItem('navHw');
        if (bc) bc.innerText = '🔧 KALIBRACE';
      } else if (dest === 'mcu') {
        switchTab('settings');
        filterSettings('mcu');
        setActiveDrawerItem('navMcu');
        if (bc) bc.innerText = '💻 DIAGNOSTIKA';
      } else if (dest === 'settings') {
        switchTab('settings');
        filterSettings('all');
        setActiveDrawerItem('navSettingsAll');
        if (bc) bc.innerText = '⚙️ NASTAVENÍ';
      } else if (dest === 'theme') {
        switchTab('theme');
        setActiveDrawerItem('navTheme');
        if (bc) bc.innerText = '🎨 VZHLED';
      }
    }

    const I18N = {
      cs: {
        lang_btn: '[ EN ]',
        drawer_title: 'ELEKTRONICKÁ ZÁTĚŽ',
        nav_sec_ops: 'Provoz a měření',
        nav_dashboard: 'Hlavní měřicí panel',
        nav_battery: 'Test kapacity baterií',
        nav_export: 'Záznam a export dat',
        nav_sec_cfg: 'Konfigurace a systém',
        nav_safety: 'Bezpečnostní ochrany',
        nav_cooling: 'Chlazení a ventilátor',
        nav_hw: 'Kalibrace a hardware',
        nav_mcu: 'Diagnostika ESP32',
        nav_all_settings: 'Všechna nastavení',
        nav_sec_ui: 'Vzhled a rozhraní',
        nav_themes: 'Barevné motivy a zvuk',
        drawer_quick_theme: 'RYCHLÝ MOTIV:',
        header_title: 'DC ELEKTRONICKÁ ZÁTĚŽ // 30V 5A',
        header_sub: 'DIGITÁLNÍ REGULACE • 50 HZ SMYČKA',
        live: '● ŽIVĚ',
        offline: '○ ODPOJENO',
        full_enter: '[ ⛶ FULL ]',
        full_exit: '[ ✕ ZAVŘÍT ]',
        tab_main: 'MĚŘENÍ',
        tab_batt: 'TEST BATERIÍ',
        tab_settings: 'NASTAVENÍ & SYSTÉM',
        tab_theme: 'VZHLED & MOTIVY',
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
        btn_csv: '📥 CSV / EXCEL',
        btn_json: '📥 JSON',
        btn_png: '📷 PNG GRAF',
        btn_clear: '✕ SMAZAT',
        batt_cap: 'KAPACITA',
        batt_energy: 'ENERGIE',
        batt_integral: 'INTEGRÁL U • I • dt',
        batt_time: 'DOBA TESTU',
        batt_elapsed: 'ČAS OD STARTU',
        batt_ri: 'VNITŘNÍ ODPOR (Ri)',
        batt_ri_calc: 'ΔU / ΔI ODHAD',
        batt_cfg_title: 'KONFIGURACE TESTU VYBÍJENÍ AKUMULÁTORU',
        batt_ready: 'PŘIPRAVENO',
        batt_running: 'PROBÍHÁ VYBÍJENÍ',
        batt_chem: 'CHEMIE ČLÁNKU:',
        batt_discharge_curr: 'VYBÍJECÍ PROUD:',
        batt_cutoff_volt: 'MEZNÍ VYPNUTÍ (CUTOFF):',
        batt_start_btn: '▶ SPUSTIT TEST',
        batt_stop_btn: '⏹ ZASTAVIT TEST',
        set_safety_title: '🛡️ BEZPEČNOSTNÍ DOHLED (SUPERVISOR)',
        set_ovp: 'Přepěťová ochrana (OVP):',
        set_ocp: 'Nadproudová ochrana (OCP):',
        set_opp: 'Výkonová ochrana (OPP):',
        set_otp: 'Tepelná ochrana (OTP):',
        set_otp_warn: 'Výstraha ventilátoru (100%):',
        set_fan_title: '❄️ CHLAZENÍ A VENTILÁTOR',
        set_fan_freq: 'Frekvence PWM:',
        set_fan_min: 'Náběh chlazení (20%):',
        set_fan_max: 'Plné otáčky (100%):',
        set_fan_duty: 'Aktuální střída ventilátoru:',
        set_hw_title: '🔧 HARDWAROVÉ KONSTANTY',
        set_shunt: 'Proudový bočník R_shunt:',
        set_divider: 'Napěťový dělič vstupu:',
        set_adc: 'A/D Převodník:',
        set_dac: 'D/A Převodník:',
        set_mcu_title: '💻 SYSTÉM & MIKROKONTROLÉR',
        set_loop: 'Perioda regulační smyčky:',
        set_api: 'Telemetrické rozhraní:',
        theme_select_title: '🎨 BAREVNÝ MOTIV ROZHRANÍ (COLOR THEMES)',
        theme_opts_title: '🔊 PŘIZPŮSOBENÍ ROZHRANÍ A ZVUKU',
        theme_sound: 'Zvuková odezva (Pípnutí / Beeper):',
        theme_trace_pts: 'Délka osciloskopické paměti:',
        theme_lang: 'Jazyk rozhraní:',
        sound_on: 'ZAPNUTO',
        sound_off: 'VYPNUTO'
      },
      en: {
        lang_btn: '[ CZ ]',
        drawer_title: 'ELECTRONIC LOAD',
        nav_sec_ops: 'Operation & Measurement',
        nav_dashboard: 'Main Dashboard',
        nav_battery: 'Battery Capacity Test',
        nav_export: 'Data Logger & Export',
        nav_sec_cfg: 'Configuration & System',
        nav_safety: 'Safety Protections',
        nav_cooling: 'Cooling & PWM Fan',
        nav_hw: 'Hardware & Calibration',
        nav_mcu: 'ESP32 Diagnostics',
        nav_all_settings: 'All Settings Overview',
        nav_sec_ui: 'Appearance & UI',
        nav_themes: 'Color Themes & Audio',
        drawer_quick_theme: 'QUICK THEME:',
        header_title: 'DC ELECTRONIC LOAD // 30V 5A',
        header_sub: 'DIGITAL CONTROL • 50 HZ LOOP',
        live: '● LIVE',
        offline: '○ OFFLINE',
        full_enter: '[ ⛶ FULL ]',
        full_exit: '[ ✕ EXIT ]',
        tab_main: 'DASHBOARD',
        tab_batt: 'BATTERY TEST',
        tab_settings: 'SETTINGS & SYSTEM',
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
        lbl_graph: 'OSCILLOSCOPE TRACE',
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
        btn_csv: '📥 CSV / EXCEL',
        btn_json: '📥 JSON',
        btn_png: '📷 PNG CHART',
        btn_clear: '✕ CLEAR',
        batt_cap: 'CAPACITY',
        batt_energy: 'ENERGY',
        batt_integral: 'INTEGRAL U • I • dt',
        batt_time: 'TEST TIME',
        batt_elapsed: 'ELAPSED TIME',
        batt_ri: 'INTERNAL RES. (Ri)',
        batt_ri_calc: 'ΔU / ΔI ESTIMATE',
        batt_cfg_title: 'BATTERY DISCHARGE TEST CONFIGURATION',
        batt_ready: 'READY',
        batt_running: 'DISCHARGING',
        batt_chem: 'CELL CHEMISTRY:',
        batt_discharge_curr: 'DISCHARGE CURRENT:',
        batt_cutoff_volt: 'CUT-OFF VOLTAGE:',
        batt_start_btn: '▶ START TEST',
        batt_stop_btn: '⏹ STOP TEST',
        set_safety_title: '🛡️ SAFETY SUPERVISOR',
        set_ovp: 'Over-Voltage Protection (OVP):',
        set_ocp: 'Over-Current Protection (OCP):',
        set_opp: 'Over-Power Protection (OPP):',
        set_otp: 'Over-Temperature Protection (OTP):',
        set_otp_warn: 'Fan 100% Warning Threshold:',
        set_fan_title: '❄️ COOLING & FAN CONTROL',
        set_fan_freq: 'PWM Frequency:',
        set_fan_min: 'Cooling Start (20%):',
        set_fan_max: 'Full Speed (100%):',
        set_fan_duty: 'Current Fan Duty:',
        set_hw_title: '🔧 HARDWARE CONSTANTS',
        set_shunt: 'Current Sense Shunt R_shunt:',
        set_divider: 'Input Voltage Divider:',
        set_adc: 'A/D Converter:',
        set_dac: 'D/A Converter:',
        set_mcu_title: '💻 SYSTEM & MICROCONTROLLER',
        set_loop: 'Control Loop Period:',
        set_api: 'Telemetry Interface:',
        theme_select_title: '🎨 INTERFACE COLOR THEMES',
        theme_opts_title: '🔊 UI & AUDIO CUSTOMIZATION',
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

    /* PŘEPÍNÁNÍ HLAVNÍCH ZÁLOŽEK */
    function switchTab(tabId) {
      playBeep(720, 0.05);
      document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.tab-content').forEach(c => c.classList.remove('active'));
      const bc = document.getElementById('breadcrumbBadge');

      if (tabId === 'main') {
        document.getElementById('btnTabMain').classList.add('active');
        document.getElementById('tabMain').classList.add('active');
        setActiveDrawerItem('navMain');
        if (bc) bc.innerText = '📊 MĚŘENÍ';
        setTimeout(resize, 60);
      } else if (tabId === 'batt') {
        document.getElementById('btnTabBatt').classList.add('active');
        document.getElementById('tabBatt').classList.add('active');
        setActiveDrawerItem('navBatt');
        if (bc) bc.innerText = '🔋 TEST BATERIÍ';
      } else if (tabId === 'settings') {
        document.getElementById('btnTabSettings').classList.add('active');
        document.getElementById('tabSettings').classList.add('active');
        filterSettings('all');
        setActiveDrawerItem('navSettingsAll');
        if (bc) bc.innerText = '⚙️ NASTAVENÍ';
      } else if (tabId === 'theme') {
        document.getElementById('btnTabTheme').classList.add('active');
        document.getElementById('tabTheme').classList.add('active');
        setActiveDrawerItem('navTheme');
        if (bc) bc.innerText = '🎨 VZHLED';
      }
    }

    /* BAREVNÉ MOTIVY */
    function applyTheme(th) {
      playBeep(900, 0.08);
      currentTheme = th;
      localStorage.setItem('load_theme', th);
      document.body.className = 'theme-' + th;

      document.querySelectorAll('.theme-card').forEach(c => c.classList.remove('active'));
      const mapId = { dark: 'thCardDark', amber: 'thCardAmber', green: 'thCardGreen', cyan: 'thCardCyan', light: 'thCardLight' };
      const names = { dark: 'CYBER DARK', amber: 'KEITHLEY AMBER', green: 'CRT PHOSPHOR', cyan: 'NEON CYAN', light: 'CLEAN PAPER' };
      const el = document.getElementById(mapId[th] || 'thCardDark');
      if (el) el.classList.add('active');
      const bdg = document.getElementById('themeActiveBadge');
      if (bdg) bdg.innerText = names[th] || 'CYBER DARK';
      renderPlot();
    }

    function toggleAudioFeedback() {
      soundEnabled = !soundEnabled;
      localStorage.setItem('load_sound', soundEnabled ? 'true' : 'false');
      if (soundEnabled) playBeep(1040, 0.1);
      updateAudioBtnText();
    }

    function updateAudioBtnText() {
      const t = I18N[currentLang] || I18N.cs;
      const btn = document.getElementById('btnAudioToggle');
      if (btn) btn.innerText = soundEnabled ? t.sound_on : t.sound_off;
      const dBtn = document.getElementById('drawerAudioBtn');
      if (dBtn) dBtn.innerText = soundEnabled ? '🔊 ZVUK: ZAP' : '🔇 ZVUK: VYP';
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
      playBeep(960, 0.08);
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
      playBeep(840, 0.06);
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
    let cssH = 230;
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

    /* --- VYKRESLOVÁNÍ GRAFU (LIN / LOG) S PODPOROU MOTIVŮ --- */
    function renderPlot() {
      ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
      ctx.clearRect(0, 0, cssW, cssH);
      const w = cssW;
      const h = cssH;

      const pL = 46;
      const pR = w - 48;
      const pT = 14;
      const pB = h - 18;
      const plotW = pR - pL;
      const plotH = pB - pT;

      const styles = getComputedStyle(document.body);
      const colBorder = styles.getPropertyValue('--border-focus').trim() || '#3b82f6';
      const colGrid = styles.getPropertyValue('--border').trim() || '#263349';
      const colText = styles.getPropertyValue('--text').trim() || '#f8fafc';
      const colMuted = styles.getPropertyValue('--text-muted').trim() || '#94a3b8';
      const colTraceU = styles.getPropertyValue('--trace-u').trim() || '#38bdf8';
      const colTraceI = styles.getPropertyValue('--trace-i').trim() || '#4ade80';

      // Ohraničení rastru
      ctx.strokeStyle = colGrid;
      ctx.lineWidth = 1;
      ctx.strokeRect(pL, pT, plotW, plotH);

      ctx.font = '700 12px ui-monospace, "SF Mono", "Roboto Mono", Cascadia Code, monospace';
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
          ctx.fillStyle = colTraceU;
          ctx.textAlign = 'right';
          ctx.fillText(vVal.toFixed(0) + 'V', pL - 7, y);

          const iVal = (i / divs) * 5.2;
          ctx.fillStyle = colTraceI;
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
            ctx.fillStyle = colTraceU;
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
          ctx.fillStyle = colTraceI;
          ctx.textAlign = 'left';
          ctx.fillText(t.label, pR + 7, y);
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
        plotTrace('u', false, 0, 32.0, colTraceU, 2.2);
        plotTrace('i', false, 0, 5.2, colTraceI, 2.0);
      } else {
        plotTrace('u', true, 0.1, 32.0, colTraceU, 2.2);
        plotTrace('i', true, 0.001, 5.2, colTraceI, 2.0);
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

      // Aktualizace mini přehledu v postranním panelu
      const sV = document.getElementById('sideV'); if (sV) sV.innerText = vStr;
      const sI = document.getElementById('sideI'); if (sI) sI.innerText = iStr;
      const sP = document.getElementById('sideP'); if (sP) sP.innerText = pStr;
      const sT = document.getElementById('sideT'); if (sT) sT.innerText = tStr;

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

      // Vnitřní odpor (Ri)
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

      if (typeof d.duration === 'number' && mode === 3) {
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
      playBeep(next ? 1200 : 600, 0.08);
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

    async function pushSetpointAndCut(val, cut) {
      try {
        const r = await fetch(`/api/control?mode=${mode}&setpoint=${val}&cut=${cut}`, { cache: 'no-store' });
        if (r.ok) {
          const d = await r.json();
          applyTelemetry(d);
        }
      } catch (err) {}
    }

    /* ================================================================= */
    /* ZÁZNAM MĚŘENÍ A EXPORT DO SOUBORŮ (CSV/EXCEL, JSON, PNG GRAF)     */
    /* ================================================================= */
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
      playBeep(isRecording ? 660 : 1100, 0.08);
      if (!isRecording) {
        if (logData.length === 0) {
          recordStartTime = new Date();
        } else if (!recordStartTime) {
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
        const bgCol = getComputedStyle(document.body).getPropertyValue('--panel').trim() || '#131a26';
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
