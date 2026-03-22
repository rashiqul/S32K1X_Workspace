cmake_minimum_required(VERSION 3.23)
project(CCppTemplateWorkspace C CXX)
enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options: Things you can set via commandline options to cmake (e.g. -DENABLE_LTO=[ON|OFF])
option(ENABLE_WARNINGS_SETTINGS "Allow target_set_warnings to add flags and defines.
                                 Set this to OFF if you want to provide your own warning parameters." ON)
option(ENABLE_LTO "Enable link time optimization" ON)
option(COVERAGE "Generate coverage data" OFF)

# Add local header-only libs as INTERFACE libraries
add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE ${CMAKE_SOURCE_DIR}/libs/spdlog/include)
target_compile_definitions(spdlog INTERFACE SPDLOG_FMT_EXTERNAL)

add_library(fmt INTERFACE)
target_include_directories(fmt INTERFACE ${CMAKE_SOURCE_DIR}/libs/fmt/include)
target_compile_definitions(fmt INTERFACE FMT_HEADER_ONLY)

add_library(autosar INTERFACE)
target_include_directories(autosar INTERFACE ${CMAKE_SOURCE_DIR}/libs/autosar-platform-types/include)

# Find GTest from Conan (only for host builds, not for ARM bare-metal target)
if(CMAKE_BUILD_TYPE MATCHES "Debug|Release" AND NOT CMAKE_CROSSCOMPILING)
    find_package(GTest CONFIG REQUIRED)
endif()

# Add cmake modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Simplified cmake configuration for local development
include(GNUInstallDirs)

# Set version if not provided
if(NOT DEFINED CONAN_PACKAGE_VERSION)
    set(CONAN_PACKAGE_VERSION "0.1.0")
endif()

# Generate config.h from config.h.in
configure_file(
    ${CMAKE_SOURCE_DIR}/include/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
    @ONLY
)

if(COVERAGE AND NOT CMAKE_CROSSCOMPILING)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
endif()
message(STATUS "COVERAGE=${COVERAGE}")

if(CMAKE_CROSSCOMPILING)
    add_subdirectory(bsp)
endif()

add_subdirectory(src)

if(NOT CMAKE_CROSSCOMPILING)
    # Using local GTest and spdlog from libs/
    add_subdirectory(test)
endif()

# Install include directories and libs
install(DIRECTORY include/ DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.in" EXCLUDE
)
install(DIRECTORY libs/autosar-platform-types/include/ DESTINATION include)
install(DIRECTORY libs/spdlog/include/ DESTINATION include)
install(DIRECTORY libs/fmt/include/ DESTINATION include)

install(DIRECTORY docs/ DESTINATION docs)