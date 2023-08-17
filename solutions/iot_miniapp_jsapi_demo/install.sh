#!/bin/sh

INSTALL_DIR=../iot_miniapp_daemon/data/resources

#保留键盘，删除其他小程序
mv ${INSTALL_DIR}/presetpkgs/8001641807316750.amr ${INSTALL_DIR}/presetpkgs/8001641807316750.bak
rm -fr ${INSTALL_DIR}/presetpkgs/*.amr
mv ${INSTALL_DIR}/presetpkgs/8001641807316750.bak ${INSTALL_DIR}/presetpkgs/8001641807316750.amr

#复制到当前小程序到littlefs打包目录
APP_ID=`ls *.amr | awk -F'.' '{print $1}'`
cp ${APP_ID}.*.amr ${INSTALL_DIR}/presetpkgs/${APP_ID}.amr
cp local_packages.json  ${INSTALL_DIR}/
