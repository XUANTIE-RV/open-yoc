#!/bin/bash

make clean && make cpu=r908 board=xiaohui -f Makefile1 -j8
xxd -i yoc.bin 1.h
cp 1.h app/include/cpu1_firmware.h -arf
echo "update app/include/cpu1_firmware.h ok."