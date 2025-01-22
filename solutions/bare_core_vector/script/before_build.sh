#!/bin/bash

echo "before build !!!!!!!"

if [ "${CDK_VERSION}" == '' ];then
    exit
fi

if [ ! -d  "${SOLUTION_PATH}" ];then
    SOLUTION_PATH=$1
fi
if [ ! -d  "${BOARD_PATH}" ];then
    BOARD_PATH=$2
fi
if [ ! -d  "${CHIP_PATH}" ];then
    CHIP_PATH=$3
fi
PLATFORM=$4
CPU=$5

# echo $SOLUTION_PATH
# echo $BOARD_PATH
# echo $CHIP_PATH
# echo $PLATFORM
# echo $CPU

echo CDK_Version: ${CDK_VERSION}
echo Board Path: ${BOARD_PATH}

if [ ! -d  "${BOARD_PATH}" ];then
    echo "Boards missing!!!!!!!"
    exit
fi

BEFORE_BUILD="${BOARD_PATH}/script/pre_build.sh"

if [ -f  "${BEFORE_BUILD}" ];then
    sh "${BEFORE_BUILD}" $SOLUTION_PATH $BOARD_PATH $CHIP_PATH $PLATFORM $CPU
fi

