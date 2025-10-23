#pragma once
/**
 * @file engine.hpp
 * @brief Stateless rule engine operating on cc::State.
 */

#include "types.hpp"
#include <vector>

namespace cc {

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
  StepResult step(State &s, const Move &m) const;

  /// Helpers for row/col/index conversions.
  static inline int r(Square sq) { return static_cast<int>(sq) / BOARD_N; }
  static inline int c(Square sq) { return static_cast<int>(sq) % BOARD_N; }
  static inline Square sq(int r, int c) {
    return static_cast<Square>(r * BOARD_N + c);
  }
};

} // namespace cc
