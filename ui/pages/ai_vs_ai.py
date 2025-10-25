from __future__ import annotations

from textual.app import ComposeResult
from textual.screen import Screen
from textual.containers import Vertical
from textual import on

from power_chess.engine import Engine, State  # type: ignore[attr-defined]
from services.ai_policy import RandomPolicy
from widgets.board_view import BoardView
from widgets.control_bar import ControlBar


class AIVsAIPage(Screen):
    """AI vs AI; press Step to advance a move."""

    def __init__(self) -> None:
        super().__init__()
        self.engine = Engine()
        self.state: State = self.engine.initial_state()
        self.ai_policy_0 = RandomPolicy(seed=1)
        self.ai_policy_1 = RandomPolicy(seed=2)

    def compose(self) -> ComposeResult:
        with Vertical():
            yield BoardView(self.engine, self.state, id="board")
            yield ControlBar(id="controls")

    @on(ControlBar.Step)
    def _step(self) -> None:
        policy = self.ai_policy_0 if self.state.to_move == 0 else self.ai_policy_1
        move = policy.select(self.engine, self.state)
        if move is not None:
            step_result = self.engine.apply_move(self.state, move)
            self.state = step_result.state
            self.query_one("#board", BoardView).set_state(self.state)
