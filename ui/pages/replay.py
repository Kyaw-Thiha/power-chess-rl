from __future__ import annotations
from typing import List

import json
from textual.app import ComposeResult
from textual.screen import Screen
from textual.containers import Vertical
from textual import on

from power_chess.engine import Engine, State, Move
from ui.models.types import ReplayEntry
from ui.widgets.board_view import BoardView
from ui.widgets.control_bar import ControlBar
from ui.widgets.replay_picker import ReplayPicker


class ReplayPage(Screen):
    """Replay past games from a JSONL where each line is a move dict."""

    def __init__(self) -> None:
        super().__init__()
        self.engine = Engine()
        self.state: State = self.engine.initial_state()
        self.moves: List[Move] = []
        self.cursor: int = 0

    def compose(self) -> ComposeResult:
        with Vertical():
            yield ReplayPicker(id="replay-picker")
            yield BoardView(self.engine, self.state, id="board")
            yield ControlBar(id="controls")

    @on(ReplayPicker.ReplayChosen)
    def _load(self, event: ReplayPicker.ReplayChosen) -> None:
        self.moves = self._read_replay(event.entry)
        self.cursor = 0
        self.state = self.engine.initial_state()
        self.query_one("#board", BoardView).set_state(self.state)

    @on(ControlBar.Step)
    def _step(self) -> None:
        if self.cursor < len(self.moves):
            move = self.moves[self.cursor]
            self.cursor += 1
            step_result = self.engine.apply_move(self.state, move)
            self.state = step_result.state
            self.query_one("#board", BoardView).set_state(self.state)

    # --- Helpers

    def _read_replay(self, entry: ReplayEntry) -> List[Move]:
        out: List[Move] = []
        with open(entry.path, "r", encoding="utf-8") as fh:
            for line in fh:
                rec = json.loads(line)
                move = Move()
                # Accept either 'from' or 'from_' in file
                move.from_ = int(rec.get("from_", rec.get("from")))
                move.to = int(rec["to"])
                move.type = rec.get("type", 0)
                move.promo_piece = rec.get("promo_piece", 0)
                move.special_code = rec.get("special_code", 0)
                out.append(move)
        return out
