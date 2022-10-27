#!/bin/bash

MK_GENERATED_IMGS_PATH=generated

APP_VERSION=$(cat package.yaml | grep -w 'CONFIG_APP_VERSION' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')

if  [ "${APP_VERSION}" == '' ];then
APP_VERSION=v1.0.0
fi

rm -fr wm_tmp
mkdir -p wm_tmp

if [ "${CDK_VERSION}" == '' ];then
BOARD_PATH=../../boards/IoTGW_CB800
fi

if [ ! -d  ${BOARD_PATH} ];then
echo "Boards missing!!!!!!!"
fi

echo "Board path:$BOARD_PATH"

COMPRESS_FLAG=$(cat package.yaml | grep -w 'CONFIG_BOOT_COMPRESS' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')

if  [ "${COMPRESS_FLAG}" == '' ];then
COMPRESS_FLAG=$(cat $BOARD_PATH/package.yaml | grep -w 'CONFIG_BOOT_COMPRESS' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')
fi

echo "Boot compress: $COMPRESS_FLAG"

if  [ "${COMPRESS_FLAG}" == '' ];then
COMPRESS_FLAG='1'
fi

echo "Boot compress: $COMPRESS_FLAG"

cp "$BOARD_PATH/script/flash.init" ./temp/script/

if [ "${CDK_VERSION}" != '' ];then
echo ${CDK_VERSION}
#Parse from yaml
PRODUCT_BIN=${BOARD_PATH}/configs/product.exe
ELF_NAME=`ls Obj/*.elf`
WM_TOOL=${BOARD_PATH}/configs/wm_tool.exe

#Dir prepare
rm -fr ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}/data
else

if [ ! -d "${MK_GENERATED_IMGS_PATH}/data" ];then
  mkdir -p ${MK_GENERATED_IMGS_PATH}/data
fi

PRODUCT_BIN=./temp/script/product
chmod +x ${PRODUCT_BIN}
gcc ../../components/chip_w800/wmsdk/tools/w800/wm_tool.c -Wall -lpthread -O2 -o ./wm_tmp/wm_tool
cp ../../components/chip_w800/wmsdk/tools/w800/wm_tool ./wm_tmp/wm_tool
WM_TOOL=./wm_tmp/wm_tool
chmod +x ${WM_TOOL}
ELF_NAME=yoc.elf
fi

if [ "${COMPRESS_FLAG}" == '0' ];then
cp $BOARD_PATH/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/config.yaml
cp $BOARD_PATH/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/
if [ -f "$BOARD_PATH/bootimgs/bt" ] ;then
cp $BOARD_PATH/bootimgs/bt ${MK_GENERATED_IMGS_PATH}/data/
fi
if [ -f "$BOARD_PATH/bootimgs/fcds" ] ;then
cp $BOARD_PATH/bootimgs/fcds ${MK_GENERATED_IMGS_PATH}/data/
fi
else
cp $BOARD_PATH/configs/config_compress.yaml ${MK_GENERATED_IMGS_PATH}/data/config.yaml
cp $BOARD_PATH/bootimgs_compress/boot ${MK_GENERATED_IMGS_PATH}/data/
if [ -f "$BOARD_PATH/bootimgs_compress/bt" ] ;then
cp $BOARD_PATH/bootimgs_compress/bt ${MK_GENERATED_IMGS_PATH}/data/
fi
if [ -f "$BOARD_PATH/bootimgs_compress/fcds" ] ;then
cp $BOARD_PATH/bootimgs_compress/fcds ${MK_GENERATED_IMGS_PATH}/data/
fi
fi

#Tools bin
OBJCOPY=csky-elfabiv2-objcopy

echo "[INFO] Generated output files ..."

${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim

${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -v "${APP_VERSION}" -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -tb
${PRODUCT_BIN} diff -f ${MK_GENERATED_IMGS_PATH}/images.zip ${MK_GENERATED_IMGS_PATH}/images.zip -r -v "${APP_VERSION}" -cp 2 -o ${MK_GENERATED_IMGS_PATH}/fota.bin

echo "Create wm bin files"

cp ${MK_GENERATED_IMGS_PATH}/data/boot wm_tmp/boot.bin
cp ${MK_GENERATED_IMGS_PATH}/data/prim wm_tmp/yoc.bin
cp ${MK_GENERATED_IMGS_PATH}/data/prim wm_tmp/prim.bin
cp ${MK_GENERATED_IMGS_PATH}/imtb wm_tmp/imtb.bin


${WM_TOOL} -b ./wm_tmp/yoc.bin -fc 0 -it 1 -ih 8132C00 -ra 8013000 -ua 8133000 -nh 0 -un 0 -vs yoc-7.3.0
cat ../../components/chip_w800/wmsdk/tools/w800/w800_secboot.img ./wm_tmp/yoc.img > ${MK_GENERATED_IMGS_PATH}/wm_yoc.fls
${WM_TOOL} -b ./wm_tmp/yoc.img -fc 1 -it 1 -ih 8132C00 -ra 8013000 -ua 8133000 -nh 0 -un 0 -vs yoc-7.3.0
mv ./wm_tmp/yoc_gz.img ${MK_GENERATED_IMGS_PATH}/wm_yoc_ota.bin

echo [INFO] Create fls file

${WM_TOOL} -b ./wm_tmp/boot.bin -fc 0 -it 512 -ih 8002000 -ra 8002400 -ua 8133000 -nh 8132C00 -un 0
${WM_TOOL} -b ./wm_tmp/prim.bin -fc 0 -it 1 -ih 8132C00 -ra 8013000 -ua 8133000 -nh 0 -un 0
${WM_TOOL} -b ./wm_tmp/imtb.bin -fc 0 -it 1 -ih 20047400 -ra 800F000 -ua 0 -nh 0 -un 0
cat ./wm_tmp/boot.img ./wm_tmp/prim.img ./wm_tmp/imtb.img > ${MK_GENERATED_IMGS_PATH}/wm_yoc_total.fls
rm -fr wm_tmp
if [ -f "${BOARD_PATH}/bootimgs/bt" ]; then cat ${MK_GENERATED_IMGS_PATH}/wm_yoc_ota.bin ${MK_GENERATED_IMGS_PATH}/data/bt > ${MK_GENERATED_IMGS_PATH}/wm_total_fota.bin
else
cp ${MK_GENERATED_IMGS_PATH}/wm_yoc_ota.bin ${MK_GENERATED_IMGS_PATH}/wm_total_fota.bin
fi

