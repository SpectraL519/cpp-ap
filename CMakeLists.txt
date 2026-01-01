cmake_minimum_required(VERSION 3.12)

if (NOT DEFINED PROJECT_NAME)
    set(CPP_ARGON_IS_TOP_LEVEL_PROJECT ON)
else()
    set(CPP_ARGON_IS_TOP_LEVEL_PROJECT OFF)
endif()

project(cpp-argon
    VERSION 4.0.0
    DESCRIPTION "Command-line argument parser for C++20"
    HOMEPAGE_URL "https://github.com/SpectraL519/cpp-argon"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_TESTS "Build project tests" OFF)

# The library target
add_library(cpp-argon INTERFACE)
target_include_directories(cpp-argon INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(cpp-argon PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

# Installation configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-argon)

# Install the headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the library target
install(TARGETS cpp-argon EXPORT cpp-argon-targets)

# Create a config file for find_package
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpp-argon-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-argon-config.cmake
    INSTALL_DESTINATION ${INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-argon-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-argon-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-argon-config-version.cmake
    DESTINATION ${INSTALL_DIR}
)

install(EXPORT cpp-argon-targets
    FILE cpp-argon-targets.cmake
    NAMESPACE cpp-argon::
    DESTINATION ${INSTALL_DIR}
)

# Include the tests directory if CPP-ARGON is a top-level project
if (CPP_ARGON_IS_TOP_LEVEL_PROJECT AND BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Exporting from the build tree
export(EXPORT cpp-argon-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cpp-argon-targets.cmake
    NAMESPACE cpp-argon::
)

export(PACKAGE cpp-argon)
