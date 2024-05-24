#!/bin/sh

BASE_PWD=`pwd`
MK_BOARD_PATH=$BOARD_PATH
MK_CHIP_PATH=$CHIP_PATH
MK_SOLUTION_PATH=$SOLUTION_PATH

echo "[INFO] Generated output files ..."
echo $BASE_PWD

MOUNT_POINT="/"
READELF=riscv64-unknown-elf-readelf
EXE_EXT=`which ls | grep -o .exe`
if [ -n "$EXE_EXT" ]; then
    echo "I am in CDK."
    OBJCOPY=riscv64-unknown-elf-objcopy
    ELF_NAME=`ls Obj/*.elf`
    $OBJCOPY -O binary $ELF_NAME yoc.bin
    PRODUCT=$MK_BOARD_PATH/configs/product$EXE_EXT
    MOUNT_POINT="\\"
    cp $ELF_NAME yoc.elf
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
    PATH_IOT_MINIAPP_SDK=../../components/iot_miniapp_sdk
fi

MK_GENERATED_PATH=${MK_SOLUTION_PATH}/generated
rm -fr $MK_GENERATED_PATH
mkdir -p $MK_GENERATED_PATH/data/

echo $MK_SOLUTION_PATH
echo $MK_BOARD_PATH
echo $MK_CHIP_PATH
echo $MK_GENERATED_PATH

FS_DATA_DIR=${MK_SOLUTION_PATH}/data


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

if [ -d data ]; then
    LFS_SIZE=$(cat $MK_BOARD_PATH/configs/config.yaml | grep ext4 | sed 's/[[:space:]\"]//g' | awk -F 'size:' '{print $2}' | awk -F '}' '{print strtonum($1)}')
    echo "ext4 size is ${LFS_SIZE}"
    dd if=/dev/zero of=${MK_GENERATED_PATH}/data/ext4.img bs=${LFS_SIZE} count=1
    $PRODUCT ext4 -i ${MK_GENERATED_PATH}/data/ext4.img -e 4 -d data -m $MOUNT_POINT -b 1024 -p
fi

if [ -d bin ]; then
	cp -arf bin/*  ${MK_GENERATED_PATH}/data/
fi

[ -f "${MK_BOARD_PATH}/bootimgs/fip.bin" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/fip.bin ${MK_GENERATED_PATH}/data/
[ -f "${MK_BOARD_PATH}/bootimgs/fip_nor.bin" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/fip_nor.bin ${MK_GENERATED_PATH}/data/
[ -f "${MK_BOARD_PATH}/bootimgs/fip_nand.bin" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/fip_nand.bin ${MK_GENERATED_PATH}/data/
[ -f "${MK_BOARD_PATH}/bootimgs/boot" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/boot ${MK_GENERATED_PATH}/data/
[ -f "${MK_BOARD_PATH}/bootimgs/boot.nand" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/boot.nand ${MK_GENERATED_PATH}/data/
[ -f "${MK_BOARD_PATH}/bootimgs/boot0" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/boot0 ${MK_GENERATED_PATH}/data/
[ -f "${MK_BOARD_PATH}/bootimgs/boot0.emmc" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/boot0.emmc ${MK_GENERATED_PATH}/data/
[ -f "${MK_BOARD_PATH}/bootimgs/boot0.nand" ] && cp -arf ${MK_BOARD_PATH}/bootimgs/boot0.nand ${MK_GENERATED_PATH}/data/
[ -f $MK_BOARD_PATH/bootimgs/kp ] && cp $MK_BOARD_PATH/bootimgs/kp ${MK_GENERATED_PATH}/data

if [ -z "$EXE_EXT" ]; then
bin_text=$($READELF -S yoc.elf | grep "\.text" | awk '{size="0x"$5; print size}'  | xargs printf %d)
#readelf error happens sometime, we don't know the reason. error info like this:
#readelf: Error: Reading 1408 bytes extends past end of file for section headers
if [ $bin_text -eq 0 ];then
	echo "readelf error happens, get bin_text variable again!!!"
	sleep 1
	bin_text=$($READELF -S yoc.elf | grep "\.text" | awk '{size="0x"$5; print size}'  | xargs printf %d)
fi
bin_post_text=$($READELF -S yoc.elf | grep "\.post_text" | awk '{size="0x"$5; print size}'  | xargs printf %d)
bin_bss=$($READELF -S yoc.elf | grep "\.bss" | awk '{size="0x"$5; print size}'  | xargs printf %d)
echo $bin_text" : "$bin_post_text" : "$bin_bss
preload_size=$(($bin_post_text-$bin_text))
postload_size=$(($bin_bss-$bin_post_text))
if [ $preload_size -gt 0 ] && [ $postload_size -gt 0 ];then
	[ -f yoc.bin ] && cp -arf yoc.bin ${MK_GENERATED_PATH}/data/prim
	total_size=$(ls -l yoc.bin | awk '{print $5}')
	preload_per=$(echo "scale=3;$preload_size/$total_size" | bc)
	echo "need preload support, preload_size/total_size<=>"$preload_size"/"$total_size" = 0"$preload_per
	preload_size=$(printf %x $preload_size)
	sed 's#prim,#prim, preload_size: 0x'$preload_size',#g' ${MK_BOARD_PATH}/configs/config.yaml > ${MK_GENERATED_PATH}/data/config.yaml
else
	$PRODUCT combine -i yoc.bin,0x80040000,lz4 -o ${MK_GENERATED_PATH}/data/prim
	cp -arf ${MK_BOARD_PATH}/configs/config.yaml ${MK_GENERATED_PATH}/data/
fi
else
	$PRODUCT combine -i yoc.bin,0x80040000,lz4 -o ${MK_GENERATED_PATH}/data/prim
	cp -arf ${MK_BOARD_PATH}/configs/config.yaml ${MK_GENERATED_PATH}/data/
fi

#[ -f yoc.bin ] && cp -arf yoc.bin ${MK_GENERATED_PATH}/data/prim
#$PRODUCT combine -i yoc.bin,0x80040000,lz4 -o ${MK_GENERATED_PATH}/data/prim

$PRODUCT image ${MK_GENERATED_PATH}/images.zip -i ${MK_GENERATED_PATH}/data  -p
$PRODUCT image ${MK_GENERATED_PATH}/images.zip -v "1.0" -spk $MK_BOARD_PATH/keystore/key.pem -dt SHA256 -st RSA2048
$PRODUCT image ${MK_GENERATED_PATH}/images.zip -e ${MK_GENERATED_PATH}
$PRODUCT image ${MK_GENERATED_PATH}/images.zip -e ${MK_GENERATED_PATH} -kp -x


#fota image
echo "fota image generate..."
# [ -f $MK_GENERATED_PATH/prima ] && cp $MK_GENERATED_PATH/prima $MK_GENERATED_PATH/prim.bin
# $PRODUCT pack -i ${MK_GENERATED_PATH}/prim.bin -o ${MK_GENERATED_PATH}/fota.raw -hv 2 -spk $MK_BOARD_PATH/keystore/key.pem -dt SHA256 -st RSA2048
$PRODUCT diff -f ${MK_GENERATED_PATH}/images.zip ${MK_GENERATED_PATH}/images.zip -r -v "1.1" -spk ${MK_BOARD_PATH}/keystore/key.pem -dt SHA256 -st RSA2048 -o ${MK_GENERATED_PATH}/fota.raw

if [ ! -f gdbinitflash ]; then
    cp -arf $MK_BOARD_PATH/script/gdbinitflash $BASE_PWD
fi

cp -arf $MK_BOARD_PATH/script/cdkinitflash $BASE_PWD

if [ ! -f gdbinit ]; then
    cp -arf $MK_BOARD_PATH/script/gdbinit $BASE_PWD
fi

cp -arf $MK_BOARD_PATH/script/mkflash.sh $BASE_PWD
