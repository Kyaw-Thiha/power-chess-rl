from __future__ import annotations
import random
from typing import Optional

from power_chess.engine import Engine, State, Move


class RandomPolicy:
    """Placeholder AI that uniformly samples a legal move."""

    def __init__(self, seed: Optional[int] = None) -> None:
        self._rng = random.Random(seed)

    def select(self, engine: Engine, state: State) -> Optional[Move]:
        legal_moves = engine.legal_moves(state)
        if not legal_moves:
            return None
        return self._rng.choice(legal_moves)
