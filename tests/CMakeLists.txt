# Minimum CMake version required to build the project
cmake_minimum_required(VERSION 3.12)

# Project
project(cpp-argon-test)

# Set compile options
if (MSVC)
    set(DEFAULT_CXX_FLAGS "/W4 /WX /Od /Z7")
    # /W4 (warnings)
    # /WX (warnings as errors)
    # /Od (no optimizations)
    # /Z7 (debug info)
else()
    set(DEFAULT_CXX_FLAGS "-Werror -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O0")
endif()

if (NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}" CACHE STRING "Default C++ compiler flags" FORCE)
endif()
message(STATUS "[CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}]")
set(COMMON_COMPILE_DEFINITIONS "AP_TESTING")

# Structure
set(SOURCE_DIRS "source" "app")
set(INCLUDE_DIRS "include" "external")
set(EXECUTABLE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Source files
file(GLOB_RECURSE SOURCES "")
foreach(SOURCE_DIR ${SOURCE_DIRS})
    file(GLOB_RECURSE CURRENT_SOURCES ${SOURCE_DIR}/*.cpp)
    list(APPEND SOURCES ${CURRENT_SOURCES})
endforeach()

# Executable
add_executable(run_uts ${SOURCES})
set_target_properties(run_uts PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_DIR}"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_include_directories(run_uts PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(run_uts PRIVATE "AP_TESTING")
target_link_libraries(run_uts PRIVATE cpp-argon)
