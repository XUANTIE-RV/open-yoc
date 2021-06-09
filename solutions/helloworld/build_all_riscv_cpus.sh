#!/bin/bash

RISCV_CPU_LIST="e902 e906 e906f e906fd"

for cpu in $RISCV_CPU_LIST
do
    str="\ \ cpu_name: $cpu"
    sed -i "15c $str" ../../components/chip_riscv_dummy/package.yaml
    sed -i "14c \ \ arch_name: riscv" ../../components/chip_riscv_dummy/package.yaml
    sed -i "s/minilibc/newlib/g" ../../components/chip_riscv_dummy/package.yaml
    make clean;make
done
