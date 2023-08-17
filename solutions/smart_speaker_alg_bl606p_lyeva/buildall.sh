#!/bin/sh

make clean
cp package_lyeva_asr.yaml package.yaml
make || exit

make clean
cp package_lyeva.yaml package.yaml
make || exit

make clean
cp package_mind.yaml package.yaml
make || exit

make clean
cp package_us.yaml package.yaml
make || exit

[ -d .git ] && git clean -dxf
[ -d .git ] && git reset --hard
