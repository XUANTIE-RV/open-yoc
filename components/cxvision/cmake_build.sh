#!/bin/bash

set -e

pushd third_party
./build_cJSON.sh
popd

rm -rf build && mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain_file.cmake
make -j16
