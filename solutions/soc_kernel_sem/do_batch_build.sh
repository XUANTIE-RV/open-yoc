#!/bin/bash

cpu_rxx=('r910')
cpu_exx=('e906' 'e906fd' 'e906fdv')
cpu_cxx=('c906' 'c906fd' 'c906fdv' 'c908' 'c908v' 'c908i' 'c910' 'c910v2' 'c920' 'c920v2')
#cpu_list=(${cpu_exx[@]} ${cpu_cxx[@]} ${cpu_rxx[@]})
cpu_list=('c906fd' 'c908' 'c908v' 'c910v2')

#rtos_list=('rhino' 'freertos' 'rtthread' 'ucos3')
rtos_list=('rhino')

echo "===start to compile==="
for _rtos in ${rtos_list[*]}; do
	for _cpu in ${cpu_list[*]}; do
		echo "./do_build.sh "$_cpu" xiaohui_evb "$_rtos
		./do_build.sh $_cpu xiaohui $_rtos || exit 1
	done
done
echo "===compile done!!!==="


