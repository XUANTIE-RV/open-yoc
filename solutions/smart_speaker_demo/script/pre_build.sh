#!/bin/sh

MK_GENERATED_PATH=generated

mkdir -p $MK_GENERATED_PATH
VOICE_PKG=`cat package.yaml | grep "\- voice_" | tr a-z A-Z | sed 's/ //g' | awk -F"[:-]" '{print $2"/"$3}'`
VOICE_PKG=${VOICE_PKG%/*}
echo $VOICE_PKG
VOICE="PATH_"$VOICE_PKG
echo $VOICE


CONFIG_LDF=${!VOICE}/sc5654/configs/gcc_flash.ld
echo $CONFIG_LDF
cp ${CONFIG_LDF} ./
