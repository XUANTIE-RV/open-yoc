#!/bin/sh

MK_GENERATED_PATH=generated

mkdir -p $MK_GENERATED_PATH

VOICE_PKG=`cat package.yaml | grep "\- voice_" | sed 's/[[:space:]\"]//g' | awk -F"[:-]" '{print $2"/"$3}'`
CONFIG_LDF=Packages/${VOICE_PKG}/sc5654/configs/gcc_flash.ld
cp ${CONFIG_LDF} ./
