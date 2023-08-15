# Copyright (C) Point One Navigation - All Rights Reserved
project(p1_polaris_client)

cmake_minimum_required(VERSION 3.3.2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# Option definitions.
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries."
       ON)

option(POLARIS_BUILD_EXAMPLES "Build example applications." ON)

# Backwards compatibility. BUILD_EXAMPLES is deprecated and may be removed in a
# future release.
if (DEFINED BUILD_EXAMPLES AND BUILD_EXAMPLES)
  set(POLARIS_BUILD_EXAMPLES ${BUILD_EXAMPLES})
endif()

# Set compilation flags.
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Werror -Wno-unused-variable)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
endif()

################################################################################
# Check for dependencies.
################################################################################

# Find gflags.
option(GFLAGS_PREFER_EXPORTED_GFLAGS_CMAKE_CONFIGURATION TRUE)
find_package(Gflags REQUIRED)
include_directories(${GFLAGS_INCLUDE_DIRS})

# Find glog (requires gflags).
option(GLOG_PREFER_EXPORTED_GLOG_CMAKE_CONFIGURATION TRUE)
find_package(Glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIRS})

################################################################################
# Library Definitions
################################################################################

# Add the C library, used by the C++ library.
add_subdirectory(c)

# Polaris client C++ library - all messages and supporting code.
add_library(polaris_cpp_client
            src/point_one/polaris/polaris_client.cc
            src/point_one/polaris/polaris_interface.cc)
target_include_directories(polaris_client PUBLIC ${PROJECT_SOURCE_DIR}/src)
if (MSVC)
    target_compile_definitions(polaris_cpp_client PRIVATE BUILDING_DLL)
endif()
if (POLARIS_ENABLE_TLS)
    target_compile_definitions(polaris_cpp_client PUBLIC POLARIS_USE_TLS=1)
endif()
target_link_libraries(polaris_cpp_client PUBLIC polaris_client)
target_link_libraries(polaris_cpp_client PUBLIC ${GLOG_LIBRARIES})
target_link_libraries(polaris_cpp_client PUBLIC ${GFLAGS_LIBRARIES})

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
