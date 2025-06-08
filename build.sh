#!/bin/bash
CMAKE_BUILD_TYPE=Release
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
cmake --build . --config ${CMAKE_BUILD_TYPE} -j4

