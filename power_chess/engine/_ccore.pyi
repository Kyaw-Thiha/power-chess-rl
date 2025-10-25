from __future__ import annotations
from typing import List

BOARD_N: int

class MoveType:
    Quiet: MoveType
    Capture: MoveType
    Promote: MoveType
    CapturePromote: MoveType
    Special: MoveType

class Move:
    def __init__(self) -> None: ...
    from_: int
    to: int
    type: MoveType
    promo_piece: int
    special_code: int

class StepResult:
    def __init__(self) -> None: ...
    state: State
    done: bool
    reward_p0: float
    info: str

class State:
    def __init__(self) -> None: ...
    board: List[int]  # len = BOARD_N * BOARD_N
    to_move: int  # 0 or 1
    ply: int

class Engine:
    def __init__(self) -> None: ...
    def initial_state(self) -> State: ...
    def legal_moves(self, state: State) -> list[Move]: ...
    def legal_moves_from(self, state: State, from_: int) -> list[Move]: ...
    def group_legal_moves_by_from(self, state: State) -> list[list[Move]]: ...
    def is_legal(self, state: State, move: Move) -> bool: ...
    def apply_move(self, state: State, move: Move) -> StepResult: ...
    @staticmethod
    def get_pos(row: int, col: int) -> int: ...
    @staticmethod
    def row(idx: int) -> int: ...
    @staticmethod
    def col(idx: int) -> int: ...
