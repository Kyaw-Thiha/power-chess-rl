from __future__ import annotations

# Tokyonight (storm-ish) styling for Textual — compatible with your Textual version
TOKYONIGHT_CSS = """
/* ───────────────────────── LAYOUT SCAFFOLD ───────────────────────── */

#root-row {
  height: 1fr;                /* fill available vertical space */
}

/* Left main panel (outer shell); #main mounts pages inside it */
#main-panel {
  height: 1fr;
  border: solid #1d2330;
  background: #16161e;
  padding: 0;                 /* crisp like lazygit panels */
}

/* Inner content area that hosts the current page */
#main {
  height: 1fr;
  min-width: 0;               /* IMPORTANT: allow shrinking to prevent overflow */
  overflow: hidden;
  background: #16161e;
}

/* Right-side navigation rail */
#nav {
  dock: right;
  width: 18;                  /* tweak to taste: 16–22 */
  min-width: 16;
  max-width: 24;
  border: solid #1d2330;
  background: #1a1b26;
  color: #c0caf5;
}

/* Bottom bars */
ControlBar {
  dock: bottom;
  height: 3;
  border: solid #1d2330;
  background: #0f1117;
}

#status {
  dock: bottom;
  height: 1;
  background: #0f1117;
  color: #7aa2f7;
}

/* ───────────────────────── NAV LOOK & FEEL ───────────────────────── */

NavTabs > .item {
  padding: 0 1;               /* tighter, terminal-y */
  height: 1;                  /* one-row entries, lazygit-like */
  content-align: left middle;
}

NavTabs > .item.-active {
  background: #24283b;
  color: #7dcfff;
  text-style: bold;
}

/* ───────────────────────── WIDGET THEMES ─────────────────────────── */

BoardView {
  background: #16161e;
  color: #c0caf5;
  border: solid #1d2330;
}

AgentPicker, ReplayPicker {
  height: auto;
  background: #0b0d14;
  border: solid #1d2330;
  padding: 1 1;
}

/* DataTable highlights */
.data-table--cursor {
  background: #24283b;
  color: #e0e7ff;
}

.data-table--highlight {
  background: #1f2335;
  color: #9ece6a;
}
"""
