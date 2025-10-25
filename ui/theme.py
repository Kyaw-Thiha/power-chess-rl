from __future__ import annotations

# Tokyonight (storm-ish) styling for Textual — pane-focused, lazygit vibe
TOKYONIGHT_CSS = """
/* ───────────── LAYOUT SCAFFOLD ───────────── */

#titlebar {
  dock: top;
  height: 1;
  border: solid #2e3a59;
  background: #0f1117;
  color: #c0caf5;
  content-align: center middle;
}

#root-row { height: 1fr; }

#main-panel {
  height: 1fr;
  border: solid #2e3a59;      /* sharper than #1d2330 */
  background: #16161e;
  padding: 0;
}
#main-panel.-active-pane { border: solid #ff9e64; }

#main {
  height: 1fr;
  min-width: 0;               /* prevent horizontal overflow */
  overflow: hidden;
  background: #16161e;
  content-align: center middle;  /* center page content (board, etc.) */
}

/* Right-side navigation rail */
#nav {
  dock: right;
  width: 18;                  /* tweak: 16–22 */
  min-width: 16;
  max-width: 24;
  border: solid #2e3a59;
  background: #1a1b26;
  color: #c0caf5;
}
#nav.-active-pane { border: solid #ff9e64; }

/* Bottom bars */
ControlBar {
  dock: bottom;
  height: 3;
  border: solid #2e3a59;
  background: #0f1117;
}
ControlBar.-active-pane { border: solid #ff9e64; }

#status {
  dock: bottom;
  height: 1;
  background: #0f1117;
  color: #7aa2f7;
}

/* ───────────── NAV LOOK & FEEL ───────────── */

NavTabs Button.item {
  padding: 0 1;
  height: 1;
  content-align: left middle;
  background: #1a1b26;
  color: #c0caf5;
  border: none;
}
NavTabs Button.item:hover {
  background: #1f2335;
  color: #a9b1d6;
}
NavTabs Button.item:focus {
  background: #24283b;
  color: #c0caf5;
  text-style: bold;
  outline: none;
}
NavTabs Button.item.-active {
  background: #24283b;
  color: #7dcfff;
  text-style: bold;
}

/* ───────────── BOARD / PAGES ───────────── */

BoardView {
  background: #16161e;
  color: #c0caf5;
  border: solid #2e3a59;
  margin: 1 2;
}

/* Stronger presence for #board */
#board { content-align: center middle; }

/* Pickers */
AgentPicker, ReplayPicker {
  height: auto;
  background: #0b0d14;
  border: solid #2e3a59;
  padding: 1 1;
}

/* DataTable highlights */
.data-table--cursor  { background: #24283b; color: #e0e7ff; }
.data-table--highlight { background: #1f2335; color: #9ece6a; }
"""
