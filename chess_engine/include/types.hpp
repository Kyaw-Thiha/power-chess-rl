#pragma once
/**
 * @file types.hpp
 * @brief Fundamental types for the 5x5 chess-like engine.
 */

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace cc {

constexpr int BOARD_N = 5;   ///< Board dimension (5x5).
using Square = std::uint8_t; ///< Encodes a 0..24 square index.
using Player = std::uint8_t; ///< 0 (P1) or 1 (P2).

/** @brief Piece identifiers. Extend with power-ups as needed. */
enum Piece : std::uint8_t {
  EMPTY = 0,
  P1_MAN = 1,
  P1_KING = 2,
  P2_MAN = 3,
  P2_KING = 4,
};

/** @brief A single move from one square to another. */
struct Move {
  Square from; ///< Source square index 0..24.
  Square to;   ///< Destination square index 0..24.
};

/** @brief Step result after applying a move. */
struct StepResult {
  bool done;        ///< True if terminal.
  int reward_p0;    ///< Reward from player-0's perspective in {-1,0,1}.
  std::string info; ///< Optional info string (debug, reason).
};

/** @brief Entire game state (compact, POD). */
struct State {
  std::array<std::uint8_t, BOARD_N * BOARD_N>
      board{};           ///< Piece codes by square.
  Player to_move = 0;    ///< Side to move (0 or 1).
  std::uint32_t ply = 0; ///< Half-move count.
};

} // namespace cc
