#!/bin/sh
MK_GENERATED_PATH=generated

echo "[INFO] Generated output files ..."

rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/
CUR_BOARD_PATH=$BOARD_PATH
CUR_SOLUTION_PATH=$SOLUTION_PATH
OBJCOPY=csky-elfabiv2-objcopy
PRODUCT_BIN=${CUR_BOARD_PATH}/configs/product.exe

#output yoc.bin
ELF_NAME=`ls Obj/*.elf`
$OBJCOPY -O binary $ELF_NAME $MK_GENERATED_PATH/data/prim

#Prepare bin
# BOARDS_CONFIG_PATH="Boards/cb5654/${cb5654}/configs"

CKBOOT_BIN=${CUR_BOARD_PATH}/bootimgs/boot_0278.bin
M0BOOT_BIN=${CUR_BOARD_PATH}/bootimgs/m0flash_0278_dbs.bin
DSPFW_BIN=${CUR_BOARD_PATH}/bootimgs/dspfw.bin
CONFIG_YAML=${CUR_BOARD_PATH}/configs/config.yaml
LFS_BIN=${CUR_BOARD_PATH}/bootimgs/lfs.bin

cp ${CKBOOT_BIN} $MK_GENERATED_PATH/data/boot
cp ${M0BOOT_BIN} $MK_GENERATED_PATH/data/m0flash
cp ${DSPFW_BIN} $MK_GENERATED_PATH/data/dsp
cp ${LFS_BIN} $MK_GENERATED_PATH/data/lfs
cp ${CONFIG_YAML} $MK_GENERATED_PATH/data/
cp ${CUR_BOARD_PATH}/script/flash.init ${CUR_SOLUTION_PATH}/script/flash.init
cp ${CUR_BOARD_PATH}/script/cdkgdbinit ${CUR_SOLUTION_PATH}/script/cdkgdbinit
cp ${CUR_BOARD_PATH}/script/flash.init ${CUR_SOLUTION_PATH}/script/flash.init

#Create Image
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -i $MK_GENERATED_PATH/data
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -l -b -p
${PRODUCT_BIN} image $MK_GENERATED_PATH/images.zip -e $MK_GENERATED_PATH -x
rm -fr $MK_GENERATED_PATH/data

