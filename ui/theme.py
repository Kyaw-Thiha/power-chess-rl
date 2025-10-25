from __future__ import annotations

# Tokyonight (storm-ish) styling for Textual
TOKYONIGHT_CSS = """
#root-row {
  height: 1fr;
}

#nav {
  dock: right;
  width: 16;
  border: solid #1d2330;
  background: #1a1b26;
  color: #c0caf5;
}

#main {
  height: 1fr;
  background: #16161e;
}

#status {
  dock: bottom;
  height: 1;
  background: #0f1117;
  color: #7aa2f7;
}

NavTabs > .item {
  padding: 1 2;
  content-align: left middle;
}

NavTabs > .item.-active {
  background: #24283b;
  color: #7dcfff;
  text-style: bold;
}

BoardView {
  background: #16161e;
  color: #c0caf5;
  border: solid #1d2330;
}

ControlBar {
  dock: bottom;
  height: 3;
  border: solid #1d2330;
  background: #0f1117;
}

AgentPicker, ReplayPicker {
  height: auto;
  background: #0b0d14;
  border: solid #1d2330;
  padding: 1 1;
}

.data-table--cursor {
  background: #24283b;
  color: #e0e7ff;
}

.data-table--highlight {
  background: #1f2335;
  color: #9ece6a;
}
"""
