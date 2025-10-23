#include "engine.hpp"
#include <algorithm>

namespace cc {

static inline bool in_bounds(int r, int c) {
  return r >= 0 && r < BOARD_N && c >= 0 && c < BOARD_N;
}

State Engine::initial_state() const {
  State s;
  s.board.fill(EMPTY);

  // Simple symmetric setup (top = P2, bottom = P1).
  s.board[sq(0, 2)] = P2_KING;
  s.board[sq(0, 1)] = P2_MAN;
  s.board[sq(0, 3)] = P2_MAN;

  s.board[sq(4, 2)] = P1_KING;
  s.board[sq(4, 1)] = P1_MAN;
  s.board[sq(4, 3)] = P1_MAN;

  s.to_move = 0;
  s.ply = 0;
  return s;
}

std::vector<Move> Engine::legal_moves(const State &s) const {
  std::vector<Move> moves;
  const Player side = s.to_move;

  for (int rr = 0; rr < BOARD_N; ++rr) {
    for (int cc_ = 0; cc_ < BOARD_N; ++cc_) {
      const Square from = sq(rr, cc_);
      const std::uint8_t p = s.board[from];
      if (p == EMPTY)
        continue;

      const bool is_p1 = (p == P1_MAN || p == P1_KING);
      const bool is_p2 = (p == P2_MAN || p == P2_KING);
      if ((side == 0 && !is_p1) || (side == 1 && !is_p2))
        continue;

      // Men move 1 forward (toward opponent) in 3 directions; Kings
      // 8-neighborhood.
      const int dir_fwd = (side == 0) ? -1 : 1;

      if (p == P1_MAN || p == P2_MAN) {
        const int nr = rr + dir_fwd;
        for (int dc : {-1, 0, 1}) {
          const int nc = cc_ + dc;
          if (!in_bounds(nr, nc))
            continue;
          const Square to = sq(nr, nc);
          const std::uint8_t q = s.board[to];
          const bool enemy = (side == 0) ? (q == P2_MAN || q == P2_KING)
                                         : (q == P1_MAN || q == P1_KING);
          if (q == EMPTY || enemy)
            moves.push_back(Move{from, to});
        }
      } else { // King
        static constexpr int dirs_k[8][2] = {{1, 0},  {-1, 0}, {0, 1},
                                             {0, -1}, {1, 1},  {1, -1},
                                             {-1, 1}, {-1, -1}};
        for (auto d : dirs_k) {
          const int nr = rr + d[0];
          const int nc = cc_ + d[1];
          if (!in_bounds(nr, nc))
            continue;
          const Square to = sq(nr, nc);
          const std::uint8_t q = s.board[to];
          const bool friendly = (side == 0) ? (q == P1_MAN || q == P1_KING)
                                            : (q == P2_MAN || q == P2_KING);
          if (!friendly)
            moves.push_back(Move{from, to});
        }
      }
    }
  }

  return moves;
}

StepResult Engine::step(State &s, const Move &m) const {
  const std::uint8_t moved = s.board[m.from];
  s.board[m.to] = moved;
  s.board[m.from] = EMPTY;
  s.ply += 1;

  // Terminal if a king is missing or move limit reached.
  bool p1_king = false, p2_king = false;
  for (const auto cell : s.board) {
    if (cell == P1_KING)
      p1_king = true;
    if (cell == P2_KING)
      p2_king = true;
  }

  const bool done = (!p1_king || !p2_king || s.ply >= 200);
  int reward_p0 = 0;
  if (done) {
    if (p1_king && !p2_king)
      reward_p0 = +1;
    else if (!p1_king && p2_king)
      reward_p0 = -1;
    else
      reward_p0 = 0;
  }

  s.to_move = 1 - s.to_move;
  return StepResult{done, reward_p0, std::string{}};
}

} // namespace cc
