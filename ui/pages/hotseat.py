from __future__ import annotations

from textual.app import ComposeResult
from textual.screen import Screen
from textual.containers import Vertical

from power_chess.engine import Engine, State
from ui.widgets.board_view import BoardView
from ui.widgets.control_bar import ControlBar


class HotseatPage(Screen):
    """Two-human hotseat play with interactive board."""

    def __init__(self) -> None:
        super().__init__()
        self.engine = Engine()
        self.state: State = self.engine.initial_state()

    def compose(self) -> ComposeResult:
        with Vertical():
            yield BoardView(self.engine, self.state, id="board")
            yield ControlBar(id="controls")
