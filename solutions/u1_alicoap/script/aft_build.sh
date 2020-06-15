#!/bin/sh

#Parse from yaml
csky-elfabiv2-objcopy -O  binary  Obj/ck.elf ck.bin

#Cleanup
#rm -fr ${MK_GENERATED_IMGS_PATH}/data
