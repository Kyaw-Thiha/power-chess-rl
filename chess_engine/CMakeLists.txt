cmake_minimum_required(VERSION 3.20)

# ── Dependencies ────────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/tags/v2.12.0.tar.gz
)
FetchContent_MakeAvailable(pybind11)

# ── Engine core ────────────────────────────────────────────────────────────────
file(GLOB CHESS_UNIT_SOURCES
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/units/*.cpp
)

add_library(chess_engine_core
  src/engine.cpp
  ${CHESS_UNIT_SOURCES}
)
set_target_properties(chess_engine_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(chess_engine_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(chess_engine_core PUBLIC cxx_std_17)
target_compile_options(chess_engine_core PRIVATE -Wall -Wextra -Wpedantic)

# ── Python module ──────────────────────────────────────────────────────────────
pybind11_add_module(_ccore MODULE src/bindings.cpp)
target_link_libraries(_ccore PRIVATE chess_engine_core)
install(TARGETS _ccore LIBRARY DESTINATION custom_chess/engine)

# ── Tests (delegated) ─────────────────────────────────────────────────────────
include(CTest)
if (BUILD_TESTING)
  add_subdirectory(tests)
endif()
