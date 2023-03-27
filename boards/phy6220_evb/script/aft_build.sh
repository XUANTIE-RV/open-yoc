#!/bin/bash

BASE_PWD=`pwd`
MK_BOARD_PATH=$BOARD_PATH
MK_CHIP_PATH=$CHIP_PATH
MK_SOLUTION_PATH=$SOLUTION_PATH

echo "[INFO] Generated output files ..."
echo $BASE_PWD

EXE_EXT=`which ls | grep -o .exe`
if [ -n "$EXE_EXT" ]; then
    echo "I am in CDK."
    OBJCOPY=csky-elfabiv2-objcopy
    ELF_NAME=`ls Obj/*.elf`
    READELF=csky-elfabiv2-readelf
    PRODUCT=${MK_BOARD_PATH}/configs/product$EXE_EXT
    ${OBJCOPY} -O binary ${ELF_NAME} yoc.bin
else
    echo "I am in Linux."
    while getopts ":s:b:c:a:" optname
    do
        case "$optname" in
        "s")
            MK_SOLUTION_PATH=$OPTARG
            ;;
        "b")
            MK_BOARD_PATH=$OPTARG
            ;;
        "c")
            MK_CHIP_PATH=$OPTARG
            ;;
        "a")
            # echo "the all variables from yoctools, value is $OPTARG"
            ;;
        "h")
            ;;
        ":")
            echo "No argument value for option $OPTARG"
            ;;
        "?")
            echo "Unknown option $OPTARG"
            ;;
        *)
            echo "Unknown error while processing options"
            ;;
        esac
        #echo "option index is $OPTIND"
    done
    PRODUCT=${MK_BOARD_PATH}/configs/product
    chmod +x ${PRODUCT}
    READELF=csky-abiv2-elf-readelf
    ELF_NAME=`ls *.elf`
    OBJCOPY=csky-abiv2-elf-objcopy
    ${OBJCOPY} -O binary ${ELF_NAME} yoc.bin
fi

MK_GENERATED_PATH=${MK_SOLUTION_PATH}/generated
rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/

echo $MK_SOLUTION_PATH
echo $MK_BOARD_PATH
echo $MK_CHIP_PATH
echo $MK_GENERATED_PATH

if [ ! -f gdbinitflash ]; then
    cp -arf $MK_BOARD_PATH/script/gdbinitflash $BASE_PWD
fi

if [ ! -f cdkinitflash ]; then
    cp -arf $MK_BOARD_PATH/script/cdkinitflash $BASE_PWD
fi

if [ ! -f gdbinit ]; then
    cp -arf $MK_BOARD_PATH/script/gdbinit $BASE_PWD
fi

cp -arf $MK_BOARD_PATH/script/mkflash.sh $BASE_PWD

APP_VERSION=$(cat package.yaml | grep -w 'CONFIG_APP_VERSION' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')

if  [ "${APP_VERSION}" = '' ];then
APP_VERSION=v1.0.0
fi

echo APP_VERSION ${APP_VERSION}

COMPRESS_FLAG=$(cat package.yaml | grep -w 'CONFIG_BOOT_COMPRESS' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')

if  [ "${COMPRESS_FLAG}" = '' ];then
COMPRESS_FLAG=$(cat $MK_BOARD_PATH/package.yaml | grep -w 'CONFIG_BOOT_COMPRESS' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')
fi

if  [ "${COMPRESS_FLAG}" = '' ];then
COMPRESS_FLAG='0'
fi

echo "Compress enable: $COMPRESS_FLAG"

if [ "${COMPRESS_FLAG}" = '0' ];then
cp -arf $MK_BOARD_PATH/configs/config.yaml ${MK_GENERATED_PATH}/data/config.yaml
cp -arf $MK_BOARD_PATH/bootimgs/boot ${MK_GENERATED_PATH}/data/
cp -arf $MK_BOARD_PATH/bootimgs/bomtb ${MK_GENERATED_PATH}/data/
if [ -f "$MK_BOARD_PATH/bootimgs/kp" ] ;then
cp -arf $MK_BOARD_PATH/bootimgs/kp ${MK_GENERATED_PATH}/data/
fi
else
cp -arf $MK_BOARD_PATH/configs/config_compress.yaml ${MK_GENERATED_PATH}/data/config.yaml
cp -arf $MK_BOARD_PATH/bootimgs_compress/boot ${MK_GENERATED_PATH}/data/
cp -arf $MK_BOARD_PATH/bootimgs_compress/bomtb ${MK_GENERATED_PATH}/data/
if [ -f "$MK_BOARD_PATH/bootimgs_compress/kp" ] ;then
cp -arf $MK_BOARD_PATH/bootimgs_compress/kp ${MK_GENERATED_PATH}/data/
fi
fi

if [ -f "yoc.bin" ]; then
cp yoc.bin ${MK_GENERATED_PATH}/data/prim -fr
fi

${READELF} -S ${ELF_NAME}

echo "[INFO] Generated output files ..."

${PRODUCT} image ${MK_GENERATED_PATH}/images.zip -i ${MK_GENERATED_PATH}/data -l -v "${APP_VERSION}" -p
${PRODUCT} image ${MK_GENERATED_PATH}/images.zip -v "${APP_VERSION}" -spk ${MK_BOARD_PATH}/configs/aes_128_ccm.key -dt SHA1 -st AES_128_CCM -iv 101112131415161718191a1b -aad 000102030405060708090a0b0c0d0e0f10111213 -tlen 16
${PRODUCT} image ${MK_GENERATED_PATH}/images.zip -e ${MK_GENERATED_PATH} -x -kp
${PRODUCT} diff -f ${MK_GENERATED_PATH}/images.zip ${MK_GENERATED_PATH}/images.zip -r -v "${APP_VERSION}" -spk ${MK_BOARD_PATH}/configs/aes_128_ccm.key -dt SHA1 -st AES_128_CCM -iv 101112131415161718191a1b -aad 000102030405060708090a0b0c0d0e0f10111213 -tlen 16 -cp 0 -o  ${MK_GENERATED_PATH}/fota.bin

cp ${MK_GENERATED_PATH}/total_image.hex ${MK_GENERATED_PATH}/total_image.hexf

rm -fr temp/