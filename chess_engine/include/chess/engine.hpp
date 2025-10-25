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

  /** @brief get legal moves of specific unit in from */
  std::vector<Move> legal_moves_from(const State &s, Square from) const;

  // Optional helper: grouped by source square
  std::array<std::vector<Move>, BOARD_N * BOARD_N> group_legal_moves_by_from(const State &s) const;

  /** @Helper to deduce the move type based on from-to Move */
  static inline MoveType deduce_move_type(const State &s, const Move &m) {
    const bool is_capture = !piece::is_empty(s.board[m.to]) && ((piece::is_p1(s.board[m.from]) && piece::is_p2(s.board[m.to])) ||
                                                                (piece::is_p2(s.board[m.from]) && piece::is_p1(s.board[m.to])));

    // Promotion if mover is a pawn that lands on last rank
    const piece::Code mover = s.board[m.from];
    const bool is_pawn = piece::unit_type(mover) == piece::PAWN;
    const int to_row = Engine::row(m.to);
    const bool promotes = is_pawn ? (piece::is_p1(mover) ? (to_row == 0) : (to_row == BOARD_N - 1)) : false;

    if (m.special_code != 0)
      return MoveType::Special;
    if (promotes && is_capture)
      return MoveType::CapturePromote;
    if (promotes && !is_capture)
      return MoveType::Promote;
    if (is_capture)
      return MoveType::Capture;
    return MoveType::Quiet;
  }

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
