# 概述

`bare_whetstone` 是一个玄铁RTOS SDK中面向baremetal领域的whetstone示例。whetstone是一种基准测试程序，用于评估处理器的浮点运算性能，不涵盖整数运算、IO、网络等方面。

该示例需要在对应fpga平台上运行，基于QEMU运行无实际意义。

注意：该示例当前仅支持在玄铁带硬浮点的处理器上编译运行

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        e906f e906fd e906fp e906fdp e907f e907fd e907fp e907fdp <br />
        r910 r920 r908 r908fd r908fdv r908-cp r908fd-cp r908fdv-cp <br />
        c906fd c906fdv c908 c908v c910v2 c910v3 c910v3-cp c920v2 c920v3 c920v3-cp <br />
        c907fd c907fdv c907fdvm c907fd-rv32 c907fdv-rv32 c907fdvm-rv32
- platform: <br />
        smartl xiaohui

以玄铁`e907fdp`类型为例，可以使用如下命令编译:
```bash
./do_build.sh e907fdp smartl
```

## 运行

示例可以基于玄铁FPGA平台运行。

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果
正常运行串口输出内容参考如下
```
bare_whetstone demo start!

Loops: 10000, Iterations: 1, Duration: 30 sec.
C Converted Double Precision Whetstones: 32.8 MIPS
bare_whetstone runs success!
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

