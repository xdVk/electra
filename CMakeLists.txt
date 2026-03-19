cmake_minimum_required(VERSION 3.20)
project(electra VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(electra
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/sema.cpp
    src/ir.cpp
    src/codegen.cpp
)

target_include_directories(electra PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(electra PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Werror)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(electra PRIVATE -fsanitize=address,undefined)
    target_link_options(electra PRIVATE -fsanitize=address,undefined)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -march=native -flto")
