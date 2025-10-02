cmake_minimum_required(VERSION 3.10)

project(ArtCele LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(ArtCele
    src/main.cpp
    src/lexer.cpp
)

target_compile_options(ArtCele PRIVATE 
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion          # implicit narrowing conversions
    -Wshadow              # variable shadowing
    -Wsign-conversion     # signed/unsigned mismatches
    -Wnull-dereference    # null pointer dereference
    -Wunused              # unused vars/functions
)
