#!/bin/bash

if [ "$1" == '' ];then
#Parse from yaml
BOARD_PATH=$PATH_W800_EVB
PRODUCT_BIN=${BOARD_PATH}/configs/product.exe
ELF_NAME=`ls Obj/*.elf`
else
PRODUCT_BIN=product
fi

#Tools bin
OBJCOPY=csky-elfabiv2-objcopy

echo "[INFO] Generated output files ..."


MK_GENERATED_IMGS_PATH=generated

if [ "$1" == '' ];then
#Dir prepare
rm -fr ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}/data

#Boot
if [ -f "${BOARD_PATH}/bootimgs/bomtb" ]; then cp ${BOARD_PATH}/bootimgs/bomtb ${MK_GENERATED_IMGS_PATH}/data/; fi
if [ -f "${BOARD_PATH}/bootimgs/boot" ]; then cp ${BOARD_PATH}/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/; fi
#if [ -f "${BOARD_PATH}/bootimgs/tee" ]; then cp ${BOARD_PATH}/bootimgs/tee ${MK_GENERATED_IMGS_PATH}/data/; fi
#Convert
cp ${BOARD_PATH}/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/
${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim
fi

${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -b

echo "Create wm bin files"
if [ "$1" == '' ];then
#unzip -u ${MK_GENERATED_IMGS_PATH}/images
cp ${MK_GENERATED_IMGS_PATH}/boot boot.bin
cp ${MK_GENERATED_IMGS_PATH}/prim prim.bin
cp ${MK_GENERATED_IMGS_PATH}/imtb imtb.bin
${BOARD_PATH}/configs/wm_tool -b boot.bin -fc 0 -it 512 -ih 8002000 -ra 8002400 -ua 8010000 -nh 80D0000 -un 0
${BOARD_PATH}/configs/wm_tool -b prim.bin -fc 0 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0
${BOARD_PATH}/configs/wm_tool -b prim.img -fc 1 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0
${BOARD_PATH}/configs/wm_tool -b imtb.bin -fc 0 -it 1 -ih 20047400 -ra 81C0000 -ua 0 -nh 0 -un 0
cat boot.img prim.img imtb.img > yoc_total.fls
mv prim_gz.img yoc_ota.bin
rm -f boot.bin prim.bin imtb.bin boot.img prim.img imtb.img
else
csky-elfabiv2-objcopy -O binary ./out/$1/yoc.elf ./out/$1/yoc.bin
gcc ../../components/chip_w800/wmsdk/tools/w800/wm_tool.c -Wall -lpthread -O2 -o ./out/$1/wm_tool
./out/$1/wm_tool -b ./out/$1/yoc.bin -fc 0 -it 1 -ih 80D0000 -ra 80D0400 -ua 8010000 -nh 0 -un 0
cat ../../components/chip_w800/wmsdk/tools/w800/w800_secboot.img ./out/$1/yoc.img > ./out/$1/yoc.fls
fi

