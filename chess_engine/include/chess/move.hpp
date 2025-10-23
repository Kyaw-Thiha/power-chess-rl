#pragma once
/**
 * @file move.hpp
 * @brief Move types and step results.
 */

#include "chess/config.hpp"
#include "chess/piece.hpp"

#include <cstdint>
#include <string>

namespace engine {

/** @brief Move kind (promotion and special include payloads). */
enum class MoveType : std::uint8_t {
  Quiet = 0,          // normal move to empty square
  Capture = 1,        // move that captures an enemy
  Promote = 2,        // promotion without capture
  CapturePromote = 3, // capture + promotion
  Special = 4         // castling, power-up effects, etc. via payload
};

/** @brief A single move from one square to another. */
struct Move {
  Square from; ///< Source square index.
  Square to;   ///< Destination square index.
  MoveType type = MoveType::Quiet;
  piece::Code promo_piece = 0;    ///< For promotions: fully-encoded target piece code.
  std::uint16_t special_code = 0; ///< Optional payload for “Special” moves.
};

/** @brief Step result after applying a move. */
struct StepResult {
  bool done = false; ///< True if terminal.
  int reward_p0 = 0; ///< Reward from player-0's perspective in {-1,0,1}.
  std::string info;  ///< Optional info string (debug, reason).
};

} // namespace engine
