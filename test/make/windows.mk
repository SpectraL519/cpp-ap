# Structure
DIR_CURR 		:= .
DIR_PREV 		:= ..
DIR_INC 		:= $(DIR_CURR)/include
DIR_INC_GLOB 	:= $(DIR_PREV)/include
DIR_SRC 		:= $(DIR_CURR)/src
DIR_OUT 		:= $(DIR_CURR)/out
DIR_APP			:= $(DIR_CURR)/app

APP_TARGET 	:= test.exe

# Shell
DEL := del /Q

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

FILE_COUNT 	:= count.tmp
COUNT_OBJ 	:= $(shell powershell -command type $(DIR_OUT)/$(FILE_COUNT))
COUNT_SRC 	:= $(words $(APP_SRC))

.PHONY: all build init destroy clean cleansys cleanall help

all: clean build

build: init $(APP_TARGET) destroy

$(DIR_OUT)/main.o: $(DIR_APP)/main.cpp
	@$(shell powershell -command echo $$(($(shell powershell -command type $(DIR_OUT)/$(FILE_COUNT))+1)) > $(DIR_OUT)/$(FILE_COUNT))
	@echo [$(shell powershell -command type $(DIR_OUT)/$(FILE_COUNT))/$(COUNT_SRC)] Compiling: $<
	@$(CXX) -c $< -o $@ $(CXX_ARGS)

$(DIR_OUT)/%.o: $(DIR_SRC)/%.cpp
	@$(shell powershell -command echo $$(($(shell powershell -command type $(DIR_OUT)/$(FILE_COUNT))+1)) > $(DIR_OUT)/$(FILE_COUNT))
	@echo [$(shell powershell -command type $(DIR_OUT)/$(FILE_COUNT))/$(COUNT_SRC)] Compiling: $<
	@$(CXX) -c $< -o $@ $(CXX_ARGS)

$(APP_TARGET): $(APP_OBJ)
	@echo.
	@echo Linking: $@
	@$(CXX) $^ -o $@ $(CXX_ARGS)
	@echo.
	@echo Build successful!

init:
	@$(shell powershell -command echo 0 > $(DIR_OUT)/$(FILE_COUNT))

destroy:
	@$(DEL) $(DIR_CURR)\out\*.tmp 2>NUL

clean:
	@echo Cleaning all generated files...
	@$(DEL) $(DIR_CURR)\out\*.o $(DIR_CURR)\out\*.tmp $(DIR_CURR)\*.exe 2>NUL
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
	@echo   all        - Clean and build the test module
	@echo   build      - Build the test module
	@echo   clean      - Clean all generated files in the test module
	@echo   help       - Display this help message
	@echo   cleanall   - Clean all generated files
	@echo   cleansys   - Clean .sys generated file
	