#!/bin/bash

echo "after build !!!!!!!"

if [ "${CDK_VERSION}" == '' ];then
    exit
fi

echo CDK_Version: ${CDK_VERSION}
echo Board Path: ${BOARD_PATH}

if [ ! -d  "${BOARD_PATH}" ];then
    echo "Boards missing!!!!!!!"
    exit
fi

AFTER_BUILD="${BOARD_PATH}/script/aft_build.sh"

if [ -f  "${AFTER_BUILD}" ];then
    sh "${AFTER_BUILD}"
fi


