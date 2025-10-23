#include "engine.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace engine;

/**
 * @brief pybind11 module exposing the C++ engine:
 *  - classes: Move, State, Engine
 *  - Engine methods: initial_state(), legal_moves(), step()
 */
PYBIND11_MODULE(_ccore, m) {
  m.doc() = "Custom 5x5 chess-like engine (C++ core)";

  py::class_<Move>(m, "Move", R"pbdoc(A move from one square to another.)pbdoc")
      .def(py::init<>())
      .def_readwrite("from", &Move::from, R"pbdoc(Source square 0..24.)pbdoc")
      .def_readwrite("to", &Move::to, R"pbdoc(Destination square 0..24.)pbdoc");

  py::class_<State>(m, "State", R"pbdoc(Complete game state.)pbdoc")
      .def(py::init<>())
      .def_readwrite("board", &State::board, R"pbdoc(Flat piece array length 25.)pbdoc")
      .def_readwrite("to_move", &State::to_move, R"pbdoc(Player to move: 0 or 1.)pbdoc")
      .def_readwrite("ply", &State::ply, R"pbdoc(Half-move count.)pbdoc");

  py::class_<Engine>(m, "Engine", R"pbdoc(Stateless rule engine.)pbdoc")
      .def(py::init<>())
      .def("initial_state", &Engine::initial_state, R"pbdoc(Return a fresh initial state.)pbdoc")
      .def("legal_moves", &Engine::legal_moves, py::arg("state"),
           R"pbdoc(Return a vector of legal moves in the given state.)pbdoc")
      .def("step", &Engine::apply_move, py::arg("state"), py::arg("move"),
           R"pbdoc(Apply move to state in-place; return StepResult-like tuple.)pbdoc");
}
