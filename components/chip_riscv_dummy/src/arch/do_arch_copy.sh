#!/bin/bash

archs_0="e902 e902m e902t e902mt"
archs_1="e906 e906f e906fd e906fdp e906fp e906p e907 e907f e907fd e907fdp e907fp e907p"
archs_2="c906 c906fd c906fdv"
archs_3="c908 c908i c908v c910 c910v2 c920 c920v2 r910 r920"
archs_4="c907 c907fd c907fdv c907fdvm c907-rv32 c907fd-rv32 c907fdv-rv32 c907fdvm-rv32"
if [ $# -lt 1 ]; then
	echo "./do_arch_copy.sh arch_name"
	echo "    eg: ./do_arch_copy.sh c908"
	exit 1
fi

arch_ori=$1
if [[ "$archs_0" =~ "$arch_ori" ]];then
	echo "arch is in e902xx list"
	mv $arch_ori "/tmp/"$arch_ori
	rm e902* -rf
	mv "/tmp/"$arch_ori $arch_ori

	for arch in ${archs_0[@]}
	do
		if [ "$arch" != "$arch_ori" ];then
			cp $arch_ori $arch -rf
		fi
	done
	echo "copy arch from "$arch_ori" is done"
	exit 0
fi

if [[ "$archs_1" =~ "$arch_ori" ]];then
	echo "arch is in e906/7xx list"
	mv $arch_ori "/tmp/"$arch_ori
	rm e906* -rf
	rm e907* -rf
	mv "/tmp/"$arch_ori $arch_ori

	for arch in ${archs_1[@]}
	do
		if [ "$arch" != "$arch_ori" ];then
			cp $arch_ori $arch -rf
		fi
	done
	echo "copy arch from "$arch_ori" is done"
	exit 0
fi

if [[ "$archs_2" =~ "$arch_ori" ]];then
	echo "arch is in c906xx list"
	mv $arch_ori "/tmp/"$arch_ori
	rm c906* -rf
	mv "/tmp/"$arch_ori $arch_ori

	for arch in ${archs_2[@]}
	do
		if [ "$arch" != "$arch_ori" ];then
			cp $arch_ori $arch -rf
		fi
	done
	echo "copy arch from "$arch_ori" is done"
	exit 0
fi

if [[ "$archs_3" =~ "$arch_ori" ]];then
	echo "arch is in c908xx list"
	mv $arch_ori "/tmp/"$arch_ori
	rm c908* -rf
	rm c910* -rf
	rm c920* -rf
	rm r910* -rf
	rm r920* -rf
	mv "/tmp/"$arch_ori $arch_ori

	for arch in ${archs_3[@]}
	do
		if [ "$arch" != "$arch_ori" ];then
			cp $arch_ori $arch -rf
		fi
	done
	echo "copy arch from "$arch_ori" is done"
	exit 0
fi

if [[ "$archs_4" =~ "$arch_ori" ]];then
	echo "arch is in c907xx list"
	mv $arch_ori "/tmp/"$arch_ori
	rm c907* -rf
	mv "/tmp/"$arch_ori $arch_ori

	for arch in ${archs_4[@]}
	do
		if [ "$arch" != "$arch_ori" ];then
			cp $arch_ori $arch -rf
		fi
	done
	echo "copy arch from "$arch_ori" is done"
	exit 0
fi

echo "error: the arch is not support"
exit 1





