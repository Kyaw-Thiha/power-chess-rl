from __future__ import annotations
from typing import Optional, Callable, cast

from rich.text import Text
from textual.widgets import DataTable
from textual.reactive import reactive
from textual import on
from textual.coordinate import Coordinate as DtCoordinate

from power_chess.engine import Engine, State, Move, BOARD_N

# Type for a piece-decoder: given an engine code -> display string (ASCII or Unicode)
PieceTextFn = Callable[[int], str]


KIND_MASK = 0b0000_0111  # bits 0..2
MOVED_MASK = 0b0000_1000  # bit 3 (unused for glyph)
SIDE_MASK = 0b0001_0000  # bit 4 (0=P1, 1=P2)
POWER_MASK = 0b1110_0000  # bits 5..7 (unused for glyph)

# kind values (match your UnitType)
K_EMPTY, K_PAWN, K_KNIGHT, K_BISHOP, K_ROOK, K_QUEEN, K_KING = 0, 1, 2, 3, 4, 5, 6


def infer_unicode_piece(code: int) -> str:
    """
    Decode your piece::Code per piece.hpp:
      - kind  = code & KIND_MASK (0 empty; 1..6 = P,N,B,R,Q,K)
      - side  = (code & SIDE_MASK) == 0  -> P1, else P2
      - moved/power bits are ignored for glyphs
    P1 is rendered as white; P2 as black.
    """
    kind = code & KIND_MASK
    if kind == K_EMPTY:
        return "·"

    is_p2 = (code & SIDE_MASK) != 0  # bit 4 set => P2

    WHITE = {
        K_PAWN: "♙",
        K_KNIGHT: "♘",
        K_BISHOP: "♗",
        K_ROOK: "♖",
        K_QUEEN: "♕",
        K_KING: "♔",
    }
    BLACK = {
        K_PAWN: "♟",
        K_KNIGHT: "♞",
        K_BISHOP: "♝",
        K_ROOK: "♜",
        K_QUEEN: "♛",
        K_KING: "♚",
    }

    table = BLACK if is_p2 else WHITE
    return table.get(kind, "·")


class BoardView(DataTable):
    """
    6x6 board using Textual's DataTable.

    Mouse:
      - click once: select origin; legal targets highlight
      - click target: applies move

    Keyboard:
      - arrows / jklh: move a logical cursor
      - enter/space: select/apply at cursor
    """

    BINDINGS = [
        ("up", "cursor_up", ""),
        ("down", "cursor_down", ""),
        ("left", "cursor_left", ""),
        ("right", "cursor_right", ""),
        ("k", "cursor_up", ""),
        ("j", "cursor_down", ""),
        ("h", "cursor_left", ""),
        ("l", "cursor_right", ""),
        ("enter", "cursor_select", ""),
        ("space", "cursor_select", ""),
    ]

    engine: Engine
    state: reactive[State]
    board_size: int

    # Public hook: set a function that returns the display text for a piece code.
    # Defaults to Unicode chess glyphs inferred from typical code scheme.
    piece_text_fn: PieceTextFn

    _selected_from_index: Optional[int]
    _legal_target_indices: set[int]

    _cur_row: int
    _cur_col: int

    def __init__(
        self,
        engine: Engine,
        state: State,
        id: str | None = None,
        piece_text_fn: PieceTextFn | None = None,
    ) -> None:
        super().__init__(id=id, zebra_stripes=False, cursor_type="cell")
        self.engine = engine
        self.state = state
        self.board_size = int(BOARD_N)
        self._selected_from_index = None
        self._legal_target_indices = set()
        self._cur_row = self.board_size - 1
        self._cur_col = 0

        # Default to Unicode glyph inference; callers can override with a custom mapper.
        self.piece_text_fn = piece_text_fn or infer_unicode_piece

        self.can_focus = True

    # --- Utilities ----------------------------------------------------------

    @staticmethod
    def _coord(row: int, col: int) -> DtCoordinate:
        return cast(DtCoordinate, (row, col))

    def _clamp_rc(self, r: int, c: int) -> tuple[int, int]:
        r = max(0, min(self.board_size - 1, r))
        c = max(0, min(self.board_size - 1, c))
        return r, c

    # --- Lifecycle ----------------------------------------------------------

    def on_mount(self) -> None:
        self._build_grid()
        self.cursor_coordinate = self._coord(self._cur_row, self._cur_col)
        self._refresh_cells()

    # --- Setup --------------------------------------------------------------

    def _build_grid(self) -> None:
        self.clear(columns=True)
        for col in range(self.board_size):
            self.add_column(f"{chr(ord('a') + col)}", width=6)  # wider cells for glyphs
        for row in range(self.board_size):
            self.add_row(*["     " for _ in range(self.board_size)], key=str(row))

    # --- Rendering ----------------------------------------------------------

    def _cell_text(self, flat_index: int) -> Text:
        code = self.state.board[flat_index]
        # Render piece
        disp = self.piece_text_fn(code)

        # Pad so cells feel larger and centered
        txt = Text(f"  {disp}  ")

        # Highlights
        if flat_index in self._legal_target_indices:
            txt.stylize("bold on #2d3f76")
        if self._selected_from_index is not None and flat_index == self._selected_from_index:
            txt.stylize("bold reverse")

        return txt

    def _refresh_cells(self) -> None:
        for flat_index, _code in enumerate(self.state.board):
            row = Engine.row(flat_index)
            col = Engine.col(flat_index)
            self.update_cell_at(self._coord(row, col), self._cell_text(flat_index))

    # --- Interactions: Mouse/Keyboard unified ------------------------------

    @on(DataTable.CellSelected)
    def handle_cell_selected(self, event: DataTable.CellSelected) -> None:
        row, col = event.coordinate
        self._cur_row, self._cur_col = self._clamp_rc(row, col)
        self._activate_at_rc(self._cur_row, self._cur_col)

    def action_cursor_up(self) -> None:
        self._move_cursor(-1, 0)

    def action_cursor_down(self) -> None:
        self._move_cursor(+1, 0)

    def action_cursor_left(self) -> None:
        self._move_cursor(0, -1)

    def action_cursor_right(self) -> None:
        self._move_cursor(0, +1)

    def _move_cursor(self, d_row: int, d_col: int) -> None:
        r, c = self._clamp_rc(self._cur_row + d_row, self._cur_col + d_col)
        self._cur_row, self._cur_col = r, c
        self.cursor_coordinate = self._coord(r, c)

    def action_cursor_select(self) -> None:
        self._activate_at_rc(self._cur_row, self._cur_col)

    def _activate_at_rc(self, row: int, col: int) -> None:
        flat_index = Engine.get_pos(row, col)
        if self._selected_from_index is None:
            self._select_origin(flat_index)
            return
        if flat_index in self._legal_target_indices:
            move = self._find_move(self._selected_from_index, flat_index)
            if move is not None:
                step_result = self.engine.apply_move(self.state, move)
                self.state = step_result.state
        self._selected_from_index = None
        self._legal_target_indices = set()
        self._refresh_cells()

    # --- Move helpers -------------------------------------------------------

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
        self.state = new_state
        self._refresh_cells()
