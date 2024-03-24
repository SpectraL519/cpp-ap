# Minimum CMake version required to build the project
cmake_minimum_required(VERSION 3.12)

# Project
project(cpp-ap-examples)

# Structure
set(SOURCE_DIR "source")
set(INCLUDE_DIRS "include" "../include")
set(BINARY_DIR "bin")
set(EXECUTABLE_DIR "bin")

# Source files
file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.cpp")

# Include dirs
include_directories(${INCLUDE_DIRS})

# Default compiler options
set(DEFAULT_CXX_FLAGS "-Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O3")

# Set compiler options
if(NOT DEFINED CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS ${DEFAULT_CXX_FLAGS} CACHE STRING "Default C++ compile flags" FORCE)
endif()

# Executables
foreach(source_file ${SOURCES})
    # get file name without extensions
    get_filename_component(executable_name ${source_file} NAME_WE)

    add_executable(${executable_name} ${source_file})
    # Target properties
    set_target_properties(${executable_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
    target_compile_options(${executable_name} PRIVATE ${CMAKE_CXX_FLAGS})
endforeach()

# Executable path
set(EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_DIR}")

# Symbolic links
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    file(CREATE_LINK "${BINARY_DIR}/test" "${EXECUTABLE_PATH}/test_${config}" SYMBOLIC)
endforeach(config)
