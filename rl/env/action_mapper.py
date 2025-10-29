from __future__ import annotations

from dataclasses import dataclass
from typing import Dict, Iterable, List

from power_chess.engine import Move, MoveType


@dataclass(frozen=True)
class MoveKey:
    """Immutable signature for a move used to index the discrete action space."""

    from_square: int
    to_square: int
    move_type: MoveType
    promo_piece: int
    special_code: int


class DiscreteActionMapper:
    """Bi-directional mapping between engine moves and discrete action ids."""

    def __init__(self, max_actions: int) -> None:
        self._max_actions = max_actions
        self._move_to_id: Dict[MoveKey, int] = {}
        self._id_to_move: Dict[int, MoveKey] = {}

    @property
    def size(self) -> int:
        """Return the size of the discrete action space."""
        return self._max_actions

    def register_moves(self, moves: Iterable[Move]) -> List[int]:
        """Register moves and return their corresponding action ids."""
        action_ids: List[int] = []
        for move in moves:
            key = self._move_to_key(move)
            if key not in self._move_to_id:
                self._register_new_move(key)
            action_ids.append(self._move_to_id[key])
        return action_ids

    def build_move(self, action_id: int) -> Move:
        """Instantiate a Move from its action id."""
        if action_id not in self._id_to_move:
            raise KeyError(f"Unknown action id: {action_id}")
        key = self._id_to_move[action_id]
        move = Move()
        move.from_ = key.from_square
        move.to = key.to_square
        move.type = key.move_type
        move.promo_piece = key.promo_piece
        move.special_code = key.special_code
        return move

    def _register_new_move(self, key: MoveKey) -> None:
        if len(self._move_to_id) >= self._max_actions:
            raise RuntimeError("Action mapper exhausted capacity for unique moves.")
        new_id = len(self._move_to_id)
        self._move_to_id[key] = new_id
        self._id_to_move[new_id] = key

    @staticmethod
    def _move_to_key(move: Move) -> MoveKey:
        return MoveKey(
            from_square=move.from_,
            to_square=move.to,
            move_type=move.type,
            promo_piece=move.promo_piece,
            special_code=move.special_code,
        )
