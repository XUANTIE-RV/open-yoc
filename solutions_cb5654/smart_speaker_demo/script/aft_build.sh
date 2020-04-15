#!/bin/sh
BASE_PWD=`pwd`
MK_GENERATED_PATH=generated

echo "[INFO] Generated output files ..."

rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/

OBJCOPY=csky-elfabiv2-objcopy

#output yoc.bin
ELF_NAME=`ls Obj/*.elf`
$OBJCOPY -O binary $ELF_NAME $MK_GENERATED_PATH/data/prim

#Prepare bin
BOARDS_CONFIG_PATH="Boards/cb5654/${cb5654}/configs"
BOARD_PATH="Boards/cb5654/${cb5654}"

CKBOOT_BIN=${BOARD_PATH}/bootimgs/boot_0278.bin
M0BOOT_BIN=${BOARD_PATH}/bootimgs/m0flash_0278_dbs.bin

VOICE_PKG=`cat package.yaml | grep "\- voice_" | sed 's/[[:space:]\"]//g' | awk -F"[:-]" '{print $2"/"$3}'`
DSPFW_BIN=Packages/${VOICE_PKG}/sc5654/dspfw.bin
CONFIG_YAML=Packages/${VOICE_PKG}/sc5654/configs/config.yaml

cp ${CKBOOT_BIN} $MK_GENERATED_PATH/data/boot
cp ${M0BOOT_BIN} $MK_GENERATED_PATH/data/m0flash
cp ${DSPFW_BIN} $MK_GENERATED_PATH/data/dsp
cp ${CONFIG_YAML} $MK_GENERATED_PATH/data/

#Create Image
PRODUCT_BIN="${BOARDS_CONFIG_PATH}/product.exe"
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -i $MK_GENERATED_PATH/data
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -l -b -p
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -e $MK_GENERATED_PATH -x
rm -fr $MK_GENERATED_PATH/data

