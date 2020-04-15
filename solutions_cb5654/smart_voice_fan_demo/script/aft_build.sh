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
BOARDS_CONFIG_PATH="Boards/cb5864/${cb5864}/configs"
BOARDS_FIRMWARE_PATH="firmware"
BOARDS_SCRIPT_PATH="cb5864/script"
BOOTIMG_PATH="Boards/cb5864/${cb5864}/bootimgs"
cp ${BOOTIMG_PATH}/boot.bin $MK_GENERATED_PATH/data/boot
cp ${BOOTIMG_PATH}/para_only.bin $MK_GENERATED_PATH/data/para_only
cp ${BOOTIMG_PATH}/m0app.bin $MK_GENERATED_PATH/data/m0app
cp "$BOARDS_FIRMWARE_PATH/dsp.bin" $MK_GENERATED_PATH/data/dsp
cp "$BOARDS_FIRMWARE_PATH/audio.bin" $MK_GENERATED_PATH/data/audio
cp "$BOARDS_CONFIG_PATH/config.yaml" $MK_GENERATED_PATH/data/

#Create Image
PRODUCT_BIN="${BOARDS_CONFIG_PATH}/product.exe"
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -i $MK_GENERATED_PATH/data
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -l -b -p
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -e $MK_GENERATED_PATH -x
rm -fr $MK_GENERATED_PATH/data

