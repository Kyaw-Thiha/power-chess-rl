#include "units/factory.hpp"

#include "bishop.cpp"
#include "king.cpp"
#include "knight.cpp"
#include "pawn.cpp"
#include "queen.cpp"
#include "rook.cpp"
#include "units/unit.hpp"

namespace engine {

std::unique_ptr<Unit> make_unit_from_code(piece::Code code) {
  if (piece::is_empty(code))
    return nullptr;

  const int owner = piece::is_p1(code) ? 0 : 1;
  switch (piece::unit_type(code)) {
  case piece::KING:
    return std::make_unique<King>(owner);
  case piece::QUEEN:
    return std::make_unique<Queen>(owner);
  case piece::ROOK:
    return std::make_unique<Rook>(owner);
  case piece::BISHOP:
    return std::make_unique<Bishop>(owner);
  case piece::KNIGHT:
    return std::make_unique<Knight>(owner);
  case piece::PAWN:
    return std::make_unique<Pawn>(owner);
  default:
    return nullptr;
  }
}

} // namespace engine
