# TODO: fix

# Structure
GLOB_DIR 	 := ..
TEST_DIR 	 := .
INC_DIR  	 := $(TEST_DIR)/include
INC_GLOB_DIR := $(GLOB_DIR)/include
SRC_DIR  	 := $(TEST_DIR)/src
OUT_DIR  	 := $(TEST_DIR)/out
APP_DIR	 	 := $(TEST_DIR)/app

TEST_TARGET := $(TEST_DIR)/test.exe

# Shell
DEL := del /Q

# Compiler & flags
CXX ?= g++

ifeq ($(CXX), clang++)
CXX_FLAGS := -std=c++2a -pedantic -Wno-c++98-compat -g
else
CXX_FLAGS := -std=c++2a -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3
endif

CXX_ARGS := -I $(INC_GLOB_DIR) -I $(INC_DIR) $(CXX_FLAGS)

# Test source & object files
SRC_FILES := $(wildcard $(APP_DIR)/*.cpp)

ifeq ($(words $(SRC_FILES)),1)
SRC_FILES += $(wildcard $(SRC_DIR)/*.cpp)
else ifneq (,$(filter all build,$(MAKECMDGOALS)))
$(error More than one .cpp file found in $(APP_DIR))
endif

APP_OBJ := $(foreach file, $(SRC_FILES), $(OUT_DIR)/$(notdir $(file:.cpp=.o)))

FILE_COUNT 	:= count.tmp
COUNT_OBJ 	:= $(shell powershell -command type $(OUT_DIR)/$(FILE_COUNT))
COUNT_SRC 	:= $(words $(SRC_FILES))

.PHONY: all build init destroy clean help

all: clean build

build: init $(TEST_TARGET) destroy

$(OUT_DIR)/main.o: $(APP_DIR)/main.cpp
	@$(shell powershell -command echo $$(($(shell powershell -command type $(OUT_DIR)/$(FILE_COUNT))+1)) > $(OUT_DIR)/$(FILE_COUNT))
	@echo [$(shell powershell -command type $(OUT_DIR)/$(FILE_COUNT))/$(COUNT_SRC)] Compiling: $<
	@$(CXX) -c $< -o $@ $(CXX_ARGS)

$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(shell powershell -command echo $$(($(shell powershell -command type $(OUT_DIR)/$(FILE_COUNT))+1)) > $(OUT_DIR)/$(FILE_COUNT))
	@echo [$(shell powershell -command type $(OUT_DIR)/$(FILE_COUNT))/$(COUNT_SRC)] Compiling: $<
	@$(CXX) -c $< -o $@ $(CXX_ARGS)

$(TEST_TARGET): $(APP_OBJ)
	@echo.
	@echo Linking: $@
	@$(CXX) $< -o $@ $(CXX_ARGS)
	@echo.
	@echo Build successful!

init:
	@$(shell powershell -command echo 0 > $(OUT_DIR)/$(FILE_COUNT))

destroy:
	@$(DEL) $(TEST_DIR)\out\*.tmp 2>NUL

clean:
	@echo Cleaning all generated files...
	@$(DEL) $(TEST_DIR)\out\*.o $(TEST_DIR)\out\*.tmp $(TEST_DIR)\*.exe 2>NUL
	@echo All generated files removed!
	@echo.

help:
	@echo Available targets:
	@echo   all        - Clean and build the test module
	@echo   build      - Build the test module
	@echo   clean      - Clean all generated files in the test module
	@echo   help       - Display this help message
