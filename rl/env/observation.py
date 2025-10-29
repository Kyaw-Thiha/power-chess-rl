from __future__ import annotations

from typing import Iterable

import numpy as np
from gymnasium import spaces

from power_chess.engine import BOARD_N, State

BOARD_AREA = BOARD_N * BOARD_N


def board_as_tensor(state: State) -> np.ndarray:
    """Convert the engine state board into a (BOARD_N, BOARD_N) tensor."""
    board_array = np.asarray(state.board, dtype=np.uint8)
    return board_array.reshape((BOARD_N, BOARD_N))


def action_mask_from_ids(max_actions: int, legal_action_ids: Iterable[int]) -> np.ndarray:
    """Construct a binary action mask with ones at legal action ids."""
    mask = np.zeros((max_actions,), dtype=np.int8)
    for action_id in legal_action_ids:
        if 0 <= action_id < max_actions:
            mask[action_id] = 1
    return mask


def observation_space(max_actions: int) -> spaces.Dict:
    """Return the observation space shared across agents."""
    return spaces.Dict(
        {
            "observation": spaces.Box(low=0, high=255, shape=(BOARD_N, BOARD_N), dtype=np.uint8),
            "action_mask": spaces.Box(low=0, high=1, shape=(max_actions,), dtype=np.int8),
        }
    )


def format_observation(state: State, legal_action_ids: Iterable[int], max_actions: int) -> dict[str, np.ndarray]:
    """Build the observation dictionary consumed by RLlib policies."""
    return {
        "observation": board_as_tensor(state),
        "action_mask": action_mask_from_ids(max_actions, legal_action_ids),
    }


def empty_observation(max_actions: int) -> dict[str, np.ndarray]:
    """Return an observation structure filled with zeros."""
    return {
        "observation": np.zeros((BOARD_N, BOARD_N), dtype=np.uint8),
        "action_mask": np.zeros((max_actions,), dtype=np.int8),
    }
