cmake_minimum_required(VERSION 3.10)

project(electra LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*.cpp")

add_executable(electra ${SOURCES})

target_include_directories(electra PRIVATE include)

target_compile_options(electra PRIVATE 
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wshadow
    -Wsign-conversion
    -Wnull-dereference
    -Wunused
)
