#!/bin/sh

#SLN_LIST="smart_speaker_demo"
SLN_LIST=`find . -maxdepth 1 -mindepth 1 -type d ! -path ./.cdk`

#Packages
echo ">>> Solution Zip" ${SLN} "<<<"
rm -fr *.zip

for SLN in ${SLN_LIST}; do
    echo ${SLN} 
    cd  ${SLN}
    rm -fr Chips Boards Packages Obj Lst
    cd - > /dev/null
    zip -qr ${SLN}.zip ${SLN}
done

for SLN in ${SLN_LIST}; do
    echo ">>> Solution CDK" ${SLN} "<<<"
    cd  ${SLN}
    sh ../../utility/cdk/create_sln_dep.sh
    cd - > /dev/null
done

