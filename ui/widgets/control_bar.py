from __future__ import annotations

from textual.widget import Widget
from textual.containers import Horizontal
from textual.widgets import Button, Label
from textual import on
from textual.message import Message


class ControlBar(Widget):
    """Bottom control bar: ▶ Start   ▷ Step   Speed 1× [−][+]"""

    class Start(Message):
        """Start a continuous stepping loop (optional; page decides behavior)."""

        pass

    class Step(Message):
        """Advance a single step (page decides what 'step' does)."""

        pass

    class SpeedChanged(Message):
        def __init__(self, speed_multiplier: float) -> None:
            self.speed_multiplier = speed_multiplier
            super().__init__()

    def __init__(self, initial_speed_multiplier: float = 1.0, id: str | None = None) -> None:
        super().__init__(id=id)
        self._speed_multiplier = initial_speed_multiplier

    def compose(self):
        with Horizontal():
            yield Button("▶ Start", id="start")
            yield Button("▷ Step", id="step")
            yield Label(f"Speed {self._speed_multiplier:.0f}×", id="speed-label")
            yield Button("−", id="speed-down")
            yield Button("+", id="speed-up")

    @on(Button.Pressed, "#start")
    def _on_start(self) -> None:
        self.post_message(self.Start())

    @on(Button.Pressed, "#step")
    def _on_step(self) -> None:
        self.post_message(self.Step())

    @on(Button.Pressed, "#speed-down")
    def _on_speed_down(self) -> None:
        self._speed_multiplier = max(0.25, self._speed_multiplier / 2)
        self.query_one("#speed-label", Label).update(f"Speed {self._speed_multiplier:.2g}×")
        self.post_message(self.SpeedChanged(self._speed_multiplier))

    @on(Button.Pressed, "#speed-up")
    def _on_speed_up(self) -> None:
        self._speed_multiplier = min(8.0, self._speed_multiplier * 2)
        self.query_one("#speed-label", Label).update(f"Speed {self._speed_multiplier:.2g}×")
        self.post_message(self.SpeedChanged(self._speed_multiplier))
