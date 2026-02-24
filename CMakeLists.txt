cmake_minimum_required(VERSION 3.10)
project(electra VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES "src/*.cpp")

add_executable(electra ${SOURCES})

target_include_directories(electra PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_options(electra PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Werror)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -march=native -flto")
