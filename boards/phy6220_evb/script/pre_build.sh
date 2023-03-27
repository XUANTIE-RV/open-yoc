#!/bin/bash
if [ "${CDK_VERSION}" != '' ];then

if [ ! -d  ${!BOARD_PATH} ];then
echo "Boards missing!!!!!!!"
fi
MK_TEMPFILES_PATH=temp
MK_GENERATED_IMGS_PATH=generated

rm -fr "${MK_GENERATED_IMGS_PATH}"
mkdir -p "${MK_GENERATED_IMGS_PATH}/data/"

mkdir -p temp/misc
cp "${BOARD_PATH}/rom_ck802.elf.ck802sym" "${MK_TEMPFILES_PATH}/misc"

fi
