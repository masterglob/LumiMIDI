#!/bin/bash
CMAKE_BUILD_TYPE=Release
mkdir -p build && cd build
cmake --build . --config ${CMAKE_BUILD_TYPE} -j4

