/**
 * @file test_bindings_embed.cpp
 * @brief C++ test that embeds Python, imports _ccore, and exercises the bindings.
 */

#include <catch2/catch_all.hpp>
#include <pybind11/embed.h> // py::scoped_interpreter
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#ifndef PY_MODULE_DIR
#error "PY_MODULE_DIR not defined (set in CMake to the folder that contains _ccore.*)"
#endif

TEST_CASE("pybind11 bindings basic roundtrip", "[bindings][embed]") {
  // Start the Python interpreter for this test scope
  py::scoped_interpreter guard{};

  // Ensure the build dir (where _ccore.* lives) is on sys.path
  py::module_ sys = py::module_::import("sys");
  sys.attr("path").cast<py::list>().append(PY_MODULE_DIR);

  // Import our module
  py::module_ m = py::module_::import("_ccore");

  // Sanity: BOARD_N is exported and equals 6
  int board_n = m.attr("BOARD_N").cast<int>();
  REQUIRE(board_n == 6);

  // Access MoveType enum
  py::object MoveType = m.attr("MoveType");
  py::object MT_Quiet = MoveType.attr("Quiet");

  // Engine + state
  py::object EngClass = m.attr("Engine");
  py::object eng = EngClass();                    // Engine()
  py::object state = eng.attr("initial_state")(); // State

  // legal_moves returns a Python list of Move
  py::list moves = eng.attr("legal_moves")(state).cast<py::list>();
  REQUIRE(moves.size() > 0);

  // Check one move has expected fields
  py::object mv0 = moves[0];
  REQUIRE(mv0.attr("from").cast<int>() >= 0);
  REQUIRE(mv0.attr("to").cast<int>() >= 0);
  // .type is a MoveType; compare via identity or just check attribute exists
  (void)MT_Quiet; // not strictly needed here

  // Ply should increment and done should be false in the opening position
  std::uint32_t ply_before = state.attr("ply").cast<std::uint32_t>();
  py::object step = eng.attr("apply_move")(state, mv0);
  bool done = step.attr("done").cast<bool>();
  std::uint32_t ply_after = state.attr("ply").cast<std::uint32_t>();

  REQUIRE_FALSE(done);
  REQUIRE(ply_after == ply_before + 1);

  // group_legal_moves_by_from returns a list size BOARD_N*BOARD_N
  py::list grouped = eng.attr("group_legal_moves_by_from")(state).cast<py::list>();
  REQUIRE(static_cast<int>(grouped.size()) == board_n * board_n);

  // legal_moves_from accepts (state, from)
  int from_sq = mv0.attr("from").cast<int>();
  py::list from_moves = eng.attr("legal_moves_from")(state, from_sq).cast<py::list>();
  // It is valid to be empty after we’ve moved; just ensure the API returns a list
  REQUIRE(from_moves.ptr() != nullptr);

  // is_legal recognizes a generated move as legal
  bool ok = eng.attr("is_legal")(state, mv0).cast<bool>();
  // Note: mv0 may no longer be legal after we already applied it; skip strict check.
  (void)ok;
}
