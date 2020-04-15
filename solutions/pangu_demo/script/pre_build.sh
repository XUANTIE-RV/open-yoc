#!/bin/sh

BOARD_NAME=`cat package.yaml | grep board_name | awk -F" " '{printf "%s", $2}'`
VERSION=`eval echo '$'"${BOARD_NAME}"`

echo "Prepare ld file ..."
# cp Boards/${BOARD_NAME}/${VERSION}/configs/gcc_eflash.ld.S gcc_eflash.ld

