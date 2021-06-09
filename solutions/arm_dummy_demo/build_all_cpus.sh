#!/bin/bash

ARM_CPU_LIST="cortex-m0 cortex-m3 cortex-m4"

for cpu in $ARM_CPU_LIST
do
    str="\ \ cpu_name: $cpu"
    sed -i "15c $str" ../../components/chip_arm_dummy/package.yaml
    sed -i "14c \ \ arch_name: arm" ../../components/chip_arm_dummy/package.yaml
    sed -i "s/minilibc/newlib/g" ../../components/chip_arm_dummy/package.yaml
    make clean;make
done
