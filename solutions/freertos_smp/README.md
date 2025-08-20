# 概述

`freertos_smp` 是一个玄铁RTOS SDK中的smp demo。该demo基于freertos原生接口实现，可运行在QEMU环境或者FPGA环境。

smp功能的使能主要依赖下面这两个配置(参考Makefile):

```makefile
# SMP CONFIG
CFLAGS += -DCONFIG_SMP=1 -DCONFIG_NR_CPUS=2
```

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
        `xiaohui`平台的cpu参考 [`components/xuantie_cpu_sdk/xt_rtos_sdk.csv`](../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv) 的 `cpu_list` <br />
- platform: <br />
        xiaohui
- libc: <br />
        newlib minilibc <br />
        默认使用newlib，E系列处理器可支持minilibc

以玄铁`c907fdvm`类型为例，可以使用如下命令编译:
```bash
./do_build.sh c907fdvm xiaohui
```

## 运行

示例可以基于玄铁QEMU或FPGA平台运行。

### 基于玄铁QEMU运行

```bash
qemu-system-riscv64 -machine xiaohui -smp cpus=2 -nographic -kernel yoc.elf -cpu c907fdvm
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果

串口看到以下内容输出，表示运行成功：

```
freertos_smp starts.
FreeRTOS version:V11.1.0
[thread_0] in 1 core, count: 0
[thread_1] in 1 core, count: 1
[thread_0] in 0 core, count: 2
[thread_1] in 1 core, count: 3
[thread_1] in 1 core, count: 4
[thread_0] in 0 core, count: 5
[thread_1] in 1 core, count: 6
[thread_0] in 0 core, count: 7
......
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁RTOS SDK用户手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁RTOS SDK用户手册》

3、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁RTOS SDK用户手册》

2、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

3、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。


