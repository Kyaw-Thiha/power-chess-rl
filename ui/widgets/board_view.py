from __future__ import annotations
from typing import Optional

from textual.widgets import DataTable
from textual.reactive import reactive
from textual import on

from power_chess.engine import Engine, State, Move, BOARD_N  # type: ignore[attr-defined]


class BoardView(DataTable):
    """
    6x6 board view using Textual's DataTable.
    - Click once: select origin; legal targets highlight.
    - Click target: performs the move via Engine.apply_move.
    """

    engine: Engine
    state: reactive[State]
    board_size: int

    _selected_from_index: Optional[int]
    _legal_target_indices: set[int]

    def __init__(self, engine: Engine, state: State, id: str | None = None) -> None:
        super().__init__(id=id, zebra_stripes=False, cursor_type="cell")
        self.engine = engine
        self.state = state
        self.board_size = int(BOARD_N)
        self._selected_from_index = None
        self._legal_target_indices = set()

    def on_mount(self) -> None:
        self._build_grid()
        self._refresh_cells()

    # --- Setup

    def _build_grid(self) -> None:
        self.clear(columns=True)
        # columns a..f
        for col in range(self.board_size):
            self.add_column(f"{chr(ord('a') + col)}", width=4)
        # rows 0..5 (engine row order; you can flip if desired)
        for row in range(self.board_size):
            self.add_row(*[" . " for _ in range(self.board_size)], key=str(row))
        self.cursor_coordinate = (self.board_size - 1, 0)  # a1-like start

    # --- Rendering

    def _refresh_cells(self) -> None:
        for flat_index, code in enumerate(self.state.board):
            row = Engine.row(flat_index)
            col = Engine.col(flat_index)
            text = " . " if code == 0 else f"{code:02X}"
            self.update_cell_at((row, col), text)

        # highlights
        self.remove_class_from_cells("data-table--highlight")
        for target_index in self._legal_target_indices:
            row = Engine.row(target_index)
            col = Engine.col(target_index)
            self.add_class_to_cell((row, col), "data-table--highlight")

    # --- Interactions

    @on(DataTable.CellSelected)
    def handle_cell_selected(self, event: DataTable.CellSelected) -> None:
        """Select origin square or apply a legal move."""
        row, col = event.coordinate
        flat_index = Engine.get_pos(row, col)

        if self._selected_from_index is None:
            self._select_origin(flat_index)
        else:
            if flat_index in self._legal_target_indices:
                move = self._find_move(self._selected_from_index, flat_index)
                if move is not None:
                    step_result = self.engine.apply_move(self.state, move)
                    self.state = step_result.state  # use returned state (no manual copy)
            # clear selection either way
            self._selected_from_index = None
            self._legal_target_indices = set()
            self._refresh_cells()

    def _select_origin(self, from_index: int) -> None:
        self._selected_from_index = from_index
        legal_moves = self.engine.legal_moves_from(self.state, from_index)
        self._legal_target_indices = {m.to for m in legal_moves}
        self._refresh_cells()

    def _find_move(self, from_index: int, to_index: int) -> Optional[Move]:
        for move in self.engine.legal_moves_from(self.state, from_index):
            if move.to == to_index:
                return move
        return None

    # --- External API

    def set_state(self, new_state: State) -> None:
        """Replace board state and repaint."""
        self.state = new_state
        self._refresh_cells()
