#!/bin/bash

set -e

if [ ! -d cJSON ]; then
  git clone https://github.com/DaveGamble/cJSON.git -b v1.7.15 --depth=1
fi
cd cJSON
rm -rf build && mkdir build
cd build
cmake .. -DENABLE_CJSON_TEST=Off -DBUILD_SHARED_LIBS=Off -DCMAKE_TOOLCHAIN_FILE=../../toolchain_file.cmake
make
