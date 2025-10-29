from __future__ import annotations

import numpy as np
import pytest

from rl.env import make_aec_env


@pytest.fixture()
def env():
    environment = make_aec_env()
    try:
        environment.reset()
        yield environment
    finally:
        environment.close()


def test_initial_action_mask_matches_legal_moves(env):
    current_agent = env.agent_selection
    observation = env.observe(current_agent)
    mask = observation["action_mask"]
    assert mask.dtype == np.int8

    legal_ids = env.unwrapped._legal_actions[current_agent]
    assert mask.sum() == len(legal_ids)
    active_indices = set(np.nonzero(mask)[0].tolist())
    assert active_indices == legal_ids


def test_illegal_action_raises(env):
    current_agent = env.agent_selection
    legal_ids = env.unwrapped._legal_actions[current_agent]
    full_range = range(env.action_space(current_agent).n)
    illegal_candidate = next(idx for idx in full_range if idx not in legal_ids)
    with pytest.raises(ValueError):
        env.step(illegal_candidate)


def test_step_switches_active_agent(env):
    current_agent = env.agent_selection
    legal_ids = env.unwrapped._legal_actions[current_agent]
    action = next(iter(legal_ids))
    env.step(action)

    assert env.agent_selection != current_agent
    if env.agents:
        next_observation = env.observe(env.agent_selection)
        assert next_observation["action_mask"].sum() > 0
