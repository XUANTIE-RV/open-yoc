# 概述

`soc_semihost2` 是一个玄铁RTOS SDK中面向soc领域(使用osal封装)的semihost2示例，使用来自zephyr的semihost三方库。

该示例包含串口打印(printf默认对接到uart外设)、串口命令行输入及semihost_printf、semihost_write等半主机打印/文件读写接口。

注意：该示例运行依赖玄铁DebugServer调试工具。

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform> <kernel>
```
- cpu: <br />
        e902 e902m e902t e902mt e906 e906f e906fd e906p e906fp e906fdp e907 e907f e907fd e907p e907fp e907fdp <br />
        r910 r920 r908 r908fd r908fdv r908-cp r908fd-cp r908fdv-cp <br />
        c906 c906fd c906fdv c908 c908v c908i c910v2 c910v3 c910v3-cp c920v2 c920v3 c920v3-cp <br />
        c907 c907fd c907fdv c907fdvm c907-rv32 c907fd-rv32 c907fdv-rv32 c907fdvm-rv32
- platform: <br />
        smartl xiaohui
- kernel: <br />
        freertos rtthread

以玄铁`c907fdvm`类型为例，可以使用如下命令编译:
```bash
./do_build.sh c907fdvm xiaohui rtthread
```

## 运行

示例仅支持FPGA平台运行。

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果

运行结果可在执行DebugServerConsole命令对应终端中查看, 参考如下
```
output by semihost_printf
output by semihost_printf2
output by semihost_write
```

串口看到以下内容输出：

```
soc_semihost2 demo runs successfully!
[   0.010]<D>[app]<app_task>Hello world! YoC
[   3.010]<D>[app]<app_task>Hello world! YoC
[   6.010]<D>[app]<app_task>Hello world! YoC
[   9.010]<D>[app]<app_task>Hello world! YoC
[  12.010]<D>[app]<app_task>Hello world! YoC
[  15.010]<D>[app]<app_task>Hello world! YoC
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

