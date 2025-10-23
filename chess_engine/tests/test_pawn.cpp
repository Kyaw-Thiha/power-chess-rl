/**
 * @file test_pawn.cpp
 * @brief Pawn-specific tests: forward, double-step, promotion (quiet and capture).
 */

#include "chess/config.hpp"
#include "chess/engine.hpp"
#include "chess/move.hpp"
#include "chess/piece.hpp"
#include "chess/state.hpp"

#include <catch2/catch_all.hpp>

using namespace engine;

static piece::Code p1(piece::UnitType t, bool moved = false) {
  return piece::make(t, piece::P1, moved, piece::POWER_NONE);
}
static piece::Code p2(piece::UnitType t, bool moved = false) {
  return piece::make(t, piece::P2, moved, piece::POWER_NONE);
}

TEST_CASE("P1 pawn initial: forward 1 and double-step available if path clear", "[pawn][push]") {
  Engine E;
  State s = E.initial_state();

  // Choose a center pawn for P1 (row 4, col 2)
  const int from_row = 4, from_col = 2;
  const Square from = E.get_pos(from_row, from_col);
  REQUIRE(piece::unit_type(s.board[from]) == piece::PAWN);
  REQUIRE(piece::is_p1(s.board[from]));

  auto moves = E.legal_moves_from(s, from);
  REQUIRE_FALSE(moves.empty());

  // Expect at least a Quiet push to (3,2)
  const Square quiet_to = E.get_pos(3, 2);
  bool has_quiet = false, has_double = false;
  for (const auto &m : moves) {
    if (m.to == quiet_to && m.type == MoveType::Quiet)
      has_quiet = true;
    if (m.type == MoveType::Quiet && E.row(m.to) == 2 && E.col(m.to) == 2)
      has_double = true;
  }
  REQUIRE(has_quiet);
  REQUIRE(has_double);
}

TEST_CASE("P1 pawn promotion on quiet push", "[pawn][promote][quiet]") {
  Engine E;
  State s{};
  s.board.fill(piece::EMPTY);
  s.to_move = 0;
  s.ply = 0;

  // Place a single P1 pawn at (1, 3); pushing to (0,3) should promote.
  const int from_row = 1, from_col = 3;
  const Square from = E.get_pos(from_row, from_col);
  s.board[from] = p1(piece::PAWN, /*moved=*/true); // already moved is fine for promotion test

  // Destination must be empty.
  const Square to = E.get_pos(0, 3);
  s.board[to] = piece::EMPTY;

  auto moves = E.legal_moves_from(s, from);
  bool has_promote = false;
  Move promote_move{};
  for (const auto &m : moves) {
    if (m.to == to && m.type == MoveType::Promote) {
      has_promote = true;
      promote_move = m;
      break;
    }
  }
  REQUIRE(has_promote);
  REQUIRE(piece::unit_type(promote_move.promo_piece) == piece::QUEEN);
  REQUIRE(piece::is_p1(promote_move.promo_piece));

  // Apply and verify board state.
  auto step = E.apply_move(s, promote_move);
  REQUIRE(piece::unit_type(s.board[to]) == piece::QUEEN);
  REQUIRE(piece::is_p1(s.board[to]));
  REQUIRE(piece::has_moved(s.board[to])); // promoted piece marked moved
  REQUIRE(piece::is_empty(s.board[from]));
  REQUIRE_FALSE(step.done); // not terminal in this setup
}

TEST_CASE("P1 pawn promotion on capture", "[pawn][promote][capture]") {
  Engine E;
  State s{};
  s.board.fill(piece::EMPTY);
  s.to_move = 0;
  s.ply = 0;

  // P1 pawn at (1, 3), P2 piece diagonally at (0, 4)
  const Square from = E.get_pos(1, 3);
  const Square cap_to = E.get_pos(0, 4);

  s.board[from] = p1(piece::PAWN, /*moved=*/true);
  s.board[cap_to] = p2(piece::KNIGHT, /*moved=*/false);

  auto moves = E.legal_moves_from(s, from);
  bool has_cap_promote = false;
  Move cap_promote_move{};
  for (const auto &m : moves) {
    if (m.to == cap_to && m.type == MoveType::CapturePromote) {
      has_cap_promote = true;
      cap_promote_move = m;
      break;
    }
  }
  REQUIRE(has_cap_promote);
  REQUIRE(piece::unit_type(cap_promote_move.promo_piece) == piece::QUEEN);
  REQUIRE(piece::is_p1(cap_promote_move.promo_piece));

  // Apply and verify board state.
  auto step = E.apply_move(s, cap_promote_move);
  REQUIRE(piece::unit_type(s.board[cap_to]) == piece::QUEEN);
  REQUIRE(piece::is_p1(s.board[cap_to]));
  REQUIRE(piece::is_empty(s.board[from]));
  REQUIRE_FALSE(step.done);
}
