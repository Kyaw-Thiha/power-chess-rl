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

class Pawn : public Unit {
public:
  using Unit::Unit; // inherit ctor

  std::vector<Move> get_legal_moves(const State &state, Square from) const override {
    std::vector<Move> moves;
    int row = Engine::row(from);
    int col = Engine::col(from);
    int dir = (owner_ == 0) ? -1 : +1;

    // Forward 1, forward 2, diagonal-right, diagonal-left (row is signed via dir)
    constexpr std::array<Vec2, 4> directions = {{{1, 0}, {2, 0}, {1, 1}, {1, -1}}};

    for (Vec2 direction : directions) {
      int new_row = row + dir * direction.row;
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

      // Handling forward movement
      if (direction.col == 0) {
        bool is_empty = piece::is_empty(new_pos_piece);
        if (is_empty) {
          // Check double-step preconditions
          if (direction.row == 2) {
            const piece::Code self_piece = state.board[from];
            bool has_moved = piece::has_moved(self_piece);
            if (has_moved) {
              continue;
            }
            // Intermediate square must also be empty
            Square mid_pos = Engine::get_pos(row + dir * 1, col);
            if (!piece::is_empty(state.board[mid_pos])) {
              continue;
            }
          }
          moves.push_back(Move{from, new_pos});
        }
      } else {
        // Handling diagonal attacks
        bool is_enemy = (owner_ == 0) ? piece::is_p2(new_pos_piece) : piece::is_p1(new_pos_piece);
        if (is_enemy) {
          moves.push_back(Move{from, new_pos});
        }
      }
    }

    return moves;
  }

  char symbol() const override {
    return owner_ == 0 ? 'p' : 'P';
  }
  std::unique_ptr<Unit> clone() const override {
    return std::make_unique<Pawn>(*this);
  }
};

} // namespace engine
