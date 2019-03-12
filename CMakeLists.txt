# Copyright (C) Point One Navigation - All Rights Reserved
# Written by Jason Moran <jason@pointonenav.com>, March 2019

# Builds a sample client for connection to Point One Navigation's
# correction service - Polaris.

PROJECT(polaris)

CMAKE_MINIMUM_REQUIRED (VERSION 3.0.2)
SET(CMAKE_CXX_FLAGS "-g -Wall -Werror -Wno-unused-local-typedef -std=c++11 -pthread -DNO_DECRYPTION=1" )
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/examples)
ADD_EXECUTABLE(asio_example examples/asio_example.cc)

ADD_SUBDIRECTORY(third_party)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
ADD_EXECUTABLE(septentrio_example examples/septentrio_example.cc)
TARGET_LINK_LIBRARIES(septentrio_example septentrio)


# FIND Glog
FIND_PACKAGE(Glog)
IF(FOUND_INSTALLED_GLOG_CMAKE_CONFIGURATION)
 TARGET_LINK_LIBRARIES(asio_example glog::glog ${GLOG_LIBRARY})
 TARGET_LINK_LIBRARIES(septentrio_example glog::glog ${GLOG_LIBRARY})
ELSE(FOUND_INSTALLED_GLOG_CMAKE_CONFIGURATION)
 TARGET_LINK_LIBRARIES(asio_example glog ${GLOG_LIBRARY})
 TARGET_LINK_LIBRARIES(septentrio_example glog ${GLOG_LIBRARY})
ENDIF(FOUND_INSTALLED_GLOG_CMAKE_CONFIGURATION)

# FIND gflags
FIND_PACKAGE(Gflags)
TARGET_LINK_LIBRARIES(asio_example gflags ${GFLAG_LIBRARY})
TARGET_LINK_LIBRARIES(septentrio_example gflags ${GFLAG_LIBRARY})

# FIND Boost
FIND_PACKAGE(Boost REQUIRED system)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(asio_example ${Boost_SYSTEM_LIBRARY})
TARGET_LINK_LIBRARIES(septentrio_example ${Boost_SYSTEM_LIBRARY})
