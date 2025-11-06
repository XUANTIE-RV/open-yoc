# 概述

`bare_boot_from_itcm` 是一个玄铁RTOS SDK中面向baremetal领域的从ITCM启动的使用示例。

## R908系列处理器
该示例分为CORE0和CORE1的可执行程序。具体流程是CORE0运行之后把CORE1的固件搬运到CORE1的ITCM空间，然后再配置寄存器触发CORE1从ITCM的首地址运行。
当前目录下有`Makefile`和`Makefile1`两个文件，其中`Makefile`是给CORE0编译用的，`Makefile1`是给CORE1编译使用的，主要区别是他们的`LINKER_SCRIPT`文件不同。
CORE1的固件可以通过`make clean && make cpu=r908 board=xiaohui -f Makefile1`进行编译。CORE1固件编译完成之后，通过`xxd -i yoc.bin 1.h`将bin文件转换成数组，将内容更新到`app/include/cpu1_firmware.h`即可。
CORE0的编译参考下面的`编译`章节进行编译即可。

## E系列处理器
E系列采用一个单独的CORE来实现`boot from itcm`功能。先使用`make clean && make cpu=e907 board=smartl -f Makefile1`来编译出在ITCM里面运行的固件。通过`xxd -i yoc.bin 1.h`将bin文件转换成数组，将内容更新到`app/include/e907_firmware.h`即可。再参考下面的`编译`章节进行编译即可。


# 基于Linux编译运行

## 编译

GCC使用以下命令编译：
```bash
./do_build.sh <cpu> <platform> <libc>
```
LLVM使用以下命令编译：
```bash
./do_build_llvm.sh <cpu> <platform> <libc>
```

`cpu`、`platform`、`libc`支持列表：
- cpu: <br />
        `smartm/smartl/xiaohui`平台的cpu参考 [`components/xuantie_cpu_sdk/xt_rtos_sdk.csv`](../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv) 的 `cpu_list` <br />
- platform: <br />
        smartm smartl xiaohui
- libc: <br />
        newlib minilibc <br />
        默认使用newlib，E系列处理器可支持minilibc

以玄铁`r908`类型为例，可以使用如下命令编译:
```bash
./do_build.sh r908 xiaohui
```

## 运行

示例可以基于FPGA平台运行。

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

注意：
使用DebugServer连接的时候需要额外增加`-setresethaltreq-always-off`选项来禁止CPU复位时被DebugServer截获导致PC停止不自动往下走的问题。
```bash
DebugServerConsole -prereset -setresethaltreq-always-off
```

### 运行结果

R908正常运行串口输出内容参考如下：
```
[cpuid: 0] start to copy itcm code to core1.
[cpuid: 0] finish copy itcm code to core1.
[cpuid: 1] start to run in itcm.
[cpuid: 1] malloc 100B success.
[cpuid: 1] run in itcm success.
```

E907正常运行串口输出内容参考如下：
```
copy itcm code to itcm region.
finish copy itcm code to itcm region.
will reset current core.
start to run in itcm.
malloc 100B success.
run in itcm success.
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁RTOS SDK用户手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁RTOS SDK用户手册》

2、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁RTOS SDK用户手册》

2、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

3、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。

