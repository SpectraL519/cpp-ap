# Structure
DIR_CURR 		:= .
DIR_PREV 		:= ..
DIR_INC 		:= $(DIR_CURR)/include
DIR_INC_GLOB 	:= $(DIR_PREV)/include
DIR_SRC 		:= $(DIR_CURR)/src
DIR_OUT 		:= $(DIR_CURR)/out
DIR_APP			:= $(DIR_CURR)/app

APP_TARGET 	:= test

# Shell
RM 		:= rm -rf
FIND 	:= find

# Compiler & flags
CXX ?= g++

ifeq ($(CXX), clang++)
CXX_FLAGS := -Weverything -pedantic -Wno-c++98-compat -g
else
CXX_FLAGS := -std=c++2a -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3
endif

CXX_ARGS := -I $(DIR_INC_GLOB) -I $(DIR_INC) $(CXX_FLAGS)

# Test source & object files
APP_SRC := $(wildcard $(DIR_APP)/*.cpp)

ifeq ($(words $(APP_SRC)),1)
APP_SRC += $(wildcard $(DIR_SRC)/*.cpp)
else ifneq (,$(filter all build,$(MAKECMDGOALS)))
$(error More than one .cpp file found in $(DIR_APP))
endif

APP_OBJ := $(foreach file, $(APP_SRC), $(DIR_OUT)/$(notdir $(file:.cpp=.o)))

COUNT_OBJ := 0
COUNT_SRC := $(words $(APP_SRC))

.PHONY: all build clean cleansys cleanall help

all: clean build

build: $(APP_TARGET)

$(DIR_OUT)/main.o: $(DIR_APP)/main.cpp
	$(eval COUNT_OBJ=$(shell echo $$(($(COUNT_OBJ)+1))))
	@echo [$(COUNT_OBJ)/$(COUNT_SRC)] Compiling: $<
	@$(CXX) -c $< -o $@ $(CXX_ARGS)

$(DIR_OUT)/%.o: $(DIR_SRC)/%.cpp
	$(eval COUNT_OBJ=$(shell echo $$(($(COUNT_OBJ)+1))))
	@echo [$(COUNT_OBJ)/$(COUNT_SRC)] Compiling: $<
	@$(CXX) -c $< -o $@ $(CXX_ARGS)

$(APP_TARGET): $(APP_OBJ)
	@echo
	@echo Linking: $@
	@$(CXX) $^ -o $@ $(CXX_ARGS)
	@echo
	@echo Build successful!

clean:
	@echo Cleaning all generated files...
	@$(RM) 	$(DIR_OUT)/*.o
	@$(FIND) . -type f -executable -not -iname "*.*" -not -iname "Makefile" -delete
	@echo All generated files removed!
	@echo

cleansys:
	@echo Removing .sys file...
	@$(RM) $(DIR_LOG)/*.sys
	@echo Removed .sys file!
	@echo

cleanall: clean cleansys
	@echo Removed everything!
	@echo

help:
	@echo "Available targets:"
	@echo "  all        - Clean and build the test module"
	@echo "  build      - Build the test module"
	@echo "  clean      - Clean all generated files in test module"
	@echo "  help       - Display this help message"
	@echo "  cleanall   - Clean all generated files"
	@echo "  cleansys   - Clean .sys generated file"
 