#pragma once
/**
 * @file piece.hpp
 * @brief Fundamental 1-byte piece encoding and helpers.
 */

#include <cstdint>

namespace piece {

using Code = std::uint8_t;

// Masks & shifts
constexpr Code KIND_MASK = 0b0000'0111;  // bits 0..2
constexpr Code MOVED_MASK = 0b0000'1000; // bit 3
constexpr Code SIDE_MASK = 0b0001'0000;  // bit 4
constexpr Code POWER_MASK = 0b1110'0000; // bits 5..7
constexpr int POWER_SHIFT = 5;

// Unit types (0 = EMPTY)
enum UnitType : Code {
  EMPTY = 0,
  PAWN = 1,
  KNIGHT = 2,
  BISHOP = 3,
  ROOK = 4,
  QUEEN = 5,
  KING = 6,
  // up to 7
};

// Side (bit 4)
enum Side : Code {
  P1 = 0,         ///< bit 4 = 0
  P2 = SIDE_MASK, ///< bit 4 = 1
};

// Power (3 bits)
enum Power : Code {
  POWER_NONE = 0,
  POWER_1 = 1,
  POWER_2 = 2,
  POWER_3 = 3,
  POWER_4 = 4,
  POWER_5 = 5,
  POWER_6 = 6,
  POWER_7 = 7,
};

// Construction & decoding
constexpr Code make(UnitType t, Side s, bool hasMoved = false, Power pwr = POWER_NONE) {
  const Code kind_bits = static_cast<Code>(t & KIND_MASK);
  const Code moved_bits = static_cast<Code>(hasMoved ? MOVED_MASK : 0);
  const Code side_bits = static_cast<Code>(s & SIDE_MASK);
  const Code power_bits = static_cast<Code>((static_cast<Code>(pwr) & 0x07) << POWER_SHIFT);
  return static_cast<Code>(kind_bits | moved_bits | side_bits | power_bits);
}

constexpr UnitType unit_type(Code c) {
  return static_cast<UnitType>(c & KIND_MASK);
}
constexpr Side side(Code c) {
  return static_cast<Side>(c & SIDE_MASK);
}
constexpr bool has_moved(Code c) {
  return (c & MOVED_MASK) != 0;
}
constexpr Power power(Code c) {
  return static_cast<Power>((c & POWER_MASK) >> POWER_SHIFT);
}

// Queries
constexpr bool is_empty(Code c) {
  return unit_type(c) == EMPTY;
}

constexpr bool is_p1(Code c) {
  return !is_empty(c) && (c & SIDE_MASK) == 0;
}

constexpr bool is_p2(Code c) {
  return !is_empty(c) && (c & SIDE_MASK) != 0;
}

// Mutators (return modified copies)
constexpr Code set_has_moved(Code c) {
  return static_cast<Code>(c | MOVED_MASK);
}
constexpr Code clear_has_moved(Code c) {
  return static_cast<Code>(c & ~MOVED_MASK);
}
constexpr Code as_p1(Code c) {
  return static_cast<Code>(c & ~SIDE_MASK);
}
constexpr Code as_p2(Code c) {
  return static_cast<Code>(c | SIDE_MASK);
}
constexpr Code with_power(Code c, Power pwr) {
  const Code cleared = static_cast<Code>(c & ~POWER_MASK);
  const Code power_bits = static_cast<Code>((static_cast<Code>(pwr) & 0x07) << POWER_SHIFT);
  return static_cast<Code>(cleared | power_bits);
}

// Compile-time sanity checks
static_assert((KIND_MASK & MOVED_MASK) == 0, "unit_type and has_moved overlap");
static_assert((KIND_MASK & SIDE_MASK) == 0, "unit_type and side overlap");
static_assert((KIND_MASK & POWER_MASK) == 0, "unit_type and power overlap");
static_assert((MOVED_MASK & SIDE_MASK) == 0, "has_moved and side overlap");
static_assert((MOVED_MASK & POWER_MASK) == 0, "has_moved and power overlap");
static_assert((SIDE_MASK & POWER_MASK) == 0, "side and power overlap");

} // namespace piece
