# Minimum CMake version required to build the project
cmake_minimum_required(VERSION 3.12)

# Project name
project(cpp-ap)

# Setting paths to directories
set(SOURCE_DIRS "src" "app")
set(INCLUDE_DIRS "include" "../include")
set(BINARY_DIR "..")
set(EXECUTABLE_DIR "cmake/exe")

# Source files
file(GLOB_RECURSE SOURCES "")
foreach(SOURCE_DIR ${SOURCE_DIRS})
    file(GLOB_RECURSE CURRENT_SOURCES ${SOURCE_DIR}/*.cpp)
    list(APPEND SOURCES ${CURRENT_SOURCES})
endforeach()

# Directories with header files
include_directories(${INCLUDE_DIRS})

# Creating an executable target
add_executable(test ${SOURCES})

# Setting the output directory path
set_target_properties(test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}"
    CXX_STANDARD 20 
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

# Setting the path to the directory where executables will be linked
set(EXECUTABLE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${EXECUTABLE_DIR}")

# Creating symbolic links for executable files
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    file(CREATE_LINK "${BINARY_DIR}/test" "${EXECUTABLE_PATH}/test_${config}" SYMBOLIC)
endforeach(config)
