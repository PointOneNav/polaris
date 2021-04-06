# Copyright (C) Point One Navigation - All Rights Reserved
project(p1_polaris_c_client)

cmake_minimum_required(VERSION 3.3.2)

# Option definitions.
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries."
       ON)

option(BUILD_EXAMPLES "Build example applications." ON)

set(POLARIS_DEBUG "OFF" CACHE STRING
    "Enable Polaris debug/trace print messages.")
set_property(CACHE POLARIS_DEBUG PROPERTY STRINGS OFF ON DEBUG TRACE)

option(POLARIS_ENABLE_TLS "Enable TLS support when connecting to Polaris." ON)

# Set compilation flags.
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Werror)
endif()

if(POLARIS_DEBUG STREQUAL "TRACE")
    add_compile_definitions(POLARIS_TRACE)
elseif(POLARIS_DEBUG)
    add_compile_definitions(POLARIS_DEBUG)
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

# Add the source directory to the include path.
include_directories(${PROJECT_SOURCE_DIR}/src)

# Polaris client C library - all messages and supporting code.
add_library(polaris_client
            src/point_one/polaris/polaris.c
            src/point_one/polaris/polaris_internal.c)
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

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
