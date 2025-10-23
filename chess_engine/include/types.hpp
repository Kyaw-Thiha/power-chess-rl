#pragma once
/**
 * @file types.hpp
 * @brief Fundamental types and compact piece encoding for the chess-like engine.
 */

#include <array>
#include <cstdint>
#include <string>
#include <vector>

// ============================================================================
// Compact piece encoding (1 byte)
// ============================================================================
namespace piece {

/// Compact type used to represent a single piece on the board.
using Code = std::uint8_t;

// ---------------------------------------------------------------------------
// Masks & shifts
// ---------------------------------------------------------------------------

/// Mask for the lower 3 bits (unit type).
constexpr Code KIND_MASK = 0b0000'0111; // bits 0..2

/// Mask for the has_moved flag (bit 3).
constexpr Code MOVED_MASK = 0b0000'1000; // bit 3

/// Mask for the side bit (bit 4) — 0=P1, 1=P2.
constexpr Code SIDE_MASK = 0b0001'0000; // bit 4

/// Mask for the 3-bit power ID (bits 5..7).
constexpr Code POWER_MASK = 0b1110'0000; // bits 5..7

/// Right-shift to bring power bits (5..7) down to (0..2).
constexpr int POWER_SHIFT = 5;

// ---------------------------------------------------------------------------
// Unit types (0 reserved for EMPTY → 7 non-empty kinds available)
// ---------------------------------------------------------------------------

/**
 * @enum UnitType
 * @brief Unit type (3 bits). 0 is EMPTY; 1..7 are real kinds.
 */
enum UnitType : Code {
  EMPTY = 0,
  PAWN = 1, // previously "MAN"
  KNIGHT = 2,
  BISHOP = 3,
  ROOK = 4,
  QUEEN = 5,
  KING = 6,
  // Can extend up to 7 (inclusive)
};

/**
 * @enum Side
 * @brief Player identifiers. Encoded at bit 4.
 */
enum Side : Code {
  P1 = 0,         ///< Player 1 (bit 4 = 0)
  P2 = SIDE_MASK, ///< Player 2 (bit 4 = 1)
};

/**
 * @enum Power
 * @brief Power-up identifier (3 bits). 0 means NONE; 1..7 are variants.
 */
enum Power : Code {
  POWER_NONE = 0, ///< No power
  POWER_1 = 1,
  POWER_2 = 2,
  POWER_3 = 3,
  POWER_4 = 4,
  POWER_5 = 5,
  POWER_6 = 6,
  POWER_7 = 7,
};

// ---------------------------------------------------------------------------
// Construction & decoding
// ---------------------------------------------------------------------------

/**
 * @brief Construct a piece code from fields.
 *
 * @param t        Unit type (0..7; 0=EMPTY)
 * @param s        Side (P1 or P2)
 * @param hasMoved Whether the unit has moved at least once
 * @param pwr      Power ID (0..7; 0=POWER_NONE)
 * @return Encoded 8-bit piece::Code value.
 */
constexpr Code make(UnitType t, Side s, bool hasMoved = false, Power pwr = POWER_NONE) {
  const Code kind_bits = static_cast<Code>(t & KIND_MASK);
  const Code moved_bits = static_cast<Code>(hasMoved ? MOVED_MASK : 0);
  const Code side_bits = static_cast<Code>(s & SIDE_MASK);
  const Code power_bits = static_cast<Code>((static_cast<Code>(pwr) & 0x07) << POWER_SHIFT);
  return static_cast<Code>(kind_bits | moved_bits | side_bits | power_bits);
}

/// Extract the unit type (0..7).
constexpr UnitType unit_type(Code c) {
  return static_cast<UnitType>(c & KIND_MASK);
}

/// Extract the side (P1/P2).
constexpr Side side(Code c) {
  return static_cast<Side>(c & SIDE_MASK);
}

/// Extract the has_moved flag.
constexpr bool has_moved(Code c) {
  return (c & MOVED_MASK) != 0;
}

/// Extract the power ID (0..7).
constexpr Power power(Code c) {
  return static_cast<Power>((c & POWER_MASK) >> POWER_SHIFT);
}

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------

/// True if the square is empty.
constexpr bool is_empty(Code c) {
  return unit_type(c) == EMPTY;
}

/// True if the piece belongs to Player 1.
constexpr bool is_p1(Code c) {
  return (c & SIDE_MASK) == 0;
}

/// True if the piece belongs to Player 2.
constexpr bool is_p2(Code c) {
  return (c & SIDE_MASK) != 0;
}

// ---------------------------------------------------------------------------
// Mutators (return modified copies)
// ---------------------------------------------------------------------------

/// Return a copy with has_moved flag set.
constexpr Code set_has_moved(Code c) {
  return static_cast<Code>(c | MOVED_MASK);
}

/// Return a copy with has_moved flag cleared.
constexpr Code clear_has_moved(Code c) {
  return static_cast<Code>(c & ~MOVED_MASK);
}

/// Return a copy with side set to P1.
constexpr Code as_p1(Code c) {
  return static_cast<Code>(c & ~SIDE_MASK);
}

/// Return a copy with side set to P2.
constexpr Code as_p2(Code c) {
  return static_cast<Code>(c | SIDE_MASK);
}

/// Return a copy with power set to the given ID (0..7).
constexpr Code with_power(Code c, Power pwr) {
  const Code cleared = static_cast<Code>(c & ~POWER_MASK);
  const Code power_bits = static_cast<Code>((static_cast<Code>(pwr) & 0x07) << POWER_SHIFT);
  return static_cast<Code>(cleared | power_bits);
}

// Optional: small compile-time sanity checks (won’t execute at runtime)
static_assert((KIND_MASK & MOVED_MASK) == 0, "unit_type and has_moved overlap");
static_assert((KIND_MASK & SIDE_MASK) == 0, "unit_type and side overlap");
static_assert((KIND_MASK & POWER_MASK) == 0, "unit_type and power overlap");
static_assert((MOVED_MASK & SIDE_MASK) == 0, "has_moved and side overlap");
static_assert((MOVED_MASK & POWER_MASK) == 0, "has_moved and power overlap");
static_assert((SIDE_MASK & POWER_MASK) == 0, "side and power overlap");

} // namespace piece

// ============================================================================
// Engine-facing primitive types
// ============================================================================
namespace engine {

constexpr int BOARD_N = 8;   ///< Board dimension (8x8).
using Square = std::uint8_t; ///< Encodes a 0..(BOARD_N*BOARD_N-1) square index.
using Player = std::uint8_t; ///< 0 (P1) or 1 (P2).

/**
 * @enum MoveType
 * @brief Move kind (promotion and special include payloads).
 */
enum class MoveType : uint8_t {
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
  piece::Code promo_piece = 0; ///< For promotions: fully-encoded target piece code.
  uint16_t special_code = 0;   ///< Optional payload for “Special” moves.
};

/** @brief Step result after applying a move. */
struct StepResult {
  bool done;        ///< True if terminal.
  int reward_p0;    ///< Reward from player-0's perspective in {-1,0,1}.
  std::string info; ///< Optional info string (debug, reason).
};

/** @brief Entire game state (compact, POD). */
struct State {
  std::array<piece::Code, BOARD_N * BOARD_N> board{}; ///< Encoded piece per square.
  Player to_move = 0;                                 ///< Side to move (0 or 1).
  std::uint32_t ply = 0;                              ///< Half-move count.
};

/** @brief 2D Vector for representing things like direction */
struct Vec2 {
  int row;
  int col;
};

} // namespace engine
