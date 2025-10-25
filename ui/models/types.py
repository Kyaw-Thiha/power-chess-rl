from __future__ import annotations
from dataclasses import dataclass
from typing import Optional, Literal

Side = Literal[0, 1]  # 0: player-0, 1: player-1


@dataclass(slots=True)
class AgentConfig:
    """Configuration for an AI policy and optional checkpoint path."""

    name: str
    checkpoint_path: Optional[str] = None


@dataclass(slots=True)
class ReplayEntry:
    """Replay descriptor for loading from disk."""

    path: str
