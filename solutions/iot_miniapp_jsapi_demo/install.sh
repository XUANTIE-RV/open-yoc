#!/bin/sh

INSTALL_DIR=../iot_miniapp_daemon/data/resources

rm -fr ${INSTALL_DIR}/presetpkgs/*.amr

APP_ID=`ls *.amr | awk -F'.' '{print $1}'`
cp ${APP_ID}.*.amr ${INSTALL_DIR}/presetpkgs/${APP_ID}.amr
cp local_packages.json  ${INSTALL_DIR}/
