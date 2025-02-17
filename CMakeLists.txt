cmake_minimum_required(VERSION 3.12)

if (NOT DEFINED PROJECT_NAME)
    set(CPP_AP_IS_TOP_LEVEL_PROJECT ON)
else()
    set(CPP_AP_IS_TOP_LEVEL_PROJECT OFF)
endif()

project(cpp-ap
    VERSION 2.0.1
    DESCRIPTION "Command-line argument parser for C++20"
    HOMEPAGE_URL "https://github.com/SpectraL519/cpp-ap"
    LANGUAGES CXX
)

# Options
option(BUILD_TESTS "Build project tests" OFF)

# The library target
add_library(cpp-ap-2 INTERFACE)
target_include_directories(cpp-ap-2 INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(cpp-ap-2 PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

# Installation configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-ap-2)

# Install the headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the library target
install(TARGETS cpp-ap-2 EXPORT cpp-ap-2-targets)

# Create a config file for find_package
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpp-ap-2-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-2-config.cmake
    INSTALL_DESTINATION ${INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-2-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-2-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-2-config-version.cmake
    DESTINATION ${INSTALL_DIR}
)

install(EXPORT cpp-ap-2-targets
    FILE cpp-ap-2-targets.cmake
    NAMESPACE cpp-ap-2::
    DESTINATION ${INSTALL_DIR}
)

# Include the tests directory if CPP-AP is a top-level project
if (CPP_AP_IS_TOP_LEVEL_PROJECT AND BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Exporting from the build tree
export(EXPORT cpp-ap-2-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-2-targets.cmake
    NAMESPACE cpp-ap-2::
)

export(PACKAGE cpp-ap-2)
