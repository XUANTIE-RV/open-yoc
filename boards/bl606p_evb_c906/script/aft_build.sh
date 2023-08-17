#!/bin/sh

BASE_PWD=`pwd`
MK_BOARD_PATH=$BOARD_PATH
MK_CHIP_PATH=$CHIP_PATH
MK_SOLUTION_PATH=$SOLUTION_PATH
CONFIG_DEFINES=""

echo "[INFO] Generated output files ..."
echo $BASE_PWD

EXE_EXT=`which ls | grep -o .exe`
if [ -n "$EXE_EXT" ]; then
    echo "I am in CDK."
    OBJCOPY=riscv64-unknown-elf-objcopy
    ELF_NAME=`ls Obj/*.elf`
    $OBJCOPY -O binary $ELF_NAME yoc.bin
    cp Lst/*.asm yoc.asm
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
fi

echo $MK_SOLUTION_PATH
echo $MK_BOARD_PATH
echo $MK_CHIP_PATH

# # yoctools >= 2.0.44
# check components alg_asr_lyeva
COMP_ALG_ASR_PATH=$(echo $CONFIG_DEFINES | grep "PATH_ALG_ASR_LYEVA" | awk -F ':' '{print $2}')
#echo $CONFIG_DEFINES
if [ -n "$EXE_EXT" ]; then
    # CDK
    COMP_ALG_ASR_PATH=$PATH_ALG_ASR_LYEVA
fi

COMP_ALG_KWS_MIND_PATH=$(echo $CONFIG_DEFINES | grep "PATH_ALG_KWS_MIND" | awk -F ':' '{print $2}')
#echo $CONFIG_DEFINES
if [ -n "$EXE_EXT" ]; then
    # CDK
    COMP_ALG_KWS_MIND_PATH=$PATH_ALG_KWS_MIND
fi

COMP_ALG_KWS_LYEVA_PATH=$(echo $CONFIG_DEFINES | grep "PATH_ALG_KWS_LYEVA" | awk -F ':' '{print $2}')
#echo $CONFIG_DEFINES
if [ -n "$EXE_EXT" ]; then
    # CDK
    COMP_ALG_KWS_LYEVA_PATH=$PATH_ALG_KWS_LYEVA
fi

COMP_US_ALGO_C906_PATH=$(echo $CONFIG_DEFINES | grep "PATH_US_ALGO_C906" | awk -F ':' '{print $2}')
#echo $CONFIG_DEFINES
if [ -n "$EXE_EXT" ]; then
    # CDK
    COMP_US_ALGO_C906_PATH=$PATH_US_ALGO_C906
fi

if [ -n "$COMP_ALG_KWS_MIND_PATH" ]; then

# mind kws + lyeva asr 
if [ -n "$COMP_ALG_ASR_PATH" ]; then
    FIRMWARE_NAME="c906fdv_fw_asr_lyeva"
    FIRMWARE_PATH="../../components/alg_asr_lyeva/libs/chip_bl606p/e907fp/firmware/"
else
# mind kws only
    FIRMWARE_NAME="c906fdv_fw_mind"
    FIRMWARE_PATH="../../components/alg_kws_mind/libs/chip_bl606p/e907fp/firmware/"
fi

else

echo $COMP_ALG_KWS_LYEVA_PATH

if [ -n "$COMP_ALG_KWS_LYEVA_PATH" ]; then
# lyeva kws
    FIRMWARE_NAME="c906fdv_fw_lyeva"
    FIRMWARE_PATH="../../components/alg_kws_lyeva/libs/chip_bl606p/e907fp/firmware/"
elif [ -n "$COMP_US_ALGO_C906_PATH" ]; then
# us algo
    FIRMWARE_NAME="c906fdv_fw_us"
    FIRMWARE_PATH="../../components/us_algo_c906/libs/chip_bl606p/e907fp/firmware/"
else
# default
    echo "default to boards/bl606p_evb_e907/bootimgs"
    FIRMWARE_PATH="../../boards/bl606p_evb_e907/bootimgs"
    FIRMWARE_NAME="c906"
fi

fi

if [ ! -d "$FIRMWARE_PATH" ];then
    mkdir -p $FIRMWARE_PATH
fi

if [ -n "${FIRMWARE_NAME}" ]; then

cp yoc.elf ${FIRMWARE_PATH}/${FIRMWARE_NAME}.elf
cp yoc.bin ${FIRMWARE_PATH}/${FIRMWARE_NAME}.bin
head -n 20 yoc.asm > ${FIRMWARE_PATH}/${FIRMWARE_NAME}.asm

echo "copy ${FIRMWARE_NAME} to ${FIRMWARE_PATH} ok"

else

echo "Generate fail"

fi