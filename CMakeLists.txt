cmake_minimum_required(VERSION 3.10)

project(electra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(electra
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
)

target_compile_features(electra PRIVATE cxx_std_20)
target_compile_options(electra PRIVATE 
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
