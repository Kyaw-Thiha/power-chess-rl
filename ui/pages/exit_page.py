from __future__ import annotations
from typing import Callable

from textual.app import ComposeResult
from textual.screen import Screen
from textual.widgets import Static, Button
from textual.containers import Vertical
from textual import on


class ExitPage(Screen):
    """Exit confirmation page."""

    def __init__(self, on_confirm: Callable[[], None]) -> None:
        super().__init__()
        self._on_confirm = on_confirm

    def compose(self) -> ComposeResult:
        with Vertical():
            yield Static("Exit PowerChess RL?", id="exit-text")
            yield Button("Confirm Exit", id="confirm-exit")

    @on(Button.Pressed, "#confirm-exit")
    def _confirm(self) -> None:
        self._on_confirm()
