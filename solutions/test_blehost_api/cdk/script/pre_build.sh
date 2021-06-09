#!/bin/bash
if [ "${CDK_VERSION}" != '' ];then
#Parse from yaml
BOARD_NAME=`grep "board_name" package.yaml | awk '{printf "%s", toupper($2)}'| xargs echo `

BOARD_PATH=PATH_${BOARD_NAME}
BOARD_PATH=${!BOARD_PATH}

if [ ! -d  ${!BOARD_PATH} ];then
echo "Boards missing!!!!!!!"
fi

MK_GENERATED_IMGS_PATH=generated

rm -fr ${MK_GENERATED_IMGS_PATH}
mkdir -p ${MK_GENERATED_IMGS_PATH}/data/

cp ${BOARD_PATH}/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/
cp ${BOARD_PATH}/bootimgs/bomtb ${MK_GENERATED_IMGS_PATH}/data/
cp ${BOARD_PATH}/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/

cp ${BOARD_PATH}/configs/aes_128_ccm.key ${MK_GENERATED_IMGS_PATH}/
cp ${BOARD_PATH}/configs/PHY6220_Flash.elf cdk/script/
cp ${BOARD_PATH}/configs/product.exe cdk/script/
cp ${BOARD_PATH}/script/flash.init cdk/script/

mkdir -p misc
cp ${BOARD_PATH}/rom_ck802.elf.ck802sym misc/

fi
