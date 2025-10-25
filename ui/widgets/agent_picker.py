from __future__ import annotations
from typing import Optional

from textual.widget import Widget
from textual.containers import Vertical, Horizontal
from textual.widgets import Label, Input, Button, Select
from textual.message import Message
from textual import on

from models.types import AgentConfig


class AgentPicker(Widget):
    """Agent + checkpoint selector."""

    class AgentChosen(Message):
        def __init__(self, config: AgentConfig) -> None:
            self.config = config
            super().__init__()

    def __init__(
        self, default_name: str = "RandomPolicy", default_checkpoint_path: Optional[str] = None, id: str | None = None
    ) -> None:
        super().__init__(id=id)
        self._default_name = default_name
        self._default_checkpoint = default_checkpoint_path

    def compose(self):
        with Vertical():
            yield Label("Agent", id="agent-title")
            yield Select(((n, n) for n in ["RandomPolicy"]), id="agent-name", value=self._default_name)
            with Horizontal():
                yield Label("Checkpoint:")
                yield Input(self._default_checkpoint or "", placeholder="/path/to/checkpoint.pt", id="agent-ckpt")
            yield Button("Use Agent", id="apply-agent")

    @on(Button.Pressed, "#apply-agent")
    def _apply(self) -> None:
        name = self.query_one("#agent-name", Select).value or "RandomPolicy"
        ckpt = self.query_one("#agent-ckpt", Input).value or None
        self.post_message(self.AgentChosen(AgentConfig(name=str(name), checkpoint_path=ckpt)))
