# 概述

`bare_core_vic` 是一个玄铁RTOS SDK中面向baremetal领域的玄铁cpu中断控制器使用示例，可运行在QEMU环境

该示例可演示中断嵌套等功能的使用。示例中开启了两个不同优先级的定时器，对于支持中断嵌套的玄铁处理器，高优先级的定时器中断会抢占低优先中断。

注意：中断嵌套功能当前仅支持玄铁E9xx处理器和R908 clic模式。

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
        `wujian300`平台的cpu参考 [`components/wujian300_soc_sdk/sdk.csv`](../../components/wujian300_soc_sdk/sdk.csv) 的 `cpu_list`
- platform: <br />
        smartm smartl xiaohui wujian300
- libc: <br />
        newlib minilibc <br />
        默认使用newlib，E系列处理器可支持minilibc

以玄铁`e907fdp`类型为例，可以使用如下命令编译:
```bash
./do_build.sh e907fdp smartl
```

## 运行

示例可以基于玄铁QEMU或FPGA平台运行。

### 基于玄铁QEMU运行

```bash
qemu-system-riscv32 -machine smartl -cpu e907fdp -nographic -kernel yoc.elf
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果
正常运行串口输出内容参考如下
```
bare_core_vic demo start!
set timer 0 's interrrupt priority with 2 (lower priority).
initialize timer 1, set it's interrupt priority : 3 (higher priority).
start reload timer 0 with lower interrupt priority, and it will enter an endless loop callback
start reload mode timer 1 with higher interrupt priority.
I am timer 1's with higher interrupt priority than timer 0.
I am timer 0's callback in an endless loop.
bare_core_vic runs success!
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

