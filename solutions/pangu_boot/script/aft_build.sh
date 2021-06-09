#!/bin/sh

ELF_NAME=`ls Obj/*.elf`
OBJCOPY=csky-elfabiv2-objcopy

${OBJCOPY} -O binary ${ELF_NAME} ./boot.bin


