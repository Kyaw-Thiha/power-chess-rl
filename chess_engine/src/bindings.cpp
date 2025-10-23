#include "chess/engine.hpp"
#include "chess/move.hpp"
#include "chess/state.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace engine;

/**
 * @brief pybind11 module exposing the C++ engine:
 *  - enums: MoveType
 *  - classes: Move, StepResult, State, Engine
 *  - Engine methods: initial_state(), legal_moves(), legal_moves_from(), group_legal_moves_by_from(),
 *                    is_legal(), apply_move()
 *  - Engine static helpers: get_pos(), row(), col()
 *  - constant: BOARD_N
 */
PYBIND11_MODULE(_ccore, m) {
  m.doc() = "Custom 6x6 power-chess engine (C++ core)";

  // Export board size constant for convenience
  m.attr("BOARD_N") = BOARD_N;

  // ---- Enums
  py::enum_<MoveType>(m, "MoveType", R"pbdoc(
    Move kinds:
      - Quiet
      - Capture
      - Promote
      - CapturePromote
      - Special
  )pbdoc")
      .value("Quiet", MoveType::Quiet)
      .value("Capture", MoveType::Capture)
      .value("Promote", MoveType::Promote)
      .value("CapturePromote", MoveType::CapturePromote)
      .value("Special", MoveType::Special)
      .export_values();

  // ---- PODs
  py::class_<Move>(m, "Move", R"pbdoc(A move from one square to another.)pbdoc")
      .def(py::init<>())
      .def_readwrite("from", &Move::from, R"pbdoc(Source square index.)pbdoc")
      .def_readwrite("to", &Move::to, R"pbdoc(Destination square index.)pbdoc")
      .def_readwrite("type", &Move::type, R"pbdoc(MoveType.)pbdoc")
      .def_readwrite("promo_piece", &Move::promo_piece, R"pbdoc(Encoded piece::Code for promotions.)pbdoc")
      .def_readwrite("special_code", &Move::special_code, R"pbdoc(16-bit payload for special moves.)pbdoc");

  py::class_<StepResult>(m, "StepResult", R"pbdoc(Result of applying a move.)pbdoc")
      .def(py::init<>())
      .def_readwrite("done", &StepResult::done, R"pbdoc(True if terminal.)pbdoc")
      .def_readwrite("reward_p0", &StepResult::reward_p0, R"pbdoc(Reward from player-0's perspective.)pbdoc")
      .def_readwrite("info", &StepResult::info, R"pbdoc(Optional info/debug string.)pbdoc");

  py::class_<State>(m, "State", R"pbdoc(Complete game state.)pbdoc")
      .def(py::init<>())
      .def_readwrite("board", &State::board, R"pbdoc(Flat array of length BOARD_N*BOARD_N with piece codes.)pbdoc")
      .def_readwrite("to_move", &State::to_move, R"pbdoc(Player to move: 0 or 1.)pbdoc")
      .def_readwrite("ply", &State::ply, R"pbdoc(Half-move count.)pbdoc");

  // ---- Engine
  py::class_<Engine>(m, "Engine", R"pbdoc(Stateless rule engine.)pbdoc")
      .def(py::init<>())

      .def("initial_state", &Engine::initial_state, R"pbdoc(Return a fresh initial state.)pbdoc")

      .def("legal_moves", &Engine::legal_moves, py::arg("state"), R"pbdoc(Return all legal moves for the side to move.)pbdoc")

      .def("legal_moves_from", &Engine::legal_moves_from, py::arg("state"), py::arg("from"),
           R"pbdoc(Return legal moves originating from a specific square.)pbdoc")

      .def("group_legal_moves_by_from", &Engine::group_legal_moves_by_from, py::arg("state"),
           R"pbdoc(Return a list (size BOARD_N*BOARD_N) of move lists, indexed by 'from' square.)pbdoc")

      .def("is_legal", &Engine::is_legal, py::arg("state"), py::arg("move"),
           R"pbdoc(Check if a move is legal in the given state.)pbdoc")

      .def("apply_move", &Engine::apply_move, py::arg("state"), py::arg("move"),
           R"pbdoc(Apply move to state in-place; returns StepResult.)pbdoc")

      .def_static("get_pos", &Engine::get_pos, py::arg("row"), py::arg("col"),
                  R"pbdoc(Convert (row, col) to flat square index.)pbdoc")
      .def_static("row", &Engine::row, py::arg("idx"), R"pbdoc(Row from flat square index.)pbdoc")
      .def_static("col", &Engine::col, py::arg("idx"), R"pbdoc(Col from flat square index.)pbdoc");
}
