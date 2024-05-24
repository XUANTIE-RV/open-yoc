#!/bin/bash

function help() {
	echo "eg: ./xtsdk_public.sh cpu_name"
	echo "like: ./xtsdk_public.sh e906fdp"
}

if [[ $# -lt 1 ]]; then
	help
	exit 1
fi

cpu_name=$1

if [[ $cpu_name == e* ]]; then
	cp package.yaml.smartl package.yaml
else
	cp package.yaml.xiaohui package.yaml
fi

