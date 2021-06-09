#!/bin/sh

#Parse from yaml

CUR_BOARD_PATH=$BOARD_PATH

echo $CUR_BOARD_PATH

./$CUR_BOARD_PATH/script/pre_build.sh

