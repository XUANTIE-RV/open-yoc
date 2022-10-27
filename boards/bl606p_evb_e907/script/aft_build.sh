#!/bin/sh

BASE_PWD=`pwd`
MK_BOARD_PATH=$BOARD_PATH
MK_CHIP_PATH=$CHIP_PATH
MK_SOLUTION_PATH=$SOLUTION_PATH

echo "[INFO] Generated output files ..."
echo $BASE_PWD

EXE_EXT=`which ls | grep -o .exe`
if [ -n "$EXE_EXT" ]; then
    echo "I am in CDK."
    OBJCOPY=riscv64-unknown-elf-objcopy
    ELF_NAME=`ls Obj/*.elf`
    $OBJCOPY -O binary $ELF_NAME yoc.bin
    PRODUCT=$MK_BOARD_PATH/configs/product$EXE_EXT
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
    PRODUCT=product
fi

MK_GENERATED_PATH=${MK_SOLUTION_PATH}/generated
rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/

echo $MK_SOLUTION_PATH
echo $MK_BOARD_PATH
echo $MK_CHIP_PATH
echo $MK_GENERATED_PATH

if [ -d data ]; then
    LFS_SIZE=0
    if [ -n "$EXE_EXT" ]; then
        LFS_SIZE=$(cat $MK_BOARD_PATH/configs/config.yaml | grep lfs | sed 's/[[:space:]\"]//g' | awk -F 'size:' '{print $2}' | awk -F '}' '{print $1}')
    else
        LFS_SIZE=$(cat $MK_BOARD_PATH/configs/config.yaml | grep lfs | sed 's/[[:space:]\"]//g' | awk -F 'size:|}' '{print $2}' | xargs printf "%d\n")
    fi
	$PRODUCT lfs ${MK_GENERATED_PATH}/data/lfs -c data -b 4096 -s ${LFS_SIZE}
	cp -arf ${MK_GENERATED_PATH}/data/lfs  ${MK_GENERATED_PATH}/littlefs.bin
fi

cp -arf $MK_BOARD_PATH/configs/config.yaml ${MK_GENERATED_PATH}/data
cp -arf $MK_BOARD_PATH/configs/*.toml ${MK_GENERATED_PATH}/data
cp -arf $MK_BOARD_PATH/bootimgs/bootmini.bin ${MK_GENERATED_PATH}/data
cp -arf $MK_BOARD_PATH/bootimgs/boot.bin ${MK_GENERATED_PATH}/data
[ -f $MK_BOARD_PATH/bootimgs/kp ] && cp $MK_BOARD_PATH/bootimgs/kp ${MK_GENERATED_PATH}/data
[ -f $MK_BOARD_PATH/bootimgs/kp ] && cp $MK_BOARD_PATH/bootimgs/kp ${MK_GENERATED_PATH}/kp.bin
cp -arf yoc.bin ${MK_GENERATED_PATH}/data/prim
$PRODUCT image ${MK_GENERATED_PATH}/images.zip -i ${MK_GENERATED_PATH}/data  -p
$PRODUCT image ${MK_GENERATED_PATH}/images.zip -v "1.0" -spk $MK_BOARD_PATH/keystore/key.pem -dt SHA1 -st RSA1024
$PRODUCT image ${MK_GENERATED_PATH}/images.zip -e ${MK_GENERATED_PATH}
$PRODUCT image ${MK_GENERATED_PATH}/images.zip -e ${MK_GENERATED_PATH} -x
cp -arf ${MK_GENERATED_PATH}/data/*.toml ${MK_GENERATED_PATH}
cp -arf ${MK_GENERATED_PATH}/data/config.yaml ${MK_GENERATED_PATH}
cp -arf ${MK_GENERATED_PATH}/bootmini ${MK_GENERATED_PATH}/bootmini.bin
[ -f ${MK_BOARD_PATH}/bootimgs/boot2.bin ] && cp ${MK_BOARD_PATH}/bootimgs/boot2.bin ${MK_GENERATED_PATH}
cp -arf ${MK_GENERATED_PATH}/boot ${MK_GENERATED_PATH}/boot.bin
cp -arf ${MK_GENERATED_PATH}/imtb ${MK_GENERATED_PATH}/imtb.bin
cp -arf ${MK_GENERATED_PATH}/prima ${MK_GENERATED_PATH}/prim.bin

#fota image
echo "fota image generate..."
$PRODUCT pack -i ${MK_GENERATED_PATH}/prim.bin -o ${MK_GENERATED_PATH}/fota.raw -hv 2 -spk $MK_BOARD_PATH/keystore/key.pem -dt SHA1 -st RSA1024

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
