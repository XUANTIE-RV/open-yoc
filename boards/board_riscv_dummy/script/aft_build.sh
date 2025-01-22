#!/bin/sh
BASE_PWD=`pwd`

echo "[INFO] Generated output files ..."
echo $BASE_PWD

EXE_EXT=`which ls | grep -o .exe`
if [ -n "$EXE_EXT" ]; then
    if [ ! -d  "${SOLUTION_PATH}" ];then
        SOLUTION_PATH=$1
    fi
    if [ ! -d  "${BOARD_PATH}" ];then
        BOARD_PATH=$2
    fi
    if [ ! -d  "${CHIP_PATH}" ];then
        CHIP_PATH=$3
    fi
    PLATFORM=$4
    CPU=$5
    if [ -z "$IS_IN_CDS" ]; then
        echo "I am in CDK."
        OBJCOPY=riscv64-unknown-elf-objcopy
        ELF_NAME=`ls Obj/*.elf`
        $OBJCOPY -O binary $ELF_NAME $SOLUTION_PATH/yoc.bin
        cp $ELF_NAME $SOLUTION_PATH/yoc.elf
        cp -arf yoc.map $SOLUTION_PATH/yoc.map
    fi
    PRODUCT=$BOARD_PATH/configs/product$EXE_EXT
else
    echo "I am in Linux."
    while getopts ":s:b:c:u:a:" optname
    do
        case "$optname" in
        "s")
            SOLUTION_PATH=$OPTARG
            ;;
        "b")
            BOARD_PATH=$OPTARG
            ;;
        "c")
            CHIP_PATH=$OPTARG
            ;;
        "u")
            CPU=$OPTARG
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
    if [ "${CPU#e}" != "$CPU" ]; then
        PLATFORM='smartl'
    else
        PLATFORM='xiaohui'
    fi
fi

MK_GENERATED_PATH=${SOLUTION_PATH}/generated
rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/

# echo $SOLUTION_PATH
# echo $BOARD_PATH
# echo $CHIP_PATH
# echo $MK_GENERATED_PATH
# echo $PLATFORM
# echo $CPU

if [ ! -f $SOLUTION_PATH/gdbinitflash ]; then
    cp -arf $BOARD_PATH/script/gdbinitflash $SOLUTION_PATH
fi

if [ ! -f $SOLUTION_PATH/cdkinitflash ]; then
    cp -arf $BOARD_PATH/script/cdkinitflash $SOLUTION_PATH
fi

if [ ! -f $SOLUTION_PATH/gdbinit ]; then
    cp -arf $BOARD_PATH/script/gdbinit $SOLUTION_PATH
fi

if [ -z "$EXE_EXT" ]; then
    # linux
    cp -arf $BOARD_PATH/script/mkflash.sh $SOLUTION_PATH
    if [ ! -f $SOLUTION_PATH/gdbinit.$CPU ]; then
        cp -arf $BOARD_PATH/script/$PLATFORM/gdbinit.$CPU $SOLUTION_PATH/gdbinit.$CPU
    fi
else
    cp -arf $BOARD_PATH/script/$PLATFORM/gdbinit.$CPU ${ProjectPath}/gdbinit.remote
fi
