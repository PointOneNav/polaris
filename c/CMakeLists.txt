# Copyright (C) Point One Navigation - All Rights Reserved
project(p1_polaris_c_client)

cmake_minimum_required(VERSION 3.3.2)

# Option definitions.
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries."
       ON)

option(BUILD_EXAMPLES "Build example applications." ON)

# Set compilation flags.
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Werror)
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
