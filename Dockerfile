# Copyright (C) Point One Navigation - All Rights Reserved
# Written by Jason Moran <jason@pointonenav.com>, March 2019

# Dockerfile to test building of project with CMake

FROM debian:jessie
MAINTAINER Pointone Navigation

RUN apt-get update && apt-get install -y \
      build-essential \
      cmake \
      libboost-system-dev \
      libgflags-dev \
      libgoogle-glog-dev

COPY . polaris

RUN mkdir /polaris/build2 && cd /polaris/build2 && cmake .. && make
