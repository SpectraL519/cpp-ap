# Minimum CMake version required to build the project
cmake_minimum_required(VERSION 3.12)

# Project
project(cpp-ap-test)

# Structure
set(SOURCE_DIRS "source" "app")
set(INCLUDE_DIRS "include" "../include")
set(BINARY_DIR ".")
set(EXECUTABLE_DIR ".")

# Source files
file(GLOB_RECURSE SOURCES "")
foreach(SOURCE_DIR ${SOURCE_DIRS})
    file(GLOB_RECURSE CURRENT_SOURCES ${SOURCE_DIR}/*.cpp)
    list(APPEND SOURCES ${CURRENT_SOURCES})
endforeach()

# Include dirs
include_directories(${INCLUDE_DIRS})

# Default compile options
set(DEFAULT_CXX_FLAGS "-Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3")

# Set compile options
if(NOT DEFINED CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS ${DEFAULT_CXX_FLAGS} CACHE STRING "Default C++ compile flags" FORCE)
endif()

# Executable
add_executable(test ${SOURCES})

# Target properties
set_target_properties(test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_compile_options(test PRIVATE ${CMAKE_CXX_FLAGS})

# Executable path
set(EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_DIR}")

# Symbolic links
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    file(CREATE_LINK "${BINARY_DIR}/test" "${EXECUTABLE_PATH}/test_${config}" SYMBOLIC)
endforeach(config)
