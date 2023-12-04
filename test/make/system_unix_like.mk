# Structure
INCLUDE_DIR := ../include
TEST_INCLUDE_DIR := ./include
TEST_DIR := .
TEST_SRC_DIR := tests
TEST_BUILD_DIR := build
TEST_EXEC := test

# specific test case
ifeq ($(firstword $(MAKECMDGOALS)), specific)
	TEST_FILE := $(word 2, $(MAKECMDGOALS))
	MAKECMDGOALS := $(filter-out specific $(TEST_FILE),$(MAKECMDGOALS))
endif

# Compiler & flags
CXX := g++
CXX_STD := -std=c++2a
CXX_FLAGS += $(CXX_STD) -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3
COMPILE_ARGS := -I $(INCLUDE_DIR) -I $(TEST_INCLUDE_DIR) $(CXX_FLAGS)

# Test source & object files
TEST_SOURCES := $(wildcard $(TEST_SRC_DIR)/*.cpp)
TEST_OBJECTS := $(patsubst $(TEST_SRC_DIR)/%.cpp, $(TEST_BUILD_DIR)/%.o, $(TEST_SOURCES))

TEST_SOURCES += $(TEST_DIR)/main.cpp
TEST_OBJECTS += $(TEST_BUILD_DIR)/main.o


all: $(TEST_EXEC)
$(TEST_BUILD_DIR)/main.o: main.cpp
	@echo Compiling: $<
	@$(CXX) -c $< -o $@ $(COMPILE_ARGS)

$(TEST_BUILD_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	@echo Compiling: $<
	@$(CXX) -c $< -o $@ $(COMPILE_ARGS)

$(TEST_EXEC): $(TEST_OBJECTS)
	@echo Linking: $@
	@$(CXX) $^ -o $@ $(COMPILE_ARGS)
	@echo Build successful!


.PHONY: specific clean

specific:
	@if [ -f "$(TEST_SRC_DIR)/$(TEST_FILE).cpp" ]; then \
		echo "Building test: $(TEST_FILE)"; \
		echo "Compiling: $(TEST_DIR)/main.cpp"; \
		$(CXX) -c "$(TEST_DIR)/main.cpp" -o "$(TEST_BUILD_DIR)/main.o" $(COMPILE_ARGS); \
		echo "Compiling: $(TEST_SRC_DIR)/$(TEST_FILE).cpp"; \
		$(CXX) -c "$(TEST_SRC_DIR)/$(TEST_FILE).cpp" -o "$(TEST_BUILD_DIR)/$(TEST_FILE).o" $(COMPILE_ARGS); \
		echo "Linking..."; \
		$(CXX) $(COMPILE_ARGS) "$(TEST_BUILD_DIR)/main.o" "$(TEST_BUILD_DIR)/$(TEST_FILE).o" -o "$(TEST_FILE)"; \
		echo "Build successful!"; \
	else \
		echo "Test not found: $(TEST_FILE)"; \
	fi


clean:
	rm -f $(TEST_BUILD_DIR)/*.o
	find . -type f -executable -not -iname "*.*" -not -iname "Makefile" -delete
