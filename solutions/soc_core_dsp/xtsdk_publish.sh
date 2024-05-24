#!/bin/bash

solution_name=$(basename "$(pwd)")
RULE_CONFIG_FILE="../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv"
if [ ! -e $RULE_CONFIG_FILE ]; then
	echo "rule file: $RULE_CONFIG_FILE is not exist, xuantie_cpu_sdk component may be not installed yet!"
	exit 1
fi

function generate_cds() {
	if [[ $cpu_name = "e"* ]]; then
		yoc cds $cpu_name smartl freertos
		yoc cds $cpu_name smartl rtthread
	else
		yoc cds $cpu_name xiaohui freertos
		yoc cds $cpu_name xiaohui rtthread
	fi
}

function help() {
	echo "eg: ./xtsdk_public.sh cpu_name"
	echo "like: ./xtsdk_public.sh e906fdp"
}

if [[ $# -lt 1 ]]; then
	help
	exit 1
fi

cpu_name=$1

cp package.yaml.$cpu_name package.yaml

generate_cds

