#!/bin/sh
SDK_NAME=yoc_v7.2.2
SDK_ROOT=../../$SDK_NAME
YOC_ROOT=..

mkdir -p $SDK_ROOT
mkdir -p $SDK_ROOT/components
mkdir -p $SDK_ROOT/boards/silan
mkdir -p $SDK_ROOT/solutions_cb5654

echo ">>>>Main comp"
comps=`cat smart_speaker_demo/package.yaml | sed 's/[[:space:]]//g' | grep "V7.2.2" | awk -F'-' '{printf "%s\n", $2}' | awk -F':V7.2.2' '{printf "%s\n", $1}'`
for comp in $comps
do
    echo $comp
    cp -rf $YOC_ROOT/components/$comp $SDK_ROOT/components
done

# extern comp
echo ">>>>Ext comp"
comps="aliot yunio"
for comp in $comps
do
    echo $comp
    cp -rf $YOC_ROOT/components/$comp $SDK_ROOT/components
done


cp -rf $YOC_ROOT/boards/silan/cb5654 $SDK_ROOT/boards/silan/.
cp -rf $YOC_ROOT/boards/silan/mit_v2 $SDK_ROOT/boards/silan/.
cp -rf $YOC_ROOT/boards/silan/mit_v3 $SDK_ROOT/boards/silan/.
#cp -rf $YOC_ROOT/tools $SDK_ROOT/.
cp $YOC_ROOT/.yoc $SDK_ROOT/.
cp -rf smart_speaker_demo $SDK_ROOT/solutions_cb5654
cp -rf chip_example_sc5654 $SDK_ROOT/solutions_cb5654
cp -rf alimqtt_demo_sc5654 $SDK_ROOT/solutions_cb5654

#tar zcf yoc_v7.2.2.tar.gz $SDK_ROOT
cd $YOC_ROOT/../
zip -qr ${SDK_NAME}.zip ${SDK_NAME}
cd -

rm $SDK_ROOT -rf
