#!/bin/bash

make clean && make cpu=e907 board=smartl -f Makefile1 -j8
xxd -i yoc.bin 1.h
cp 1.h app/include/e907_firmware.h -arf
echo "update app/include/e907_firmware.h ok."