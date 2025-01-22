
#!/bin/sh

echo "I am in CDS post build."
echo `pwd`

OBJCOPY=$5
OBJDUMP=$6
ELF_NAME="${PWD}/$1.elf"
MAP_NAME="${PWD}/yoc.map"
SOLUTION_PATH=$2
BOARD_PATH=$3
CHIP_PATH=$4
PLATFORM=xiaohui
CPU=c920v3

# echo $ELF_NAME
# echo $SOLUTION_PATH
# echo $BOARD_PATH
# echo $CHIP_PATH
# echo $PLATFORM
# echo $CPU
# echo $OBJCOPY
# echo $OBJDUMP

$OBJCOPY -O binary $ELF_NAME ${SOLUTION_PATH}/yoc.bin
$OBJDUMP -d $ELF_NAME > ${SOLUTION_PATH}/yoc.asm
cp -arf $ELF_NAME ${SOLUTION_PATH}/yoc.elf
cp -arf $MAP_NAME ${SOLUTION_PATH}/yoc.map

export SOLUTION_PATH=$SOLUTION_PATH
export BOARD_PATH=$BOARD_PATH
export CHIP_PATH=$CHIP_PATH
export IS_IN_CDS=1

cp -arf $BOARD_PATH/script/$PLATFORM/gdbinit.$CPU ../gdbinit.remote

if [ -f "$BOARD_PATH/script/aft_build.sh" ];then
	cd $SOLUTION_PATH
	sh $BOARD_PATH/script/aft_build.sh $SOLUTION_PATH $BOARD_PATH $CHIP_PATH $PLATFORM $CPU $OBJCOPY $OBJDUMP
fi
