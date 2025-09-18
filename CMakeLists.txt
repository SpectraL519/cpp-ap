cmake_minimum_required(VERSION 3.12)

if (NOT DEFINED PROJECT_NAME)
    set(CPP_AP_IS_TOP_LEVEL_PROJECT ON)
else()
    set(CPP_AP_IS_TOP_LEVEL_PROJECT OFF)
endif()

project(cpp-ap
    VERSION 3.0.0.3
    DESCRIPTION "Command-line argument parser for C++20"
    HOMEPAGE_URL "https://github.com/SpectraL519/cpp-ap"
    LANGUAGES CXX
)

# Options
option(BUILD_TESTS "Build project tests" OFF)

# The library target
add_library(cpp-ap INTERFACE)
target_include_directories(cpp-ap INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(cpp-ap PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

# Installation configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-ap)

# Install the headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the library target
install(TARGETS cpp-ap EXPORT cpp-ap-targets)

# Create a config file for find_package
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cpp-ap-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config.cmake
    INSTALL_DESTINATION ${INSTALL_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config-version.cmake
    DESTINATION ${INSTALL_DIR}
)

install(EXPORT cpp-ap-targets
    FILE cpp-ap-targets.cmake
    NAMESPACE cpp-ap::
    DESTINATION ${INSTALL_DIR}
)

# Include the tests directory if CPP-AP is a top-level project
if (CPP_AP_IS_TOP_LEVEL_PROJECT AND BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Exporting from the build tree
export(EXPORT cpp-ap-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-targets.cmake
    NAMESPACE cpp-ap::
)

export(PACKAGE cpp-ap)
