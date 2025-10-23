cmake_minimum_required(VERSION 3.20)

# Use modern C++ and pybind11
include(FetchContent)
FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/tags/v2.12.0.tar.gz
)
FetchContent_MakeAvailable(pybind11)

# Library target: engine core
add_library(chess_engine_core
  src/engine.cpp
)
target_include_directories(chess_engine_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(chess_engine_core PUBLIC cxx_std_17)
target_compile_options(chess_engine_core PRIVATE -Wall -Wextra -Wpedantic)

# Python module target (pybind11)
pybind11_add_module(_ccore MODULE
  src/bindings.cpp
)
target_link_libraries(_ccore PRIVATE chess_engine_core)

# Install the module into the Python package path (scikit-build-core handles wheel)
install(TARGETS _ccore
  LIBRARY DESTINATION custom_chess/engine
)
