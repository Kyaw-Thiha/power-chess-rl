#include "chess/engine.hpp"

#include "chess/move.hpp"
#include "chess/state.hpp"
#include "units/factory.hpp"
#include "units/unit.hpp"

#include <algorithm>

namespace engine {

/**
 * @brief Set the initial state for the chess board.
 *
 * Pawn | Pawn | Pawn | Pawn | Pawn | Pawn
 * Rook | Bishop | Knight | King | Bishop | Rook
 */
State Engine::initial_state() const {
  State s;
  s.board.fill(piece::EMPTY);

  // Helper lambdas
  auto P1 = [](piece::UnitType t) { return piece::make(t, piece::P1, /*hasMoved=*/false, piece::POWER_NONE); };
  auto P2 = [](piece::UnitType t) { return piece::make(t, piece::P2, /*hasMoved=*/false, piece::POWER_NONE); };

  // Row indices
  const int ROW_TOP_BACK = 0;
  const int ROW_TOP_PAWN = 1;
  const int ROW_BOT_PAWN = BOARD_N - 2; // 4
  const int ROW_BOT_BACK = BOARD_N - 1; // 5

  s.board[get_pos(ROW_TOP_BACK, 0)] = P2(piece::ROOK);
  s.board[get_pos(ROW_TOP_BACK, 1)] = P2(piece::BISHOP);
  s.board[get_pos(ROW_TOP_BACK, 2)] = P2(piece::KNIGHT);
  s.board[get_pos(ROW_TOP_BACK, 3)] = P2(piece::KING);
  s.board[get_pos(ROW_TOP_BACK, 4)] = P2(piece::BISHOP);
  s.board[get_pos(ROW_TOP_BACK, 5)] = P2(piece::ROOK);

  for (int c = 0; c < BOARD_N; ++c) {
    s.board[get_pos(ROW_TOP_PAWN, c)] = P2(piece::PAWN);
    s.board[get_pos(ROW_BOT_PAWN, c)] = P1(piece::PAWN);
  }

  s.board[get_pos(ROW_BOT_BACK, 0)] = P1(piece::ROOK);
  s.board[get_pos(ROW_BOT_BACK, 1)] = P1(piece::BISHOP);
  s.board[get_pos(ROW_BOT_BACK, 2)] = P1(piece::KNIGHT);
  s.board[get_pos(ROW_BOT_BACK, 3)] = P1(piece::KING);
  s.board[get_pos(ROW_BOT_BACK, 4)] = P1(piece::BISHOP);
  s.board[get_pos(ROW_BOT_BACK, 5)] = P1(piece::ROOK);

  s.to_move = 0; // P1 starts
  s.ply = 0;
  return s;
}

std::vector<Move> Engine::legal_moves_from(const State &s, Square from) const {
  std::vector<Move> out;
  if (from < 0 || from >= BOARD_N * BOARD_N)
    return out;

  const piece::Code pc = s.board[from];
  if (piece::is_empty(pc))
    return out;

  const Player side = s.to_move;
  const bool belongs_to_side = (side == 0) ? piece::is_p1(pc) : piece::is_p2(pc);
  if (!belongs_to_side)
    return out;

  auto unit = make_unit_from_code(pc);
  if (!unit)
    return out;

  out = unit->get_legal_moves(s, from);

  // (Optional) If your unit code produces pseudo-legal moves, you could
  // filter here (e.g., remove moves that leave king in check).
  return out;
}

std::vector<Move> Engine::legal_moves(const State &s) const {
  std::vector<Move> moves;
  moves.reserve(64); // small pre-reserve

  for (int idx = 0; idx < BOARD_N * BOARD_N; ++idx) {
    const piece::Code pc = s.board[idx];
    if (piece::is_empty(pc))
      continue;

    const bool is_ours = (s.to_move == 0) ? piece::is_p1(pc) : piece::is_p2(pc);
    if (!is_ours)
      continue;

    auto unit = make_unit_from_code(pc);
    if (!unit)
      continue;

    auto v = unit->get_legal_moves(s, idx);
    moves.insert(moves.end(), v.begin(), v.end());
  }
  return moves;
}

std::array<std::vector<Move>, BOARD_N * BOARD_N> Engine::group_legal_moves_by_from(const State &s) const {
  std::array<std::vector<Move>, BOARD_N * BOARD_N> buckets;
  for (int idx = 0; idx < BOARD_N * BOARD_N; ++idx) {
    buckets[idx] = legal_moves_from(s, idx);
  }
  return buckets;
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
