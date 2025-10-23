#pragma once
/**
 * @file config.hpp
 * @brief Core engine constants and small primitive types.
 */

#include <cstdint>

namespace engine {

// Board configuration
constexpr int BOARD_N = 8; ///< Board dimension (8x8).

// Small POD primitives
using Square = std::uint8_t; ///< Encodes a 0..(BOARD_N*BOARD_N-1) square index.
using Player = std::uint8_t; ///< 0 (P1) or 1 (P2).

/** @brief 2D vector for grid math (rows, cols). */
struct Vec2 {
  int row;
  int col;
};

} // namespace engine
