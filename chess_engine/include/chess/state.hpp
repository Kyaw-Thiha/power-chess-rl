#pragma once
/**
 * @file state.hpp
 * @brief Game state POD for the engine.
 */

#include "chess/config.hpp"
#include "chess/piece.hpp"

#include <array>
#include <cstdint>

namespace engine {

/** @brief Entire game state (compact, POD). */
struct State {
  std::array<piece::Code, BOARD_N * BOARD_N> board{}; ///< Encoded piece per square.
  Player to_move = 0;                                 ///< Side to move (0 or 1).
  std::uint32_t ply = 0;                              ///< Half-move count.
};

} // namespace engine
