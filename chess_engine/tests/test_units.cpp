/**
 * @file test_units.cpp
 * @brief Piece-specific tests: Bishop, Rook, Queen, Knight, King.
 *
 * Notes:
 * - We place one P1 king and one P2 king so Engine::apply_move() does not mark the state as terminal.
 * - We construct tiny positions to verify sliding, blocking, capture rules, and jump behavior.
 */

#include "chess/config.hpp"
#include "chess/engine.hpp"
#include "chess/move.hpp"
#include "chess/piece.hpp"
#include "chess/state.hpp"

#include <catch2/catch_all.hpp>

using namespace engine;

// Helpers to create pieces quickly
static piece::Code p1(piece::UnitType t, bool moved = false) {
  return piece::make(t, piece::P1, moved, piece::POWER_NONE);
}
static piece::Code p2(piece::UnitType t, bool moved = false) {
  return piece::make(t, piece::P2, moved, piece::POWER_NONE);
}

// Place kings in safe corners so tests are non-terminal.
static void place_default_kings(Engine &E, State &s) {
  s.board[E.get_pos(0, 0)] = p2(piece::KING, false);
  s.board[E.get_pos(BOARD_N - 1, BOARD_N - 1)] = p1(piece::KING, false);
}

// Utility: returns true if there exists a move (from,to) with optional type match.
static bool has_move(const std::vector<Move> &moves, Square from, Square to, std::optional<MoveType> type = std::nullopt) {
  for (const auto &m : moves) {
    if (m.from == from && m.to == to) {
      if (!type.has_value() || m.type == *type)
        return true;
    }
  }
  return false;
}

// ------------------------------ Bishop ------------------------------
TEST_CASE("Bishop slides diagonally, stops at blockers, can capture enemy", "[unit][bishop]") {
  Engine E;
  State s{};
  s.board.fill(piece::EMPTY);
  s.to_move = 0; // P1
  s.ply = 0;

  place_default_kings(E, s);

  // Put a P1 bishop at center-ish square (2,2) on 6x6
  const Square from = E.get_pos(2, 2);
  s.board[from] = p1(piece::BISHOP);

  // Own blocker on (1,1), enemy on (3,3)
  const Square own_block = E.get_pos(1, 1);
  const Square enemy_diag = E.get_pos(3, 3);
  s.board[own_block] = p1(piece::PAWN);
  s.board[enemy_diag] = p2(piece::PAWN);

  const auto moves = E.legal_moves_from(s, from);

  // Can capture enemy on (3,3), but cannot continue past it
  REQUIRE(has_move(moves, from, enemy_diag));
  const Square past_enemy = E.get_pos(4, 4);
  REQUIRE_FALSE(has_move(moves, from, past_enemy));

  // Cannot move onto own piece at (1,1), and cannot go beyond it to (0,0)
  REQUIRE_FALSE(has_move(moves, from, own_block));
  const Square beyond_own = E.get_pos(0, 0);
  REQUIRE_FALSE(has_move(moves, from, beyond_own));

  // Still should be able to slide to other open diagonals e.g., (1,3), (0,4), (3,1), (4,0)
  REQUIRE(has_move(moves, from, E.get_pos(1, 3)));
  REQUIRE(has_move(moves, from, E.get_pos(0, 4)));
  REQUIRE(has_move(moves, from, E.get_pos(3, 1)));
  REQUIRE(has_move(moves, from, E.get_pos(4, 0)));
}

// ------------------------------ Rook ------------------------------
TEST_CASE("Rook slides straight, stops at blockers, can capture enemy", "[unit][rook]") {
  Engine E;
  State s{};
  s.board.fill(piece::EMPTY);
  s.to_move = 0; // P1
  s.ply = 0;

  place_default_kings(E, s);

  const Square from = E.get_pos(2, 2);
  s.board[from] = p1(piece::ROOK);

  // Blockers: own at (2,4); enemy at (0,2)
  const Square own_block = E.get_pos(2, 4);
  const Square enemy_line = E.get_pos(0, 2);
  s.board[own_block] = p1(piece::PAWN);
  s.board[enemy_line] = p2(piece::PAWN);

  const auto moves = E.legal_moves_from(s, from);

  // Horizontal towards +col: should reach (2,3), but not (2,4) or beyond
  REQUIRE(has_move(moves, from, E.get_pos(2, 3)));
  REQUIRE_FALSE(has_move(moves, from, own_block));
  REQUIRE_FALSE(has_move(moves, from, E.get_pos(2, 5)));

  // Vertical towards -row: should include capture at (0,2), but not beyond (cannot jump)
  REQUIRE(has_move(moves, from, E.get_pos(1, 2)));
  REQUIRE(has_move(moves, from, enemy_line));
  REQUIRE_FALSE(has_move(moves, from, E.get_pos(-1, 2))); // guard (out of board, always false)
}

// ------------------------------ Queen ------------------------------
TEST_CASE("Queen combines rook and bishop movement", "[unit][queen]") {
  Engine E;
  State s{};
  s.board.fill(piece::EMPTY);
  s.to_move = 0; // P1
  s.ply = 0;

  place_default_kings(E, s);

  const Square from = E.get_pos(3, 3);
  s.board[from] = p1(piece::QUEEN);

  // Place enemy to capture diagonally and a friendly blocker on a file
  const Square enemy_diag = E.get_pos(1, 1);
  const Square own_file_block = E.get_pos(3, 5);
  s.board[enemy_diag] = p2(piece::KNIGHT);
  s.board[own_file_block] = p1(piece::PAWN);

  const auto moves = E.legal_moves_from(s, from);

  // Rook-like moves: can go (3,2) and (3,4) but not onto (3,5) nor beyond
  REQUIRE(has_move(moves, from, E.get_pos(3, 2)));
  REQUIRE(has_move(moves, from, E.get_pos(3, 4)));
  REQUIRE_FALSE(has_move(moves, from, own_file_block));
  REQUIRE_FALSE(has_move(moves, from, E.get_pos(3, 6))); // out of board on 6x6, implicitly false

  // Bishop-like moves: capture at (1,1) but not beyond
  REQUIRE(has_move(moves, from, enemy_diag));
  REQUIRE_FALSE(has_move(moves, from, E.get_pos(0, 0)));
}

// ------------------------------ Knight ------------------------------
TEST_CASE("Knight jumps over blockers and can capture enemy", "[unit][knight]") {
  Engine E;
  State s{};
  s.board.fill(piece::EMPTY);
  s.to_move = 0; // P1
  s.ply = 0;

  place_default_kings(E, s);

  const Square from = E.get_pos(2, 2);
  s.board[from] = p1(piece::KNIGHT);

  // Drop a ring of blocking pieces around the knight; it should ignore them.
  // Also place an enemy at a valid knight target (4,3).
  s.board[E.get_pos(2, 1)] = p1(piece::PAWN);
  s.board[E.get_pos(2, 3)] = p1(piece::PAWN);
  s.board[E.get_pos(1, 2)] = p1(piece::PAWN);
  s.board[E.get_pos(3, 2)] = p1(piece::PAWN);
  s.board[E.get_pos(1, 1)] = p1(piece::PAWN);
  s.board[E.get_pos(1, 3)] = p1(piece::PAWN);
  s.board[E.get_pos(3, 1)] = p1(piece::PAWN);
  s.board[E.get_pos(3, 3)] = p1(piece::PAWN);

  const Square enemy = E.get_pos(4, 3);
  s.board[enemy] = p2(piece::BISHOP);

  const auto moves = E.legal_moves_from(s, from);

  // Knight targets from (2,2) on 6x6:
  // (0,1), (0,3), (1,0), (1,4), (3,0), (3,4), (4,1), (4,3)
  REQUIRE(has_move(moves, from, E.get_pos(0, 1)));
  REQUIRE(has_move(moves, from, E.get_pos(0, 3)));
  REQUIRE(has_move(moves, from, E.get_pos(1, 0)));
  REQUIRE(has_move(moves, from, E.get_pos(1, 4)));
  REQUIRE(has_move(moves, from, E.get_pos(3, 0)));
  REQUIRE(has_move(moves, from, E.get_pos(3, 4)));
  REQUIRE(has_move(moves, from, E.get_pos(4, 1)));
  REQUIRE(has_move(moves, from, enemy)); // capture
}

// ------------------------------ King ------------------------------
TEST_CASE("King moves 1 step in 8 directions and can capture adjacent enemy", "[unit][king]") {
  Engine E;
  State s{};
  s.board.fill(piece::EMPTY);
  s.to_move = 0; // P1
  s.ply = 0;

  // Kings: place P1 king as the unit under test, and a P2 king far away
  const Square p1k_sq = E.get_pos(2, 2);
  s.board[p1k_sq] = p1(piece::KING);
  s.board[E.get_pos(0, 0)] = p2(piece::KING);

  // Surround some squares with own and enemy pieces
  const Square enemy_adj = E.get_pos(1, 1);
  const Square own_adj = E.get_pos(1, 2);
  s.board[enemy_adj] = p2(piece::PAWN);
  s.board[own_adj] = p1(piece::PAWN);

  const auto moves = E.legal_moves_from(s, p1k_sq);

  // Can move to empty neighbors
  REQUIRE(has_move(moves, p1k_sq, E.get_pos(1, 3)));
  REQUIRE(has_move(moves, p1k_sq, E.get_pos(3, 1)));
  REQUIRE(has_move(moves, p1k_sq, E.get_pos(3, 3)));

  // Can capture enemy adjacent
  REQUIRE(has_move(moves, p1k_sq, enemy_adj));

  // Cannot move onto own piece
  REQUIRE_FALSE(has_move(moves, p1k_sq, own_adj));
}
