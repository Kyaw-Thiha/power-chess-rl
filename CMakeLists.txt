cmake_minimum_required(VERSION 3.20)
project(power-chess-rl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Setting up testing
include(CTest)        

# Defer to the chess_engine/ subproject (keeps engine encapsulated)
add_subdirectory(chess_engine)
