#!/bin/sh

SLN_LIST=`find . -maxdepth 1 -mindepth 1 -type d ! -path ./.cdk`

#Packages
for SLN in ${SLN_LIST}; do
    echo ">>> Solution" ${SLN} "<<<"
    cd  ${SLN}
    sh ../../utility/cdk/create_sln_dep.sh
    cd -
done
