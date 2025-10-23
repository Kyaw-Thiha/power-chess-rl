cmake_minimum_required(VERSION 3.20)

# Use modern C++ and pybind11
include(FetchContent)
FetchContent_Declare(
  pybind11
  URL https://github.com/pybind/pybind11/archive/refs/tags/v2.12.0.tar.gz
)
FetchContent_MakeAvailable(pybind11)

# Library target: engine core + units
file(GLOB CHESS_UNIT_SOURCES
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/units/*.cpp
)
add_library(chess_engine_core
  src/engine.cpp
  ${CHESS_UNIT_SOURCES}
)
# Build the core as PIC so it can link into the shared pybind module
set_target_properties(chess_engine_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

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

# ===== Tests =====
include(CTest)                # provides BUILD_TESTING option
if (BUILD_TESTING)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.3.tar.gz
  )
  FetchContent_MakeAvailable(Catch2)

  file(GLOB CHESS_TEST_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
  )
  # Exclude the embedded-Python test from the regular C++ test target
  list(REMOVE_ITEM CHESS_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_bindings.cpp
  )
  add_executable(chess_engine_tests
    ${CHESS_TEST_SOURCES}
  )
  target_include_directories(chess_engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(chess_engine_tests PRIVATE chess_engine_core Catch2::Catch2WithMain)
  target_compile_features(chess_engine_tests PRIVATE cxx_std_17)
  target_compile_options(chess_engine_tests PRIVATE -Wall -Wextra -Wpedantic)

  add_test(NAME chess_core_tests COMMAND chess_engine_tests)

  # ===== Embedded-Python bindings test (separate runner) =====
  add_executable(chess_engine_tests_embed
    tests/test_bindings.cpp
  )
  target_include_directories(chess_engine_tests_embed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(chess_engine_tests_embed PRIVATE Catch2::Catch2WithMain pybind11::embed)
  target_compile_features(chess_engine_tests_embed PRIVATE cxx_std_17)
  target_compile_options(chess_engine_tests_embed PRIVATE -Wall -Wextra -Wpedantic)

# Tell the embedded test where the compiled Python module lives:
# (this subdir's binary dir is .../build/chess_engine, which contains _ccore.*)
  target_compile_definitions(chess_engine_tests_embed PRIVATE
    PY_MODULE_DIR="${CMAKE_CURRENT_BINARY_DIR}"
  )

  add_test(NAME chess_bindings_embed_tests COMMAND chess_engine_tests_embed)

endif()

