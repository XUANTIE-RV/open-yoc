#!/bin/bash

if [ "${CDK_VERSION}" != '' ];then
#Parse from yaml

echo CDK_Version: ${CDK_VERSION}
echo Board Path: ${BOARD_PATH}

if [ ! -d  ${BOARD_PATH} ];then
echo "Boards missing!!!!!!!"
fi

MK_TEMPFILES_PATH=temp/
rm -rf "${MK_TEMPFILES_PATH}"
mkdir -p "${MK_TEMPFILES_PATH}script/"

BOARD_BUILD_SCRIPT=${BOARD_PATH}/script/

if [ -f "${BOARD_BUILD_SCRIPT}pre_build.sh" ]; then
cp "${BOARD_BUILD_SCRIPT}pre_build.sh" "${MK_TEMPFILES_PATH}/script/"
fi

if [ -f "${BOARD_BUILD_SCRIPT}aft_build.sh" ]; then
cp "${BOARD_BUILD_SCRIPT}aft_build.sh" "${MK_TEMPFILES_PATH}/script/"
fi

if [ -f "${MK_TEMPFILES_PATH}script/pre_build.sh" ]; then
"${MK_TEMPFILES_PATH}script/pre_build.sh"
fi

fi
