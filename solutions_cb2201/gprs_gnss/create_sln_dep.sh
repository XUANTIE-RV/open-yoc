#!/bin/sh

# 在yoc同目录建立临时目录，复制工程到临时目录中
# 配置 SDK_BASE及COMP_VER
# 执行后创建Boards Chips Packages，可用cdk打开工程

SDK_BASE=../../
COMP_VER=V7.2.2

PACK_LIST=`cat project.cdkproj | grep "<Package ID" | awk -F"\"" '{printf"%s\n",$2}'`
BOARD_NAME=`cat project.cdkproj | grep "<Board ID" | awk -F"\"" '{printf"%s",$2}'`
CHIP_NAME=`cat project.cdkproj | grep "<Chip ID" | awk -F"\"" '{printf"%s",$2}'`

#Clean
rm -fr Boards Chips Packages

#Create Boards
echo "Boards" ${BOARD_NAME}
mkdir -p Boards/${BOARD_NAME}
cp -r ${SDK_BASE}/boards/csky/${BOARD_NAME}  Boards/${BOARD_NAME}/${COMP_VER}

#Create Chips
echo "Chips" ${CHIP_NAME}
mkdir -p Chips/${CHIP_NAME}
cp -r ${SDK_BASE}/components/${CHIP_NAME}  Chips/${CHIP_NAME}/${COMP_VER}

#Packages
for CPNT in ${PACK_LIST}; do
    echo "Packages" ${CPNT}
    mkdir -p Packages/${CPNT}
    cp -r ${SDK_BASE}/components/${CPNT}  Packages/${CPNT}/${COMP_VER}
done
