# Tests CMake for chess_engine

include(FetchContent)
FetchContent_Declare(
  Catch2
  URL https://github.com/catchorg/Catch2/archive/refs/tags/v3.5.3.tar.gz
)
FetchContent_MakeAvailable(Catch2)

# Gather all test sources, exclude embedded one
file(GLOB CHESS_TEST_SOURCES
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
list(REMOVE_ITEM CHESS_TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/test_bindings.cpp
)

# C++ core tests
add_executable(chess_engine_tests ${CHESS_TEST_SOURCES})
target_include_directories(chess_engine_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(chess_engine_tests PRIVATE chess_engine_core Catch2::Catch2WithMain)
target_compile_features(chess_engine_tests PRIVATE cxx_std_17)
target_compile_options(chess_engine_tests PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME chess_core_tests COMMAND chess_engine_tests)

# Embedded-Python bindings test
add_executable(chess_engine_tests_embed test_bindings.cpp)
target_include_directories(chess_engine_tests_embed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(chess_engine_tests_embed PRIVATE Catch2::Catch2WithMain pybind11::embed)
target_compile_features(chess_engine_tests_embed PRIVATE cxx_std_17)
target_compile_options(chess_engine_tests_embed PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(chess_engine_tests_embed PRIVATE
  PY_MODULE_DIR="${CMAKE_CURRENT_BINARY_DIR}/.."
)
add_test(NAME chess_bindings_embed_tests COMMAND chess_engine_tests_embed)
