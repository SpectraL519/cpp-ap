# Structure
INCLUDE_DIR := ../include
EXAMPLE_SRC := ./main.cpp
EXAMPLE_EXEC := example


# Compiler & flags
CXX := g++
CXX_STD := -std=c++2a
CXX_FLAGS += $(CXX_STD) -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3
COMPILE_ARGS := -I $(INCLUDE_DIR) $(CXX_FLAGS)


all:
	$(CXX) -o $(EXAMPLE_EXEC) $(EXAMPLE_SRC) $(COMPILE_ARGS)

clean:
	del $(EXAMPLE_EXEC).exe
