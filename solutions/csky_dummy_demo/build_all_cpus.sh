#!/bin/bash

CSKY_CPU_LIST="ck802 ck803 ck804f ck804ef ck805ef"

for cpu in $CSKY_CPU_LIST
do
    str="\ \ cpu_name: $cpu"
    sed -i "15c $str" ../../components/chip_csky_dummy/package.yaml
    sed -i "14c \ \ arch_name: csky" ../../components/chip_csky_dummy/package.yaml
    sed -i "s/newlib/minilibc/g" ../../components/chip_csky_dummy/package.yaml
    make clean;make
    sleep 1
done
