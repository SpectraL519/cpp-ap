# Minimum CMake version required to build the project
cmake_minimum_required(VERSION 3.12)

# Project
project(cpp-ap-test)
enable_testing()

# Structure
set(INCLUDE_DIRS "include" "external")
set(EXECUTABLE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Set compile options
set(DEFAULT_CXX_FLAGS "-Werror -Wall -Wextra -Wcast-align -Wconversion -Wunreachable-code -Wuninitialized -pedantic -g -O0")
if (NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${DEFAULT_CXX_FLAGS}" CACHE STRING "Default C++ compiler flags" FORCE)
endif()
message(STATUS "[CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}]")
set(COMMON_COMPILE_DEFINITIONS "AP_TESTING")


# Decalre test targets
add_library(doctest_main STATIC app/main.cpp)
set_target_properties(doctest_main PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_include_directories(doctest_main PUBLIC "external")

macro(add_doctest SOURCE_FILE)
    # Parse macro arguments
    get_filename_component(TEST_NAME ${SOURCE_FILE} NAME_WE)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs COMPILE_DEFINITIONS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Define the executable
    add_executable(${TEST_NAME} ${SOURCE_FILE})
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_DIR}"
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
    target_include_directories(${TEST_NAME} PRIVATE ${INCLUDE_DIRS})
    target_compile_definitions(${TEST_NAME} PRIVATE ${COMMON_COMPILE_DEFINITIONS})
    if(ARGS_COMPILE_DEFINITIONS)
        target_compile_definitions(${TEST_NAME} PRIVATE ${ARGS_COMPILE_DEFINITIONS})
    endif()
    target_link_libraries(${TEST_NAME} PRIVATE doctest_main cpp-ap-2)

    # Register with CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

add_doctest("source/test_argument_descriptor.cpp")
add_doctest("source/test_argument_name.cpp")
add_doctest("source/test_argument_parser_add_argument.cpp")
add_doctest("source/test_argument_parser_info.cpp")
add_doctest("source/test_argument_parser_parse_args.cpp")
add_doctest("source/test_argument_parser_parse_args_unknown_flags_as_values.cpp"
    COMPILE_DEFINITIONS "AP_UNKNOWN_FLAGS_AS_VALUES"
)
add_doctest("source/test_nargs_range.cpp")
add_doctest("source/test_optional_argument.cpp")
add_doctest("source/test_positional_argument.cpp")
add_doctest("source/test_str_utility.cpp")
