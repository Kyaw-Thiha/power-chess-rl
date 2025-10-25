from __future__ import annotations

# Tokyonight (storm-ish) styling for Textual — pane-focused, lazygit vibe
TOKYONIGHT_CSS = """
/* ───────────── LAYOUT SCAFFOLD ───────────── */

#root-row { height: 1fr; }

#main-panel {
  height: 1fr;
  border: solid #2e3a59;      /* sharper than #1d2330 */
  background: #16161e;
  padding: 0;
}
#main-panel.-active-tab {
  border: solid #ff9e64;      /* orange when a tab is active/selected */
}

#main {
  height: 1fr;
  min-width: 0;               /* prevent horizontal overflow */
  overflow: hidden;
  background: #16161e;

  /* Center its content (e.g., the Board) both ways */
  content-align: center middle;
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

/* Bottom bars */
ControlBar {
  dock: bottom;
  height: 3;
  border: solid #2e3a59;
  background: #0f1117;
}

#status {
  dock: bottom;
  height: 1;
  background: #0f1117;
  color: #7aa2f7;
}

/* ───────────── NAV LOOK & FEEL ───────────── */

/* Style all nav buttons with Tokyonight tone (keeps your .item class) */
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
  /* Keep DataTable aesthetic, but let it breathe and center nicely */
  background: #16161e;
  color: #c0caf5;
  border: solid #2e3a59;
  margin: 1 2;
}

/* If you name your board nodes #board, give them a tad of presence */
#board {
  /* let it center within #main; DataTable won't stretch columns, but it will be centered */
  content-align: center middle;
}

/* Pickers */
AgentPicker, ReplayPicker {
  height: auto;
  background: #0b0d14;
  border: solid #2e3a59;
  padding: 1 1;
}

/* DataTable highlights */
.data-table--cursor { background: #24283b; color: #e0e7ff; }
.data-table--highlight { background: #1f2335; color: #9ece6a; }
"""
