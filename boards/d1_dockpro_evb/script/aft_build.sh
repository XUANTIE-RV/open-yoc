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
            CONFIG_DEFINES=$OPTARG
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
    PATH_IOT_MINIAPP_SDK=../../components/iot_miniapp_sdk
fi

MK_GENERATED_PATH=${MK_SOLUTION_PATH}/generated
rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/

echo $MK_SOLUTION_PATH
echo $MK_BOARD_PATH
echo $MK_CHIP_PATH
echo $MK_GENERATED_PATH

#Prepare bin
BOARDS_CONFIG_PATH=$MK_BOARD_PATH/configs
BOOT0_BIN="${MK_BOARD_PATH}/bootimgs/boot0"
BOOT_BIN="${MK_BOARD_PATH}/bootimgs/boot"
CONFIG_YAML="${MK_BOARD_PATH}/configs/config.yaml"
KP_BIN="${MK_BOARD_PATH}/bootimgs/kp"

# # yoctools >= 2.0.44
# check components alg_asr_lyeva

COMP_ALG_ASR_PATH=$(echo $CONFIG_DEFINES | grep "PATH_ALG_ASR_LYEVA" | awk -F ':' '{print $2}')
#echo $CONFIG_DEFINES
if [ -n "$EXE_EXT" ]; then
    # CDK
    COMP_ALG_ASR_PATH=$PATH_ALG_ASR_LYEVA
fi

if [ -n "$COMP_ALG_ASR_PATH" ];then
echo $COMP_ALG_ASR_PATH
CONFIG_YAML="${MK_BOARD_PATH}/configs/config_lyeva_asr.yaml"
fi

cp ${BOOT0_BIN} $MK_GENERATED_PATH/data
cp ${BOOT_BIN} $MK_GENERATED_PATH/data
cp ${CONFIG_YAML} $MK_GENERATED_PATH/data/config.yaml
echo $CONFIG_YAML
[ -f ${KP_BIN} ] && cp ${KP_BIN} $MK_GENERATED_PATH/data/

BOARD_DIR=$MK_BOARD_PATH
OUT_DIR=${MK_SOLUTION_PATH}/out
RTOS_IMG=${MK_SOLUTION_PATH}/yoc.bin
FS_DATA_DIR=${MK_SOLUTION_PATH}/data
MK_GENERATED_IMGS_PATH=${MK_SOLUTION_PATH}/generated
CONFIG_YAML_FILE=${CONFIG_YAML}

if [ ! -d $OUT_DIR ]; then
    mkdir $OUT_DIR
fi

if [ ! -d "${MK_GENERATED_IMGS_PATH}/data" ]; then
    echo "folder ${MK_GENERATED_IMGS_PATH}/data not find."
    exit 1
fi

# copy haasui resources
if [ -f "$FS_DATA_DIR/resources/local_packages.json" ]; then
    echo "update ${FS_DATA_DIR}/resources from ${PATH_IOT_MINIAPP_SDK}"
    mkdir -p ${MK_SOLUTION_PATH}/.data/resources
    cp ${PATH_IOT_MINIAPP_SDK}/resources/* ${MK_SOLUTION_PATH}/.data/resources -arf
    if [ -d "${FS_DATA_DIR}/resources" ]; then
        cp ${FS_DATA_DIR}/resources/* ${MK_SOLUTION_PATH}/.data/resources -arf
        rm -rf ${FS_DATA_DIR}/resources
    else
        mkdir -p ${FS_DATA_DIR}/resources
    fi
    cp ${MK_SOLUTION_PATH}/.data/resources ${FS_DATA_DIR}/resources -arf
    rm -rf ${MK_SOLUTION_PATH}/.data
fi

# # copy wifi firmware
# CHIP_D1_DIR=$PATH_CHIP_D1
# echo "update ${FS_DATA_DIR}/mnt/D/data/firmware from $CHIP_D1_DIR"
# mkdir -p ${FS_DATA_DIR}/mnt/D/data/firmware
# cp ${CHIP_D1_DIR}/firmware/* ${FS_DATA_DIR}/mnt/D/data/firmware/ -arf
if [ -n "$EXE_EXT" ]; then
    $BOARD_DIR\\pack\\pack.exe -d 0 -b ${BOARD_DIR} -r ${RTOS_IMG} -f ${FS_DATA_DIR} -o ${OUT_DIR} -m ${MK_GENERATED_IMGS_PATH} -c ${CONFIG_YAML_FILE}
else
    # FACTORY_ZIP=${MK_SOLUTION_PATH}/$1
    # python ${BOARD_DIR}/pack/pack.py -d 0 -r ${RTOS_IMG} -f ${FS_DATA_DIR} -o ${OUT_DIR} -m ${MK_GENERATED_IMGS_PATH} -c ${CONFIG_YAML_FILE} -z ${FACTORY_ZIP}
    python ${BOARD_DIR}/pack/pack.py -d 0 -b ${BOARD_DIR} -r ${RTOS_IMG} -f ${FS_DATA_DIR} -o ${OUT_DIR} -m ${MK_GENERATED_IMGS_PATH} -c ${CONFIG_YAML_FILE}
fi

# fota image
$PRODUCT diff -f ${MK_GENERATED_IMGS_PATH}/images.zip ${MK_GENERATED_IMGS_PATH}/images.zip -r -v "1.1" -spk ${MK_BOARD_PATH}/keystore/key.pem -dt SHA1 -st RSA1024 -o ${MK_GENERATED_IMGS_PATH}/fota.bin

if [ ! -f gdbinitflash ]; then
    cp -arf $MK_BOARD_PATH/script/gdbinitflash $BASE_PWD
fi

cp -arf $MK_BOARD_PATH/script/cdkinitflash $BASE_PWD

if [ ! -f gdbinit ]; then
    cp -arf $MK_BOARD_PATH/script/gdbinit $BASE_PWD
fi

cp -arf $MK_BOARD_PATH/script/mkflash.sh $BASE_PWD

