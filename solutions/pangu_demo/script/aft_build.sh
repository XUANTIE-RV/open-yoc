#!/bin/sh

#Parse from yaml
BOARD_NAME=`cat package.yaml | grep board_name | awk -F" " '{printf "%s", $2}'`
VERSION=`eval echo '$'"${BOARD_NAME}"`
BOARD_PATH=Boards/${BOARD_NAME}/${VERSION}

#Tools bin
PRODUCT_BIN=${BOARD_PATH}/configs/product.exe
OBJCOPY=csky-elfabiv2-objcopy

echo "[INFO] Generated output files ..."
ELF_NAME=`ls Obj/*.elf`
MK_GENERATED_IMGS_PATH=generated

#Dir prepare
rm -fr ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}/data

#Boot
if [ -f "${BOARD_PATH}/bootimgs/cpu1" ]; then cp ${BOARD_PATH}/bootimgs/cpu1 ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/cpu2" ]; then cp ${BOARD_PATH}/bootimgs/cpu2 ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/boot" ]; then cp ${BOARD_PATH}/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/tee" ]; then cp ${BOARD_PATH}/bootimgs/tee ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/lpm" ]; then cp ${BOARD_PATH}/bootimgs/lpm ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/kp" ]; then cp ${BOARD_PATH}/bootimgs/kp ${MK_GENERATED_IMGS_PATH}/data/; fi

#Convert
cp ${BOARD_PATH}/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/
${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x

#Cleanup
rm -fr ${MK_GENERATED_IMGS_PATH}/data
#rm gcc_eflash.ld

