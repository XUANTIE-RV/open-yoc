#!/bin/sh

#Parse from yaml
echo $PATH_CB2201

BOARD_PATH=$PATH_CB2201

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
if [ -f "${BOARD_PATH}/bootimgs/mtb" ]; then cp ${BOARD_PATH}/bootimgs/mtb ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/boot" ]; then cp ${BOARD_PATH}/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/tee" ]; then cp ${BOARD_PATH}/bootimgs/tee ${MK_GENERATED_IMGS_PATH}/data/; fi
#Convert
cp ${BOARD_PATH}/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/
${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x

#Cleanup
rm -fr ${MK_GENERATED_IMGS_PATH}/data
