# Copyright (C) Point One Navigation - All Rights Reserved
project(p1_polaris_client)

cmake_minimum_required(VERSION 3.3.2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Option definitions.
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries."
       ON)

option(BUILD_EXAMPLES "Build example applications." ON)

# Set compilation flags.
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Werror)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
endif()

################################################################################
# Check for dependencies.
################################################################################

# Find glog.
option(GLOG_PREFER_EXPORTED_GLOG_CMAKE_CONFIGURATION TRUE)
find_package(Glog REQUIRED)
include_directories(${GLOG_INCLUDE_DIRS})

# Find gflags.
option(GFLAGS_PREFER_EXPORTED_GFLAGS_CMAKE_CONFIGURATION TRUE)
find_package(Gflags REQUIRED)
include_directories(${GFLAGS_INCLUDE_DIRS})

################################################################################
# Library Definitions
################################################################################

# Add the C library, used by the C++ library.
add_subdirectory(c)
include_directories(${PROJECT_SOURCE_DIR}/c/src)

# Add the source directory to the include path.
include_directories(${PROJECT_SOURCE_DIR}/src)

# Polaris client C++ library - all messages and supporting code.
add_library(polaris_cpp_client
            src/point_one/polaris/polaris_client.cc
            src/point_one/polaris/polaris_interface.cc)
if (MSVC)
    target_compile_definitions(polaris_cpp_client PRIVATE BUILDING_DLL)
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

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
