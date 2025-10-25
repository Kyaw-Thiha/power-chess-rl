from __future__ import annotations
from typing import Optional

from textual.app import ComposeResult
from textual.screen import Screen
from textual.containers import Vertical
from textual import on

from power_chess.engine import Engine, State
from ui.models.types import AgentConfig
from ui.services.ai_policy import RandomPolicy
from ui.widgets.board_view import BoardView
from ui.widgets.agent_picker import AgentPicker
from ui.widgets.control_bar import ControlBar


class VsAIPage(Screen):
    """Human vs AI; by default side 1 (player-1) is AI."""

    def __init__(self) -> None:
        super().__init__()
        self.engine = Engine()
        self.state: State = self.engine.initial_state()
        self.ai_policy: RandomPolicy = RandomPolicy()
        self.agent_configuration: Optional[AgentConfig] = None

    def compose(self) -> ComposeResult:
        with Vertical():
            yield AgentPicker(id="agent-picker")
            yield BoardView(self.engine, self.state, id="board")
            yield ControlBar(id="controls")

    @on(AgentPicker.AgentChosen)
    def _set_agent(self, event: AgentPicker.AgentChosen) -> None:
        self.agent_configuration = event.config
        # In the future: instantiate policy based on self.agent_configuration.name / checkpoint

    @on(ControlBar.Step)
    def _ai_step(self) -> None:
        """If it is AI's turn (to_move == 1), make one AI move."""
        if self.state.to_move == 1:
            move = self.ai_policy.select(self.engine, self.state)
            if move is not None:
                step_result = self.engine.apply_move(self.state, move)
                self.state = step_result.state
                self.query_one("#board", BoardView).set_state(self.state)
