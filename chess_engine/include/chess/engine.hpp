#pragma once
/**
 * @file engine.hpp
 * @brief Stateless rule engine operating on engine::State.
 */

#include "chess/move.hpp"
#include "chess/state.hpp"

#include <vector>

namespace engine {

/**
 * @brief Engine exposes rule queries (legal moves) and state transitions.
 * The engine is intentionally stateless; State is passed in/out explicitly.
 */
class Engine {
public:
  Engine() = default;

  /** @return A fresh initial position. */
  State initial_state() const;

  /**
   * @brief Compute pseudo-legal moves (captures overwrite; no check rules yet).
   * @param s Current state.
   * @return Vector of legal Move.
   */
  std::vector<Move> legal_moves(const State &s) const;

  /**
   * @brief Apply a move to the state in-place.
   * @param s Mutable state.
   * @param m Move to apply (must be legal).
   * @return StepResult containing termination and reward-from-P0.
   */
  StepResult apply_move(State &s, const Move &m) const;

  /** @brief Check if a move is legal under current rules. */
  bool is_legal(const State &s, const Move &m) const;

  // Helpers for index conversions could live here or in a small detail header.
  static inline int get_pos(int row, int col) {
    return row * BOARD_N + col;
  }
  static inline int row(int idx) {
    return idx / BOARD_N;
  }
  static inline int col(int idx) {
    return idx % BOARD_N;
  }
};

} // namespace engine
