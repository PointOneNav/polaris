################################################################################
# This is a simple example of how to import the Polaris client library in
# your own project using the CMake FetchContent feature. We strongly encourage
# you to use FetchContent to download Polaris client from the publicly available
# source code.
#
# Alternatively, you may choose to use a git submodule to import the source code
# into your repository. We do not recommend copying the Polaris client souce
# code directly into your repository. Doing so makes it much more difficult to
# receive updates as new features and improvements are released.
################################################################################

# Note: CMake 3.18 or newer required to use SOURCE_SUBDIR below in order to
# include just the Polaris C library and not the top-level C++ library.
cmake_minimum_required(VERSION 3.18)

project(polaris_usage_example C)

# Use FetchContent to import the Polaris client C library using Git.
#
# Note that we always recommend using a specific version of the library in your
# code by specifying a git tag (e.g., `GIT_TAG v1.3.1`), and updating that as
# new versions are released. That way, you can be sure that your code is always
# built with a known version of Polaris client. If you prefer, however, you can
# set the GIT_TAG to track the latest changes by setting `GIT_TAG master` below.
#
# We explicitly disable example applications from the Polaris client library by
# setting POLARIS_BUILD_EXAMPLES to OFF below. We only want to build the library
# and make the fusion_engine_client CMake target available here. By default, if
# we do not tell it otherwise, FetchContent_MakeAvailable() will also import all
# of the example applications in polaris/c/examples/.
#
# It is important to specify it as an INTERNAL variable. If you do not do this,
# the option definition in the fusion-engine-client CMakeLists.txt file will
# override this value and enable the example applications anyway. This is a
# result of CMP0077, which was added in CMake 3.13.
include(FetchContent)
FetchContent_Declare(
    polaris
    GIT_REPOSITORY https://github.com/PointOneNav/polaris.git
    GIT_TAG v1.3.1
    SOURCE_SUBDIR c
)
set(POLARIS_BUILD_EXAMPLES OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(polaris)

# Now we define an example application that uses the Polaris client library.
# In your own code, you can link any add_executable() or add_library() target
# with polaris_client by calling target_link_libraries() as shown.
add_executable(example_app main.c)
target_link_libraries(example_app PUBLIC polaris_client)
