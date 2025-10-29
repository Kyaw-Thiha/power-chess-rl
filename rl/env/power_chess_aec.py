from __future__ import annotations

from typing import Dict, Optional, Set

import numpy as np
from gymnasium import spaces
from gymnasium.utils import seeding
from pettingzoo.utils import agent_selector
from pettingzoo.utils.env import AECEnv
from pettingzoo.utils.wrappers import OrderEnforcingWrapper

from power_chess.engine import BOARD_N, Engine, State
from .action_mapper import DiscreteActionMapper
from .observation import empty_observation, format_observation, observation_space

PLAYER_AGENT_NAMES: tuple[str, str] = ("player_0", "player_1")
DEFAULT_MAX_ACTIONS = 4096


def make_aec_env(*, max_actions: int = DEFAULT_MAX_ACTIONS) -> AECEnv:
    """Return an order-enforced PettingZoo AEC environment."""
    base_env = PowerChessAECEnv(max_actions=max_actions)
    return OrderEnforcingWrapper(base_env)


class PowerChessAECEnv(AECEnv):
    """Two-player PettingZoo environment backed by the C++ power-chess engine."""

    metadata = {"name": "power_chess_aec_v0", "is_parallelizable": False, "render_modes": ["ansi"]}

    def __init__(self, *, max_actions: int = DEFAULT_MAX_ACTIONS) -> None:
        super().__init__()
        self._engine = Engine()
        self._state: Optional[State] = None
        self._max_actions = max_actions
        self._action_mapper = DiscreteActionMapper(max_actions=max_actions)
        self.np_random, self._last_seed = seeding.np_random(None)

        self.possible_agents = list(PLAYER_AGENT_NAMES)
        self.agents: list[str] = []
        self._agent_selector = agent_selector(self.possible_agents)

        self.action_spaces: Dict[str, spaces.Space] = {
            agent: spaces.Discrete(self._action_mapper.size) for agent in self.possible_agents
        }
        obs_space = observation_space(self._action_mapper.size)
        self.observation_spaces: Dict[str, spaces.Space] = {agent: obs_space for agent in self.possible_agents}

        self._legal_actions: Dict[str, Set[int]] = {agent: set() for agent in self.possible_agents}

    # --------------------------------------------------------------------- API
    def reset(self, *, seed: Optional[int] = None, options: Optional[dict] = None) -> None:  # type: ignore[override]
        """Reset the environment to the initial game state."""
        self._seed(seed)
        self.agents = self.possible_agents[:]
        self._state = self._engine.initial_state()

        self.rewards = {agent: 0.0 for agent in self.agents}
        self._cumulative_rewards = {agent: 0.0 for agent in self.agents}
        self.terminations = {agent: False for agent in self.agents}
        self.truncations = {agent: False for agent in self.agents}
        self.infos = {agent: {} for agent in self.agents}
        self._legal_actions = {agent: set() for agent in self.possible_agents}

        self._agent_selector = agent_selector(self.agents)
        self.agent_selection = self._agent_selector.next()
        self._sync_legal_actions()
        self.has_reset = True

    def action_space(self, agent: str) -> spaces.Space:
        """Return the discrete action space for a given agent."""
        if agent not in self.action_spaces:
            raise ValueError(f"Unknown agent '{agent}'.")
        return self.action_spaces[agent]

    def observation_space(self, agent: str) -> spaces.Space:
        """Return the observation space for a given agent."""
        if agent not in self.observation_spaces:
            raise ValueError(f"Unknown agent '{agent}'.")
        return self.observation_spaces[agent]

    def observe(self, agent: str) -> dict[str, np.ndarray]:
        """Return the observation dictionary for the requested agent."""
        if agent not in self.possible_agents:
            raise ValueError(f"Unknown agent '{agent}'.")

        if self._state is None or agent not in self.agents:
            return empty_observation(self._action_mapper.size)

        legal_ids = self._legal_actions.get(agent, set())
        return format_observation(self._state, legal_ids, self._action_mapper.size)

    def step(self, action: int) -> None:
        """Apply the selected action for the current agent."""
        agent = self.agent_selection

        if self.terminations.get(agent, False) or self.truncations.get(agent, False):
            self._was_dead_step(action)
            return

        legal_for_agent = self._legal_actions.get(agent, set())
        if action not in legal_for_agent:
            raise ValueError(f"Action {action} is illegal for agent '{agent}'.")

        if self._state is None:
            raise RuntimeError("Environment state is uninitialised.")
        move = self._action_mapper.build_move(action)
        step_result = self._engine.apply_move(self._state, move)
        self._state = step_result.state

        reward_p0 = float(step_result.reward_p0)
        self.rewards["player_0"] = reward_p0
        self.rewards["player_1"] = -reward_p0

        for agent_name in self.agents:
            self._cumulative_rewards[agent_name] = 0.0
        self._accumulate_rewards()

        if step_result.done:
            for agent_name in self.agents:
                self.terminations[agent_name] = True
            self._legal_actions = {agent_name: set() for agent_name in self.possible_agents}
            self.agents = []
        else:
            self.agent_selection = self._agent_selector.next()
            self._sync_legal_actions()

    def render(self) -> str:
        """Render the board as an ASCII string."""
        if self._state is None:
            return "<terminated>"
        board = self._state.board
        rows = []
        for r in range(BOARD_N):
            row_values = board[r * BOARD_N : (r + 1) * BOARD_N]
            rows.append(" ".join(f"{value:02d}" for value in row_values))
        return "\n".join(rows)

    # ----------------------------------------------------------------- Helpers
    def _sync_legal_actions(self) -> None:
        if self._state is None or not self.agents:
            return
        current_agent = self.agent_selection
        moves = self._engine.legal_moves(self._state)
        action_ids = self._action_mapper.register_moves(moves)
        self._legal_actions = {agent: set() for agent in self.possible_agents}
        self._legal_actions[current_agent] = set(action_ids)

    def _accumulate_rewards(self) -> None:
        for agent in self.agents:
            self._cumulative_rewards[agent] += self.rewards[agent]

    def _seed(self, seed: Optional[int]) -> None:
        self.np_random, self._last_seed = seeding.np_random(seed)
