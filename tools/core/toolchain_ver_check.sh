#!/bin/bash

IS_V210XX_TOOLCHAIN=0
if riscv64-unknown-elf-gcc -v 2>&1 | grep -q "Xuantie-900 elf newlib gcc Toolchain V2.10"; then
	IS_V210XX_TOOLCHAIN=1
fi
echo $IS_V210XX_TOOLCHAIN
