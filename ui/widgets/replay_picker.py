from __future__ import annotations
from textual.widget import Widget
from textual.containers import Vertical, Horizontal
from textual.widgets import Label, Input, Button
from textual.message import Message
from textual import on

from models.types import ReplayEntry


class ReplayPicker(Widget):
    """File picker for replays."""

    class ReplayChosen(Message):
        def __init__(self, entry: ReplayEntry) -> None:
            self.entry = entry
            super().__init__()

    def compose(self):
        with Vertical():
            yield Label("Replay", id="replay-title")
            with Horizontal():
                yield Label("File:")
                yield Input("", placeholder="/path/to/replay.jsonl", id="replay-path")
            yield Button("Load Replay", id="load-replay")

    @on(Button.Pressed, "#load-replay")
    def _load(self) -> None:
        path = self.query_one("#replay-path", Input).value.strip()
        if path:
            self.post_message(self.ReplayChosen(ReplayEntry(path=path)))
