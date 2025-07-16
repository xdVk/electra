.SILENT:
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude/
SRC := $(wildcard src/*.cpp)
OUT := bin/rendE

all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS)

run: all
	./$(OUT)