#!/bin/sh

#Parse from yaml

CUR_BOARD_PATH=$BOARD_PATH

echo $CUR_BOARD_PATH

#Tools bin
PRODUCT_BIN=${CUR_BOARD_PATH}/configs/product.exe
OBJCOPY=riscv64-unknown-elf-objcopy

echo "[INFO] Generated output files ..."
ELF_NAME=`ls Obj/*.elf`
MK_GENERATED_IMGS_PATH=generated

#Dir prepare
rm -fr ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}/data

echo $ELF_NAME
#Boot
if [ -f "${CUR_BOARD_PATH}/bootimgs/mtb" ]; then cp ${CUR_BOARD_PATH}/bootimgs/mtb ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${CUR_BOARD_PATH}/bootimgs/boot" ]; then cp ${CUR_BOARD_PATH}/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/; fi
#Convert
cp configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/
${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x

#Cleanup
#rm -fr ${MK_GENERATED_IMGS_PATH}/data
