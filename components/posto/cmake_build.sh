#!/bin/bash

set -e

rm -rf build && mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain_file.cmake
make -j16
