#include "chess/config.hpp"
#include "chess/engine.hpp"
#include "chess/move.hpp"
#include "chess/piece.hpp"
#include "chess/state.hpp"
#include "units/unit.hpp"

#include <array>
#include <memory>
#include <vector>

namespace engine {

class Knight : public Unit {
public:
  using Unit::Unit; // inherit ctor

  std::vector<Move> get_legal_moves(const State &state, Square from) const override {
    std::vector<Move> moves;
    int row = Engine::row(from);
    int col = Engine::col(from);

    constexpr std::array<Vec2, 8> jumps = {{{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}}};

    for (Vec2 d : jumps) {
      int new_row = row + d.row;
      int new_col = col + d.col;

      // Skip if out of board
      if (new_row < 0 || new_row >= BOARD_N)
        continue;
      if (new_col < 0 || new_col >= BOARD_N)
        continue;

      Square new_pos = Engine::get_pos(new_row, new_col);
      const piece::Code tgt = state.board[new_pos];

      bool is_empty = piece::is_empty(tgt);
      bool is_enemy = (owner_ == 0) ? piece::is_p2(tgt) : piece::is_p1(tgt);

      if (is_empty || is_enemy) {
        moves.push_back(Move{from, new_pos});
      }
    }

    return moves;
  }

  char symbol() const override {
    return owner_ == 0 ? 'k' : 'K';
  }

  std::unique_ptr<Unit> clone() const override {
    return std::make_unique<Knight>(*this);
  }
};

} // namespace engine
