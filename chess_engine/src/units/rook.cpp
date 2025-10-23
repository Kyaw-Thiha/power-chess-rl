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

class Rook : public Unit {
public:
  using Unit::Unit; // inherit ctor

  std::vector<Move> get_legal_moves(const State &state, Square from) const override {
    std::vector<Move> moves;
    int row = Engine::row(from);
    int col = Engine::col(from);

    constexpr std::array<Vec2, 4> directions = {{{1, 0}, {0, 1}, {-1, 0}, {0, -1}}};

    for (Vec2 direction : directions) {
      int new_row = row + direction.row;
      int new_col = col + direction.col;

      // Continue sliding in this direction until blocked
      while (new_row >= 0 && new_row < BOARD_N && new_col >= 0 && new_col < BOARD_N) {
        Square new_pos = Engine::get_pos(new_row, new_col);
        const piece::Code new_pos_piece = state.board[new_pos];

        bool is_empty = piece::is_empty(new_pos_piece);
        bool is_enemy = (owner_ == 0) ? piece::is_p2(new_pos_piece) : piece::is_p1(new_pos_piece);

        if (is_empty || is_enemy) {
          moves.push_back(Move{from, new_pos});
        }

        // Stop sliding if the path is blocked (enemy or own piece)
        if (!is_empty)
          break;

        new_row += direction.row;
        new_col += direction.col;
      }
    }

    return moves;
  }

  char symbol() const override {
    return owner_ == 0 ? 'r' : 'R';
  }

  std::unique_ptr<Unit> clone() const override {
    return std::make_unique<Rook>(*this);
  }
};

} // namespace engine
