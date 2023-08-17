#!/bin/sh

INSTALL_DIR=../iot_miniapp_daemon/data/resources

rm ${INSTALL_DIR}/presetpkgs/*.amr
cp 8001670229526062.1_0_0.amr ${INSTALL_DIR}/presetpkgs/8001670229526062.amr
cp local_packages.json  ${INSTALL_DIR}/
