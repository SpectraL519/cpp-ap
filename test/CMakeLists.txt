# Minimum CMake version required to build the project
cmake_minimum_required(VERSION 3.12)

# Project
project(cpp-ap-test)

# Structure
set(SOURCE_DIRS "source" "app")
set(INCLUDE_DIRS "include")
set(EXECUTABLE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Source files
file(GLOB_RECURSE SOURCES "")
foreach(SOURCE_DIR ${SOURCE_DIRS})
    file(GLOB_RECURSE CURRENT_SOURCES ${SOURCE_DIR}/*.cpp)
    list(APPEND SOURCES ${CURRENT_SOURCES})
endforeach()

# Set compile options
if(NOT DEFINED CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3"
        CACHE STRING "Default C++ compile flags" FORCE)
endif()

# Executable
add_executable(run_tests ${SOURCES})

set_target_properties(run_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_DIR}"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_include_directories(run_tests PRIVATE ${INCLUDE_DIRS})
target_compile_options(run_tests PRIVATE ${CMAKE_CXX_FLAGS})
target_link_libraries(run_tests PRIVATE cpp-ap)

# Create symbolic links in the build directory
set(EXECUTABLE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    file(CREATE_LINK "${EXECUTABLE_DIR}/run_tests" "${EXECUTABLE_DIR}/test_${config}" SYMBOLIC)
endforeach(config)
