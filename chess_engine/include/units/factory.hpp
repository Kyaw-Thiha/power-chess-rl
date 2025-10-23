#pragma once
#include "chess/piece.hpp"

#include <memory>

// Forward-declare Unit
namespace engine {
class Unit;
std::unique_ptr<Unit> make_unit_from_code(piece::Code code);
} // namespace engine
