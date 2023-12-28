# Structure
DIR_CURR 		:= .
DIR_PREV 		:= ..
DIR_INC_GLOB 		:= $(DIR_PREV)/include
DIR_SRC 		:= $(DIR_CURR)/src
DIR_EXE			:= $(DIR_CURR)/exe

# Shell
DEL := del /Q

# Compiler & flags
CXX ?= g++

ifeq ($(CXX), clang++)
CXX_FLAGS := -Weverything -pedantic -Wno-c++98-compat -g
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

FILE_COUNT 	:= count.tmp
COUNT_OBJ 	:= 0
COUNT_SRC 	:= $(words $(SOURCES))

.PHONY: all build init destroy clean cleansys cleanall help

all: clean build

build: init $(OBJECTS) destroy
	@echo.
	@echo Build successful!

%: $(DIR_SRC)/%.cpp
ifeq ($(ENUMARATE), true)
	@$(shell powershell -command echo $$(($(shell powershell -command type $(DIR_CURR)/$(FILE_COUNT))+1)) > $(DIR_CURR)/$(FILE_COUNT))
	@echo [$(shell powershell -command type $(DIR_CURR)/$(FILE_COUNT))/$(COUNT_SRC)] Compiling: $<
else
	@echo Compiling: $<
endif
	@$(CXX) $< -o $(DIR_EXE)/$@ $(CXX_ARGS)
ifeq ($(ENUMARATE), false)
	@echo Build successful!
	@echo.
endif

init:
	@$(shell powershell -command echo 0 > $(DIR_CURR)/$(FILE_COUNT))

destroy:
	@$(DEL) $(DIR_CURR)\$(FILE_COUNT) 2>NUL

clean:
	@echo Cleaning all generated files...
	@for %%i in ($(DIR_EXE)\*) do if /I not "%%~nxi" == ".gitkeep" del "%%i"
	@$(DEL) $(DIR_CURR)\$(FILE_COUNT) 2>NUL
	@echo All generated files removed!
	@echo.

cleansys:
	@echo Removing .sys file...
	@$(DEL) ..\log\windows.sys
	@echo Removed .sys file!
	@echo.

cleanall: clean cleansys
	@echo Removed everything!
	@echo.

help:
	@echo Available targets:
	@echo   all        - Clean and build the example module
	@echo   build      - Build the example module
	@echo   clean      - Clean all generated files in example module
	@echo   help       - Display this help message

