#include "chess/config.hpp"
#include "chess/engine.hpp"
#include "chess/move.hpp"
#include "chess/piece.hpp"
#include "chess/state.hpp"
#include "unit.cpp"

#include <array>
#include <memory>
#include <vector>

namespace engine {

class King : public Unit {
public:
  using Unit::Unit; // inherit ctor

  std::vector<Move> get_legal_moves(const State &state, Square from) const override {
    std::vector<Move> moves;
    int row = Engine::row(from);
    int col = Engine::col(from);

    constexpr std::array<Vec2, 8> directions = {{{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

    for (Vec2 direction : directions) {
      int new_row = row + direction.row;
      int new_col = col + direction.col;

      // Skip move if the move ends up out of the board
      if (new_row < 0 || new_row >= BOARD_N) {
        continue;
      }
      if (new_col < 0 || new_col >= BOARD_N) {
        continue;
      }

      Square new_pos = Engine::get_pos(new_row, new_col);
      const piece::Code new_pos_piece = state.board[new_pos];

      // Check if new position is empty or is enemy
      bool is_empty = piece::is_empty(new_pos_piece);
      bool is_enemy = false;
      if (owner_ == 0) {
        is_enemy = piece::is_p2(new_pos_piece);
      } else {
        is_enemy = piece::is_p1(new_pos_piece);
      }

      if (is_empty || is_enemy) {
        moves.push_back(Move{from, new_pos});
      }
    }

    return moves;
  }

  char symbol() const override {
    return owner_ == 0 ? 'b' : 'B';
  }
  std::unique_ptr<Unit> clone() const override {
    return std::make_unique<King>(*this);
  }
};

} // namespace engine
