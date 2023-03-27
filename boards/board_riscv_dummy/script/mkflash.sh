#bin/bash

MK_GENERATED_IMGS_PATH=generated
PRODUCTBIN=product
IMGZIP_PATH=${MK_GENERATED_IMGS_PATH}/images.zip
PARTITION=$1
FLASH_ELF=$2


if [ ! -f "${IMGZIP_PATH}" ];then
    echo "${IMGZIP_PATH} is not existed!"
    exit
fi

if [ ! -f "${FLASH_ELF}" ];then
    echo "${FLASH_ELF} is not existed!"
    exit
fi

if [[ "$1" == "erasechip" ]];then
    ${PRODUCTBIN} flash ${IMGZIP_PATH} -ep -f $2 -x gdbinitflash
    echo "erase chip over!"
    exit
fi

echo "want to burn $1"
if [[ "$1" == "all" ]];then
    ${PRODUCTBIN} flash ${IMGZIP_PATH} -a -f $2 -x gdbinitflash
    echo "burn all over!"
    exit
fi

${PRODUCTBIN} flash ${IMGZIP_PATH} -w $1 -f $2 -x gdbinitflash
echo "burn $3 over!"
