#!/bin/bash
CMAKE_BUILD_TYPE=Debug
[ "$1" == "-c" ] && rm -fr ./build/
mkdir -p build && cd build
[ "$1" == "-l" ] || cmake .. -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
cmake --build . --config ${CMAKE_BUILD_TYPE} -j8

