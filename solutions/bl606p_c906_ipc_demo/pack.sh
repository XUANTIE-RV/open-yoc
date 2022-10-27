#!/bin/bash

echo "static uint8_t g_c906_fw[] = {" >> c906_fw.h
xxd -i < yoc.bin >> c906_fw.h
echo "};" >> c906_fw.h

var=`grep "__stext" yoc.asm | awk -F'<' '{print $1}'`
var2=${var:0-9:8}
echo "static uint32_t g_c906_fw_addr = 0x"$var2";" >> c906_fw.h
# BOARD_PATH=`yoc variable | grep "BOARD_PATH" | awk -F'=' '{print $2}'`
# echo $BOARD_PATH
cp c906_fw.h ../../boards/bl606p_evb_e907/include/
rm c906_fw.h
echo "copy ok"
# echo -n $var2 >> c906_fw.h
