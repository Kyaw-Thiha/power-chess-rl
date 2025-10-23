#pragma once
#include "types.hpp"

#include <memory>
#include <vector>

namespace cc {

/**
 * @brief Base class for all chess-like units (pieces).
 * Provides polymorphic move generation and move application.
 */
class Unit {
public:
  explicit Unit(Player owner) : owner_(owner) {}
  virtual ~Unit() = default;

  /// @return Which player owns this unit (0 = P1, 1 = P2).
  Player owner() const {
    return owner_;
  }

  /// Generate all legal moves for this unit from the given square.
  virtual std::vector<Move> get_legal_moves(const State &s, Square from) const = 0;

  /// Apply a move involving this unit. Override for special moves (e.g., promotion).
  virtual void move(State &s, const Move &m) const;

  /// Symbolic representation (for rendering / debugging).
  virtual char symbol() const = 0;

  /// Polymorphic clone (needed if you store pieces by pointer).
  virtual std::unique_ptr<Unit> clone() const = 0;

protected:
  Player owner_;
};

} // namespace cc
