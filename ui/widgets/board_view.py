from __future__ import annotations
from typing import Optional, cast

from rich.text import Text
from textual.widgets import DataTable
from textual.reactive import reactive
from textual import on
from textual.coordinate import Coordinate as DtCoordinate

from power_chess.engine import Engine, State, Move, BOARD_N


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
        self.state = state  # reactive is fine to assign like this at runtime
        self.board_size = int(BOARD_N)
        self._selected_from_index = None
        self._legal_target_indices = set()

    # --- Utilities ----------------------------------------------------------

    @staticmethod
    def _coord(row: int, col: int) -> DtCoordinate:
        """Typed coordinate helper so Pyright is satisfied."""
        return cast(DtCoordinate, (row, col))

    # --- Lifecycle ----------------------------------------------------------

    def on_mount(self) -> None:
        self._build_grid()
        self._refresh_cells()

    # --- Setup --------------------------------------------------------------

    def _build_grid(self) -> None:
        self.clear(columns=True)
        # columns a..f
        for col in range(self.board_size):
            self.add_column(f"{chr(ord('a') + col)}", width=4)
        # rows 0..5 (engine row order; you can flip if desired)
        for row in range(self.board_size):
            self.add_row(*[" . " for _ in range(self.board_size)], key=str(row))

        # Start cursor at bottom-left (like a1). Cast to DtCoordinate for typing.
        self.cursor_coordinate = self._coord(self.board_size - 1, 0)

    # --- Rendering ----------------------------------------------------------

    def _cell_text(self, flat_index: int) -> Text:
        """Return styled Text for a given flat board index (with highlights)."""
        code = self.state.board[flat_index]
        row = Engine.row(flat_index)
        col = Engine.col(flat_index)

        # Base content
        txt = Text(" . ") if code == 0 else Text(f"{code:02X}")

        # Highlight legal targets
        if flat_index in self._legal_target_indices:
            # A subtle tokyonight-like highlight: bold + background tint
            txt.stylize("bold on #2d3f76")  # adjust to taste

        # Highlight selected origin (optional but helpful)
        if self._selected_from_index is not None and flat_index == self._selected_from_index:
            txt.stylize("bold reverse")  # invert as a cursor-like mark

        return txt

    def _refresh_cells(self) -> None:
        # Repaint every cell with (possibly) styled Text
        for flat_index, _code in enumerate(self.state.board):
            row = Engine.row(flat_index)
            col = Engine.col(flat_index)
            self.update_cell_at(self._coord(row, col), self._cell_text(flat_index))

    # --- Interactions -------------------------------------------------------

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
                    self.state = step_result.state  # take returned state
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

    # --- External API -------------------------------------------------------

    def set_state(self, new_state: State) -> None:
        """Replace board state and repaint."""
        self.state = new_state
        self._refresh_cells()
