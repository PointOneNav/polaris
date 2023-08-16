# Copyright (C) Point One Navigation - All Rights Reserved
cmake_minimum_required(VERSION 3.6)

# Option definitions.
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries."
       ON)

option(POLARIS_BUILD_EXAMPLES "Build example applications." ON)

option(POLARIS_ENABLE_PRINT
       "Enable Polaris debug/trace print messages." ON)

option(POLARIS_ENABLE_TLS "Enable TLS support when connecting to Polaris." ON)

# Backwards compatibility. BUILD_EXAMPLES is deprecated and may be removed in a
# future release.
if (DEFINED BUILD_EXAMPLES AND BUILD_EXAMPLES)
  set(POLARIS_BUILD_EXAMPLES ${BUILD_EXAMPLES})
endif()

# Define the project and setup the compiler toolchain.
project(p1_polaris_c_client)

# Set compilation flags.
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Werror)
endif()

if(NOT POLARIS_ENABLE_PRINT)
    add_compile_definitions(POLARIS_NO_PRINT)
endif()

################################################################################
# Check for dependencies.
################################################################################

# Find OpenSSL (or BoringSSL).
if (POLARIS_ENABLE_TLS)
    find_package(OpenSSL REQUIRED)
endif()

################################################################################
# Library Definitions
################################################################################

# Polaris client C library - all messages and supporting code.
add_library(polaris_client
            src/point_one/polaris/polaris.c
            src/point_one/polaris/polaris_internal.c)
target_include_directories(polaris_client PUBLIC ${PROJECT_SOURCE_DIR}/src)
if (MSVC)
    target_compile_definitions(polaris_client PRIVATE BUILDING_DLL)
endif()
if (POLARIS_ENABLE_TLS)
    target_compile_definitions(polaris_client PUBLIC POLARIS_USE_TLS=1)
    target_include_directories(polaris_client PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(polaris_client PUBLIC ${OPENSSL_LIBRARIES})
endif()

# Install targets.
install(TARGETS polaris_client
        LIBRARY DESTINATION lib)

install(DIRECTORY src/point_one DESTINATION include
        FILES_MATCHING PATTERN "*.h")

################################################################################
# Example Applications
################################################################################

if (POLARIS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
