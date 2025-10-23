#pragma once
#include "chess/config.hpp"
#include "chess/move.hpp"
#include "chess/state.hpp"

#include <memory>
#include <vector>

namespace engine {

class Unit {
public:
  explicit Unit(int owner) : owner_{owner} {}
  virtual ~Unit() = default;

  // Pure virtual: every piece implements this.
  virtual std::vector<Move> get_legal_moves(const State &state, Square from) const = 0;

  // Optional helpers
  virtual char symbol() const = 0;
  virtual std::unique_ptr<Unit> clone() const = 0;

protected:
  int owner_ = 0; // 0=P1, 1=P2
};

} // namespace engine
