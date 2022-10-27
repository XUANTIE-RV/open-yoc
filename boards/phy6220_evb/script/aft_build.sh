#!/bin/bash
MK_TEMPFILES_PATH=temp
MK_GENERATED_IMGS_PATH=generated

APP_VERSION=$(cat package.yaml | grep -w 'CONFIG_APP_VERSION' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')

if  [ "${APP_VERSION}" == '' ];then
APP_VERSION=v1.0.0
fi

if [ "${CDK_VERSION}" == '' ];then
BOARD_PATH=../../boards/phy6220_evb
fi

if [ ! -d  ${BOARD_PATH} ];then
echo "Boards missing!!!!!!!"
fi

echo "Board path:$BOARD_PATH"

COMPRESS_FLAG=$(cat package.yaml | grep -w 'CONFIG_BOOT_COMPRESS' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')

if  [ "${COMPRESS_FLAG}" == '' ];then
COMPRESS_FLAG=$(cat $BOARD_PATH/package.yaml | grep -w 'CONFIG_BOOT_COMPRESS' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')
fi

if  [ "${COMPRESS_FLAG}" == '' ];then
COMPRESS_FLAG='0'
fi

echo "Compress enable: $COMPRESS_FLAG"

if [ "${COMPRESS_FLAG}" == '0' ];then
cp $BOARD_PATH/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/config.yaml
cp $BOARD_PATH/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/
cp $BOARD_PATH/bootimgs/bomtb ${MK_GENERATED_IMGS_PATH}/data/
if [ -f "$BOARD_PATH/bootimgs/kp" ] ;then
cp $BOARD_PATH/bootimgs/kp ${MK_GENERATED_IMGS_PATH}/data/
fi
else
cp $BOARD_PATH/configs/config_compress.yaml ${MK_GENERATED_IMGS_PATH}/data/config.yaml
cp $BOARD_PATH/bootimgs_compress/boot ${MK_GENERATED_IMGS_PATH}/data/
cp $BOARD_PATH/bootimgs_compress/bomtb ${MK_GENERATED_IMGS_PATH}/data/
if [ -f "$BOARD_PATH/bootimgs_compress/kp" ] ;then
cp $BOARD_PATH/bootimgs_compress/kp ${MK_GENERATED_IMGS_PATH}/data/
fi
fi

if [ "${CDK_VERSION}" != '' ];then
ELF_NAME=`ls Obj/*.elf`
OBJCOPY=csky-elfabiv2-objcopy
READELF=csky-elfabiv2-readelf
${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim
PRODUCT_BIN=${MK_TEMPFILES_PATH}/misc/product.exe
else
PRODUCT_BIN=${MK_TEMPFILES_PATH}/misc/product
chmod +x ${PRODUCT_BIN}
READELF=csky-abiv2-elf-readelf
ELF_NAME=`ls *.elf`
fi

${READELF} -S ${ELF_NAME}


echo "[INFO] Generated output files ..."

${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -v "${APP_VERSION}" -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -v "${APP_VERSION}" -spk ${MK_TEMPFILES_PATH}/misc/aes_128_ccm.key -dt SHA1 -st AES_128_CCM -iv 101112131415161718191a1b -aad 000102030405060708090a0b0c0d0e0f10111213 -tlen 16
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x -kp
${PRODUCT_BIN} diff -f ${MK_GENERATED_IMGS_PATH}/images.zip ${MK_GENERATED_IMGS_PATH}/images.zip -r -v "${APP_VERSION}" -spk ${MK_TEMPFILES_PATH}/misc/aes_128_ccm.key -dt SHA1 -st AES_128_CCM -iv 101112131415161718191a1b -aad 000102030405060708090a0b0c0d0e0f10111213 -tlen 16 -cp 0 -o  ${MK_GENERATED_IMGS_PATH}/fota.bin

cp ${MK_GENERATED_IMGS_PATH}/total_image.hex ${MK_GENERATED_IMGS_PATH}/total_image.hexf