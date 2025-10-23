/**
 * @file test_engine.cpp
 * @brief Engine-level tests: initial state, move aggregation, legality, apply_move.
 */

#include "chess/config.hpp"
#include "chess/engine.hpp"
#include "chess/move.hpp"
#include "chess/piece.hpp"
#include "chess/state.hpp"

#include <catch2/catch_all.hpp>

using namespace engine;

TEST_CASE("Initial state layout is correct (6x6 mirror)", "[engine][initial]") {
  REQUIRE(BOARD_N == 6);

  Engine E;
  State s = E.initial_state();

  // Top back row (P2): R B N K B R
  REQUIRE(piece::unit_type(s.board[E.get_pos(0, 0)]) == piece::ROOK);
  REQUIRE(piece::unit_type(s.board[E.get_pos(0, 1)]) == piece::BISHOP);
  REQUIRE(piece::unit_type(s.board[E.get_pos(0, 2)]) == piece::KNIGHT);
  REQUIRE(piece::unit_type(s.board[E.get_pos(0, 3)]) == piece::KING);
  REQUIRE(piece::unit_type(s.board[E.get_pos(0, 4)]) == piece::BISHOP);
  REQUIRE(piece::unit_type(s.board[E.get_pos(0, 5)]) == piece::ROOK);
  for (int c = 0; c < BOARD_N; ++c) {
    REQUIRE(piece::unit_type(s.board[E.get_pos(1, c)]) == piece::PAWN);
    REQUIRE(piece::is_p2(s.board[E.get_pos(1, c)]));
  }

  // Bottom back row (P1): R B N K B R
  REQUIRE(piece::unit_type(s.board[E.get_pos(5, 0)]) == piece::ROOK);
  REQUIRE(piece::unit_type(s.board[E.get_pos(5, 1)]) == piece::BISHOP);
  REQUIRE(piece::unit_type(s.board[E.get_pos(5, 2)]) == piece::KNIGHT);
  REQUIRE(piece::unit_type(s.board[E.get_pos(5, 3)]) == piece::KING);
  REQUIRE(piece::unit_type(s.board[E.get_pos(5, 4)]) == piece::BISHOP);
  REQUIRE(piece::unit_type(s.board[E.get_pos(5, 5)]) == piece::ROOK);
  for (int c = 0; c < BOARD_N; ++c) {
    REQUIRE(piece::unit_type(s.board[E.get_pos(4, c)]) == piece::PAWN);
    REQUIRE(piece::is_p1(s.board[E.get_pos(4, c)]));
  }

  REQUIRE(s.to_move == 0);
  REQUIRE(s.ply == 0);
}

TEST_CASE("legal_moves aggregates per-square moves", "[engine][moves]") {
  Engine E;
  State s = E.initial_state();

  // P1 to move at start; collect all legal moves.
  auto all_moves = E.legal_moves(s);
  REQUIRE_FALSE(all_moves.empty());

  // Pick a P1 pawn (e.g., row 4, col 2) and compare with legal_moves_from.
  const int row = 4, col = 2;
  const Square from = E.get_pos(row, col);
  REQUIRE(piece::unit_type(s.board[from]) == piece::PAWN);
  REQUIRE(piece::is_p1(s.board[from]));

  auto from_moves = E.legal_moves_from(s, from);
  REQUIRE_FALSE(from_moves.empty());

  // Grouped view should contain exactly the same set for this 'from' square.
  auto grouped = E.group_legal_moves_by_from(s);
  REQUIRE(grouped[from].size() == from_moves.size());
}

TEST_CASE("is_legal matches moves produced by legal_moves", "[engine][is_legal]") {
  Engine E;
  State s = E.initial_state();

  auto all_moves = E.legal_moves(s);
  REQUIRE_FALSE(all_moves.empty());

  // Every generated move must be recognized as legal.
  for (const auto &m : all_moves) {
    REQUIRE(E.is_legal(s, m));
  }

  // A junk move should be illegal.
  Move bogus{};
  bogus.from = 0;
  bogus.to = 0;
  bogus.type = MoveType::Quiet;
  REQUIRE_FALSE(E.is_legal(s, bogus));
}

TEST_CASE("apply_move updates ply and switches side", "[engine][apply]") {
  Engine E;
  State s = E.initial_state();

  const auto all_moves_before = E.legal_moves(s);
  REQUIRE_FALSE(all_moves_before.empty());

  const std::uint32_t old_ply = s.ply;
  const std::uint32_t old_to_move = s.to_move;

  // Apply the first legal move.
  const Move m = all_moves_before.front();
  auto step = E.apply_move(s, m);

  REQUIRE(s.ply == old_ply + 1);
  REQUIRE(s.to_move == 1 - old_to_move);
  // Not terminal at game start.
  REQUIRE(step.done == false);
}
