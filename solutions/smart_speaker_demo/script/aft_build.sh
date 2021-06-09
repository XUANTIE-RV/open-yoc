#!/bin/sh
MK_GENERATED_PATH=generated

echo "[INFO] Generated output files ..."

rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/
BOARD_PATH=$PATH_CB5654
OBJCOPY=csky-elfabiv2-objcopy
PRODUCT_BIN=${BOARD_PATH}/configs/product.exe

#output yoc.bin
ELF_NAME=`ls Obj/*.elf`
$OBJCOPY -O binary $ELF_NAME $MK_GENERATED_PATH/data/prim

#Prepare bin
# BOARDS_CONFIG_PATH="Boards/cb5654/${cb5654}/configs"

CKBOOT_BIN=${BOARD_PATH}/bootimgs/boot_0278.bin
M0BOOT_BIN=${BOARD_PATH}/bootimgs/m0flash_0278_dbs.bin

VOICE_PKG=`cat package.yaml | grep "\- voice_" | tr a-z A-Z | sed 's/ //g' | awk -F"[:-]" '{print $2"/"$3}'`
VOICE_PKG="PATH_"${VOICE_PKG%/*}
echo "VOICE ..........."
echo $VOICE_PKG
DSPFW_BIN=${!VOICE_PKG}/sc5654/dspfw.bin
CONFIG_YAML=${!VOICE_PKG}/sc5654/configs/config.yaml
LFS_BIN=app/src/audio/lfs.bin

cp ${CKBOOT_BIN} $MK_GENERATED_PATH/data/boot
cp ${M0BOOT_BIN} $MK_GENERATED_PATH/data/m0flash
cp ${DSPFW_BIN} $MK_GENERATED_PATH/data/dsp
cp ${LFS_BIN} $MK_GENERATED_PATH/data/lfs
cp ${CONFIG_YAML} $MK_GENERATED_PATH/data/

#Create Image
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -i $MK_GENERATED_PATH/data
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -l -b -p
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -e $MK_GENERATED_PATH -x
rm -fr $MK_GENERATED_PATH/data

