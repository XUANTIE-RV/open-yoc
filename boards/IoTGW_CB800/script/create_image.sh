#!/usr/bin/env bash

if [ "$1" = "make" ];then
echo "create board_w800 img"
     product image ./$2/images.zip -i ./$2/data -l -p
     product image ./$2/images.zip -e ./$2 -x

echo "Create wm bin files"
    csky-elfabiv2-objcopy -O binary ./out/$3/yoc.elf ./out/$3/yoc.bin
    gcc ../../components/chip_w800/wmsdk/tools/w800/wm_tool.c -Wall -lpthread -O2 -o ./out/$3/wm_tool
    ./out/$3/wm_tool -b ./out/$3/yoc.bin -fc 0 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0
    cat ../../components/chip_w800/wmsdk/tools/w800/w800_secboot.img ./out/$3/yoc.img > ./out/$3/yoc.fls
else
echo "flash board_w800"
    product flash ./$2/images.zip -w prim -f w800_flash.elf
fi