#!/bin/bash

MK_GENERATED_IMGS_PATH=generated

if [ "${CDK_VERSION}" != '' ];then
ELF_NAME=`ls Obj/*.elf`
OBJCOPY=csky-elfabiv2-objcopy
READELF=csky-elfabiv2-readelf
${OBJCOPY} -O binary ${ELF_NAME} ${MK_GENERATED_IMGS_PATH}/data/prim
PRODUCT_BIN=cdk/script/product.exe
else
PRODUCT_BIN=${MK_GENERATED_IMGS_PATH}/product
chmod +x ${PRODUCT_BIN}
READELF=csky-abiv2-elf-readelf
ELF_NAME=`ls *.elf`
fi

${READELF} -S ${ELF_NAME}

echo "[INFO] Generated output files ..."

${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -i ${MK_GENERATED_IMGS_PATH}/data -l -v "v1.0" -p
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -v "v1.0" -spk ${MK_GENERATED_IMGS_PATH}/aes_128_ccm.key -dt SHA1 -st AES_128_CCM -iv 101112131415161718191a1b -aad 000102030405060708090a0b0c0d0e0f10111213 -tlen 16
${PRODUCT_BIN} image ${MK_GENERATED_IMGS_PATH}/images.zip -e ${MK_GENERATED_IMGS_PATH} -x
${PRODUCT_BIN} diff -f ${MK_GENERATED_IMGS_PATH}/images.zip ${MK_GENERATED_IMGS_PATH}/images.zip -r -v "v1.0" -spk ${MK_GENERATED_IMGS_PATH}/aes_128_ccm.key -dt SHA1 -st AES_128_CCM -iv 101112131415161718191a1b -aad 000102030405060708090a0b0c0d0e0f10111213 -tlen 16 -o ${MK_GENERATED_IMGS_PATH}/fota.bin

cp ${MK_GENERATED_IMGS_PATH}/total_image.hex ${MK_GENERATED_IMGS_PATH}/total_image.hexf