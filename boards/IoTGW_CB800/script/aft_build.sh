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
    WM_TOOL=${BOARD_PATH}/configs/wm_tool.exe
    rm -fr wm_tmp
    mkdir -p wm_tmp
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

    rm -fr wm_tmp
    mkdir -p wm_tmp
    gcc ../../components/chip_w800/wmsdk/tools/w800/wm_tool.c -Wall -lpthread -O2 -o ./wm_tmp/wm_tool
    cp ../../components/chip_w800/wmsdk/tools/w800/wm_tool ./wm_tmp/wm_tool

    WM_TOOL=./wm_tmp/wm_tool
    chmod +x ${WM_TOOL}

    ELF_NAME=`ls *.elf`
  
    READELF=csky-abiv2-elf-readelf

    OBJCOPY=csky-abiv2-elf-objcopy
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
COMPRESS_FLAG=$(cat ${MK_BOARD_PATH}/package.yaml | grep -w 'CONFIG_BOOT_COMPRESS' | sed 's/[[:space:]\"]//g' | awk -F":" '{print $2}')
fi

echo "Boot compress: $COMPRESS_FLAG"

if  [ "${COMPRESS_FLAG}" = '' ];then
COMPRESS_FLAG='1'
fi
echo "Boot compress: $COMPRESS_FLAG"
if [ "${COMPRESS_FLAG}" = '0' ];then
cp ${MK_BOARD_PATH}/configs/config.yaml ${MK_GENERATED_PATH}/data/config.yaml
cp ${MK_BOARD_PATH}/bootimgs/boot ${MK_GENERATED_PATH}/data/
if [ -f "${MK_BOARD_PATH}/bootimgs/bt" ] ;then
cp ${MK_BOARD_PATH}/bootimgs/bt ${MK_GENERATED_PATH}/data/
fi
if [ -f "${MK_BOARD_PATH}/bootimgs/fcds" ] ;then
cp ${MK_BOARD_PATH}/bootimgs/fcds ${MK_GENERATED_PATH}/data/
fi
else
cp ${MK_BOARD_PATH}/configs/config_compress.yaml ${MK_GENERATED_PATH}/data/config.yaml
cp ${MK_BOARD_PATH}/bootimgs_compress/boot ${MK_GENERATED_PATH}/data/
if [ -f "${MK_BOARD_PATH}/bootimgs_compress/bt" ] ;then
cp ${MK_BOARD_PATH}/bootimgs_compress/bt ${MK_GENERATED_PATH}/data/
fi
if [ -f "${MK_BOARD_PATH}/bootimgs_compress/fcds" ] ;then
cp ${MK_BOARD_PATH}/bootimgs_compress/fcds ${MK_GENERATED_PATH}/data/
fi
fi

echo "[INFO] Generated output files ..."

${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_PATH}/data/prim

${PRODUCT} image ${MK_GENERATED_PATH}/images.zip -i ${MK_GENERATED_PATH}/data -l -v "${APP_VERSION}" -p
${PRODUCT} image ${MK_GENERATED_PATH}/images.zip -e ${MK_GENERATED_PATH} -x
${PRODUCT} image ${MK_GENERATED_PATH}/images.zip -e ${MK_GENERATED_PATH} -tb
${PRODUCT} diff -f ${MK_GENERATED_PATH}/images.zip ${MK_GENERATED_PATH}/images.zip -r -v "${APP_VERSION}" -cp 2 -o ${MK_GENERATED_PATH}/fota.bin

echo "Create wm bin files"

cp ${MK_GENERATED_PATH}/data/boot wm_tmp/boot.bin
cp ${MK_GENERATED_PATH}/data/prim wm_tmp/yoc.bin
cp ${MK_GENERATED_PATH}/data/prim wm_tmp/prim.bin
cp ${MK_GENERATED_PATH}/imtb wm_tmp/imtb.bin

${WM_TOOL} -b ./wm_tmp/yoc.bin -fc 0 -it 1 -ih 8132C00 -ra 8013000 -ua 8133000 -nh 0 -un 0 -vs yoc-7.3.0
cat ../../components/chip_w800/wmsdk/tools/w800/w800_secboot.img ./wm_tmp/yoc.img > ${MK_GENERATED_PATH}/wm_yoc.fls
${WM_TOOL} -b ./wm_tmp/yoc.img -fc 1 -it 1 -ih 8132C00 -ra 8013000 -ua 8133000 -nh 0 -un 0 -vs yoc-7.3.0
mv ./wm_tmp/yoc_gz.img ${MK_GENERATED_PATH}/wm_yoc_ota.bin

echo [INFO] Create fls file

${WM_TOOL} -b ./wm_tmp/boot.bin -fc 0 -it 512 -ih 8002000 -ra 8002400 -ua 8133000 -nh 8132C00 -un 0
${WM_TOOL} -b ./wm_tmp/prim.bin -fc 0 -it 1 -ih 8132C00 -ra 8013000 -ua 8133000 -nh 0 -un 0
${WM_TOOL} -b ./wm_tmp/imtb.bin -fc 0 -it 1 -ih 20047400 -ra 800F000 -ua 0 -nh 0 -un 0
cat ./wm_tmp/boot.img ./wm_tmp/prim.img ./wm_tmp/imtb.img > ${MK_GENERATED_PATH}/wm_yoc_total.fls
rm -fr wm_tmp
if [ -f "${BOARD_PATH}/bootimgs/bt" ]; then cat ${MK_GENERATED_PATH}/wm_yoc_ota.bin ${MK_GENERATED_PATH}/data/bt > ${MK_GENERATED_PATH}/wm_total_fota.bin
else
cp ${MK_GENERATED_PATH}/wm_yoc_ota.bin ${MK_GENERATED_PATH}/wm_total_fota.bin
fi
