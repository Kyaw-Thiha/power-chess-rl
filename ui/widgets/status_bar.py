from __future__ import annotations
from textual.widget import Widget
from textual.widgets import Label


class StatusBar(Widget):
    """Single-line status bar."""

    def compose(self):
        yield Label("Ready.", id="status-msg")

    def set_message(self, message: str) -> None:
        self.query_one("#status-msg", Label).update(message)
