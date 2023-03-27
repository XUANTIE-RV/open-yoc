#!/bin/bash

echo "before build !!!!!!!"

if [ "${CDK_VERSION}" == '' ];then
    exit
fi

echo CDK_Version: ${CDK_VERSION}
echo Board Path: ${BOARD_PATH}

if [ ! -d  "${BOARD_PATH}" ];then
    echo "Boards missing!!!!!!!"
    exit
fi

BEFORE_BUILD="${BOARD_PATH}/script/pre_build.sh"

if [ -f  "${BEFORE_BUILD}" ];then
    sh "${BEFORE_BUILD}"
fi

