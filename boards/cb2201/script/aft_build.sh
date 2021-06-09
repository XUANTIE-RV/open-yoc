#!/bin/sh

#Parse from yaml
echo $BOARD_PATH

CUR_BOARD_PATH=$BOARD_PATH
CUR_SOLUTION_PATH=$SOLUTION_PATH

#Tools bin
PRODUCT_BIN=${CUR_BOARD_PATH}/configs/product.exe
OBJCOPY=csky-elfabiv2-objcopy

echo "[INFO] Generated output files ..."
ELF_NAME=`ls Obj/*.elf`
MK_GENERATED_IMGS_PATH=generated

#Dir prepare
rm -fr ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}/data

#Boot
if [ -f "${CUR_BOARD_PATH}/bootimgs/mtb" ]; then cp ${CUR_BOARD_PATH}/bootimgs/mtb ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${CUR_BOARD_PATH}/bootimgs/boot" ]; then cp ${CUR_BOARD_PATH}/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${CUR_BOARD_PATH}/bootimgs/tee" ]; then cp ${CUR_BOARD_PATH}/bootimgs/tee ${MK_GENERATED_IMGS_PATH}/data/; fi

if [ -f "${CUR_BOARD_PATH}/script/flash.init" ]; then cp ${CUR_BOARD_PATH}/script/flash.init ${CUR_SOLUTION_PATH}/script/flash.init; fi
if [ -f "${CUR_BOARD_PATH}/script/cdkgdbinit" ]; then cp ${CUR_BOARD_PATH}/script/cdkgdbinit ${CUR_SOLUTION_PATH}/script/cdkgdbinit; fi
if [ -f "${CUR_BOARD_PATH}/script/flash.init" ]; then cp ${CUR_BOARD_PATH}/script/flash.init ${CUR_SOLUTION_PATH}/script/flash.init; fi

#Convert
cp ${CUR_BOARD_PATH}/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/
${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x

#Cleanup
#rm -fr ${MK_GENERATED_IMGS_PATH}/data
