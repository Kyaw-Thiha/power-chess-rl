#include "engine.hpp"

#include "types.hpp"

#include <algorithm>

namespace engine {

static inline bool in_bounds(int r, int c) {
  return r >= 0 && r < BOARD_N && c >= 0 && c < BOARD_N;
}

State Engine::initial_state() const {
  State s;
  s.board.fill(piece::EMPTY);

  // Simple symmetric setup (top = P2, bottom = P1).
  // s.board[get_pos(0, 2)] = P2_KING;
  // s.board[get_pos(0, 1)] = P2_MAN;
  // s.board[get_pos(0, 3)] = P2_MAN;
  //
  // s.board[get_pos(4, 2)] = P1_KING;
  // s.board[get_pos(4, 1)] = P1_MAN;
  // s.board[get_pos(4, 3)] = P1_MAN;

  s.to_move = 0;
  s.ply = 0;
  return s;
}

std::vector<Move> Engine::legal_moves(const State &s) const {
  std::vector<Move> moves;
  const Player side = s.to_move;

  return moves;
}

bool Engine::is_legal(const State &s, const Move &m) const {
  for (const Move &lm : legal_moves(s)) {
    if (lm.from == m.from && lm.to == m.to && lm.type == m.type && lm.promo_piece == m.promo_piece &&
        lm.special_code == m.special_code) {
      return true;
    }
  }
  return false;
}

StepResult Engine::apply_move(State &s, const Move &m) const {
  const piece::Code moved = s.board[m.from];
  const piece::Code dest = s.board[m.to];
  (void)dest;

  // (Optional) fast assert if you didn’t mask actions in RL:
  // if (!is_legal(s, m)) { return StepResult{/*done=*/true, /*reward_p0=*/-1, "illegal"}; }

  switch (m.type) {
  case MoveType::Quiet: {
    s.board[m.to] = piece::set_has_moved(moved);
    s.board[m.from] = piece::make(piece::EMPTY, piece::P1); // EMPTY (side ignored)
    break;
  }
  case MoveType::Capture: {
    // Overwrite dest with mover
    s.board[m.to] = piece::set_has_moved(moved);
    s.board[m.from] = piece::make(piece::EMPTY, piece::P1);
    break;
  }
  case MoveType::Promote: {
    // Promotion target is a fully-encoded piece::Code in m.promo_piece.
    // We typically mark promotions as "has moved".
    s.board[m.to] = piece::set_has_moved(m.promo_piece);
    s.board[m.from] = piece::make(piece::EMPTY, piece::P1);
    break;
  }
  case MoveType::CapturePromote: {
    s.board[m.to] = piece::set_has_moved(m.promo_piece);
    s.board[m.from] = piece::make(piece::EMPTY, piece::P1);
    break;
  }
  case MoveType::Special: {
    // Interpret m.special_code, e.g., castling, power-up effects:
    // - Move rook, apply buffs, remove tiles, etc.
    // (Provide a small dispatcher or table for special codes.)
    s.board[m.to] = piece::set_has_moved(moved);
    s.board[m.from] = piece::make(piece::EMPTY, piece::P1);
    // then apply extra effects keyed by m.special_code
    break;
  }
  }

  s.ply += 1;
  s.to_move = 1 - s.to_move;

  // Terminal check (kings missing or ply cap)
  bool p1_king = false, p2_king = false;
  for (auto cell : s.board) {
    if (!piece::is_empty(cell) && piece::unit_type(cell) == piece::KING) {
      if (piece::is_p1(cell))
        p1_king = true;
      else
        p2_king = true;
    }
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
  return StepResult{done, reward_p0, std::string{}};
}

} // namespace engine
