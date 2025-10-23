#pragma once
#include "chess/config.hpp"
#include "chess/move.hpp"
#include "chess/state.hpp"

#include <memory>
#include <vector>

namespace engine {

/**
 * @brief Base class for all chess-like units (pieces).
 * Provides polymorphic move generation.
 */
class Unit {
public:
  explicit Unit(Player owner) : owner_(owner) {}
  virtual ~Unit() = default;

  /// @return Which player owns this unit (0 = P1, 1 = P2).
  /**
   * @brief Gets the owner of the unit
   * @return Which player owns this unit (0 = P1, 1 = P2).
   */
  Player owner() const {
    return owner_;
  }

  /**
   * @brief Generate all legal moves for this unit from the given square.
   */
  virtual std::vector<Move> get_legal_moves(const State &state, Square from) const = 0;

  /**
   * @brief Symbolic representation (for rendering / debugging).
   */
  virtual char symbol() const = 0;

  /**
   * @brief Polymorphic clone
   */
  virtual std::unique_ptr<Unit> clone() const = 0;

protected:
  Player owner_ = 0; // 0=P1, 1=P2
};

} // namespace engine
