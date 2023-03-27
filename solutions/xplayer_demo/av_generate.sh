#!/bin/bash

components=(amrnb amrwb av flac ogg opus pvmp3dec sonic speex speexdsp)
#if [ $# -lt 1 ]; then
#    echo "./av_generate branch"
#    echo "    eg: ./av_generate develop"
#    exit 1
#fi
#
#rm av_lib -rf
#mkdir av_lib
#cd av_lib
#yoc init --aone
#yoc install xplayer_demo -b $1
#cd solutions/xplayer_demo

make clean
make SDK=sdk_chip_bl606p_e907 || exit 1
arch="e907fp"
for element in ${components[@]}
do
    echo "install prebuild library: $element"
    dir_path="../../components/"$element"/libs/"$arch
    mkdir -p $dir_path
    cp "yoc_sdk/lib/lib"$element".a" $dir_path"/lib"$element"_prebuild.a"
done

make clean
make SDK=sdk_chip_d1 || exit 1
arch="c906fdv"
for element in ${components[@]}
do
    echo "install prebuild library: $element"
    dir_path="../../components/"$element"/libs/"$arch
    mkdir -p $dir_path
    cp "yoc_sdk/lib/lib"$element".a" $dir_path"/lib"$element"_prebuild.a"
done


make clean
make SDK=sdk_chip_ch2601 || exit 1
arch="e906"
for element in ${components[@]}
do
    echo "install prebuild library: $element"
    dir_path="../../components/"$element"/libs/"$arch
    mkdir -p $dir_path
    cp "yoc_sdk/lib/lib"$element".a" $dir_path"/lib"$element"_prebuild.a"
done

echo "prebuild library is done"





