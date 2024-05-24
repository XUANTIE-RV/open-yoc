#!/bin/bash

CHIP_COMP=../../components/chip_cv181x

cp $CHIP_COMP/package.yaml $CHIP_COMP/package.yaml.bak
cp $CHIP_COMP/package.yaml.boot $CHIP_COMP/package.yaml

if [[ $1 = "nand" ]];then
    echo "make for nand flash"
    cp package.yaml package.yaml.bak -arf
    cp package.yaml.nand package.yaml -arf
    make;
    mv package.yaml.bak package.yaml
else
    make;
fi

cp $CHIP_COMP/package.yaml.bak $CHIP_COMP/package.yaml

echo "make over"

