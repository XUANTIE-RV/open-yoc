#!/bin/bash
if [ "${CDK_VERSION}" != '' ];then

if [ ! -d  ${!BOARD_PATH} ];then
echo "Boards missing!!!!!!!"
fi
MK_TEMPFILES_PATH=temp
MK_GENERATED_IMGS_PATH=generated

rm -fr "${MK_GENERATED_IMGS_PATH}"
mkdir -p "${MK_GENERATED_IMGS_PATH}/data/"

#cp ${BOARD_PATH}/bootimgs/boot ${MK_GENERATED_IMGS_PATH}/data/
#cp ${BOARD_PATH}/bootimgs/bomtb ${MK_GENERATED_IMGS_PATH}/data/
#cp ${BOARD_PATH}/configs/config.yaml ${MK_GENERATED_IMGS_PATH}/data/
#if [ -f "${BOARD_PATH}/bootimgs/kp" ]; then cp ${BOARD_PATH}/bootimgs/kp ${MK_GENERATED_IMGS_PATH}/data/; fi

cp "${BOARD_PATH}/configs/PHY6220_Flash.elf" "${MK_TEMPFILES_PATH}/script"
cp "${BOARD_PATH}/script/flash.init" "${MK_TEMPFILES_PATH}/script"

mkdir -p temp/misc
cp "${BOARD_PATH}/configs/product.exe" "${MK_TEMPFILES_PATH}/misc"
cp "${BOARD_PATH}/configs/aes_128_ccm.key" "${MK_TEMPFILES_PATH}/misc"
cp "${BOARD_PATH}/rom_ck802.elf.ck802sym" "${MK_TEMPFILES_PATH}/misc"

fi
