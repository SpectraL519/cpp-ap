cmake_minimum_required(VERSION 3.12)

# check if CPP-AP is a top level project
if (NOT DEFINED PROJECT_NAME)
    set(CPP_AP_IS_TOP_LEVEL_PROJECT ON)
else()
    set(CPP_AP_IS_TOP_LEVEL_PROJECT OFF)
endif()

project(
    cpp-ap
    VERSION 1.0
    DESCRIPTION "Command-line argument parser for C++20"
    HOMEPAGE_URL "https://github.com/SpectraL519/cpp-ap"
    LANGUAGES CXX
)

# define the library target
add_library(cpp-ap INTERFACE)
target_include_directories(cpp-ap INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# installation rules
include(GNUInstallDirs)
install(TARGETS cpp-ap
    EXPORT cpp-ap-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# generate and install the CMake package configuration files
install(EXPORT cpp-ap-targets
    FILE cpp-ap-targets.cmake
    NAMESPACE cpp-ap::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-ap
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/cpp-ap-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-ap
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-ap
)

if (CPP_AP_IS_TOP_LEVEL_PROJECT)
    option(CPP_AP_BUILD_TESTS "Build tests" ON)
    option(CPP_AP_BUILD_EXAMPLES "Build examples" OFF)

    if(CPP_AP_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()

    if(CPP_AP_BUILD_TESTS)
        enable_testing()
        add_subdirectory(test)
    endif()

    if(CPP_AP_INSTALL)
        install(EXPORT cpp-ap-targets
            FILE cpp-ap-config.cmake
            NAMESPACE ap::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-ap
        )

        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/cpp-ap-config-version.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpp-ap
        )
    endif()

    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
    set(CPACK_PACKAGE_VENDOR "SpectraL519")
    set(CPACK_PACKAGE_CONTACT "Jakub Musiał jmusial2003@gmail.com")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

    include(CPack)
endif()
