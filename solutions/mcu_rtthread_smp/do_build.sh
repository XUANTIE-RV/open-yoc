#!/bin/bash
###attention: do_build.sh in demo mcu_freertos_xxx should keep same with solutions/mcu_freertos_helloworld/do_build.sh for xt-rtos sdk

solution_name=$(basename "$(pwd)")
if [ "$2" == 'wujian300' ]; then
	RULE_CONFIG_FILE="../../components/wujian300_soc_sdk/sdk.csv"
	SDK_NAME="wujian300_soc_sdk"
else
	RULE_CONFIG_FILE="../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv"
	SDK_NAME="xuantie_cpu_sdk"
fi
if [ ! -e $RULE_CONFIG_FILE ]; then
	echo "rule file: $RULE_CONFIG_FILE is not exist, $SDK_NAME component may be not installed yet!"
	exit 1
fi

function help() {
	echo "./do_build.sh cpu_name board"
	echo "eg:"
	echo "./do_build.sh c906fd xiaohui"
	echo "./do_build.sh e906fdp smartl"
	echo "./do_build.sh e906fdp wujian300"
}

function check_cpu() {
	cpu=$1
	while IFS=',' read -r col0 col1 col2 col3
	do
		if [ "$solution_name" == "$col0" ]; then
			_col1="${col1//\"/}"
			cpu_list=(${_col1//// })
			#echo ${cpu_list[@]}
			for _cpu in ${cpu_list[@]}; do
				if [ "$cpu" == "$_cpu" ]; then
					#echo "found"
					return 0
				fi
			done

			break
		fi
	done < $RULE_CONFIG_FILE

	echo "this solution is not support for the cpu, may be rule file is not update yet!"
	exit 1
}

function check_board() {
	board=$1
	board_list=('smartl' 'xiaohui' 'wujian300')
	#echo "the board is "$board
	for _board in ${board_list[*]}; do
		if [ "$board" == "$_board" ]; then
			return 0
		fi
	done

	echo "the board is not support for the cpu. optional boards are:"
	echo ${board_list[@]}
	exit 1
}

if [[ $# -lt 2 ]]; then
	help
	exit 1
fi

cpu_name=$1
board=$2
rtos=rtthread

check_cpu $cpu_name
check_board $board

if [ "$board" == 'wujian300' ]; then
	SDK_COMP=../../components/sdk_chip_wujian300
	CHIP_COMP=../../components/chip_wujian300
	BOARD_COMP=../../boards/board_wujian300_evb
	SDK_CHIP=sdk_chip_wujian300
else
	SDK_COMP=../../components/sdk_chip_riscv_dummy
	CHIP_COMP=../../components/chip_riscv_dummy
	BOARD_COMP=../../boards/board_riscv_dummy
	SDK_CHIP=sdk_chip_riscv_dummy
fi

cp $SDK_COMP/package.yaml $SDK_COMP/package.yaml.bak
cp $CHIP_COMP/package.yaml $CHIP_COMP/package.yaml.bak
cp $BOARD_COMP/package.yaml $BOARD_COMP/package.yaml.bak

cp $SDK_COMP/package.yaml.$rtos $SDK_COMP/package.yaml
cp $CHIP_COMP/package.yaml.$cpu_name $CHIP_COMP/package.yaml
cp $BOARD_COMP/package.yaml.$board $BOARD_COMP/package.yaml

#echo "===start to compile==="
make SDK=$SDK_CHIP || exit 1
#echo "===compile done!!!==="

mv $SDK_COMP/package.yaml.bak $SDK_COMP/package.yaml
mv $CHIP_COMP/package.yaml.bak $CHIP_COMP/package.yaml
mv $BOARD_COMP/package.yaml.bak $BOARD_COMP/package.yaml

