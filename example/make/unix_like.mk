# Structure
DIR_CURR 		:= .
DIR_PREV 		:= ..
DIR_INC_GLOB 	:= $(DIR_PREV)/include
DIR_SRC 		:= $(DIR_CURR)/src
DIR_EXE			:= $(DIR_CURR)/exe

# Shell
RM 		:= rm -rf
FIND 	:= find

# Compiler & flags
CXX ?= g++

ifeq ($(CXX), clang++)
CXX_FLAGS := -std=c++2a -pedantic -Wno-c++98-compat -g
else
CXX_FLAGS := -std=c++2a -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3
endif

CXX_ARGS := -I $(DIR_INC_GLOB) $(CXX_FLAGS)

# Enumeration
ENUMARATE := false

ifeq ($(filter all build,$(MAKECMDGOALS)),)
else
ENUMARATE := true
endif

# Test source & object files
SOURCES := $(wildcard $(DIR_SRC)/*.cpp)
OBJECTS := $(notdir $(SOURCES:.cpp=))

COUNT_OBJ := 0
COUNT_SRC := $(words $(SOURCES))

.PHONY: all build clean help

all: clean build

build: $(OBJECTS)
	@echo
	@echo Build successful!

%: $(DIR_SRC)/%.cpp 
ifeq ($(ENUMARATE), true)
	$(eval COUNT_OBJ=$(shell echo $$(($(COUNT_OBJ)+1))))
	@echo [$(COUNT_OBJ)/$(COUNT_SRC)] Compiling: $<
else
	@echo Compiling: $<
endif
	@$(CXX) $< -o $(DIR_EXE)/$@ $(CXX_ARGS)
ifeq ($(ENUMARATE), false)
	@echo Build successful!
	@echo
endif

clean:
	@echo Cleaning all generated files...
	@$(FIND) $(DIR_EXE) -type f ! -name .gitkeep -delete
	@echo All generated files removed!
	@echo

help:
	@echo "Available targets:"
	@echo "  all        - Clean and build the example module"
	@echo "  build      - Build the example module"
	@echo "  clean      - Clean all generated files in example module"
	@echo "  help       - Display this help message"
