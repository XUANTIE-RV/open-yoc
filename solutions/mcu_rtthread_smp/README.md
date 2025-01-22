# 概述

`mcu_rtthread_smp` 是一个玄铁RTOS SDK中面向mcu领域的smp demo。该demo基于rtthread原生接口实现，可运行在QEMU环境。

smp功能的使能主要依赖下面这两个配置(参考package.yaml):

```bash
  CONFIG_SMP: 1
  CONFIG_NR_CPUS: 2
```

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        r910 r920 r908 r908fd r908fdv r908-cp r908fd-cp r908fdv-cp <br />
        c908 c908v c908i c910v2 c910v3 c910v3-cp c920v2 c920v3 c920v3-cp <br />
        c907 c907fd c907fdv c907fdvm c907-rv32 c907fd-rv32 c907fdv-rv32 c907fdvm-rv32
- platform: <br />
        xiaohui

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
[thread_1] in 1 core, count:0
[thread_0] in 0 core, count:1
[thread_0] in 0 core, count:2
[thread_1] in 1 core, count:3
[thread_0] in 0 core, count:4
[thread_1] in 1 core, count:5
[thread_0] in 0 core, count:6
[thread_1] in 1 core, count:7
[thread_0] in 0 core, count:8
[thread_1] in 1 core, count:9
[thread_0] in 0 core, count:10
......
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁RTOS SDK用户手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁RTOS SDK用户手册》

2、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁RTOS SDK用户手册》

2、如果出现无法编译情况，请使用`sudo pip install yoctools -U`更新最新版本的yoctools再做尝试。

3、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

4、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。


