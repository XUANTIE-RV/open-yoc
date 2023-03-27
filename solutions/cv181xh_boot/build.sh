#!/bin/bash

CHIP_COMP=../../components/chip_cv181x

cp $CHIP_COMP/package.yaml $CHIP_COMP/package.yaml.bak
cp $CHIP_COMP/package.yaml.boot $CHIP_COMP/package.yaml

make

cp $CHIP_COMP/package.yaml.bak $CHIP_COMP/package.yaml

echo "make over"

