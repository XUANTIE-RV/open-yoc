# 概述

`bare_core_matrix` 是一个玄铁RTOS SDK中面向baremetal领域的玄铁cpu matrix使用示例，可运行在QEMU环境。

用户可参考matrix汇编相关代码实现其他功能。

注意：该示例仅支持玄铁处理器带matrix扩展的型号上编译运行

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        c907fdvm
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
qemu-system-riscv64 -machine xiaohui -nographic -kernel yoc.elf -cpu c907fdvm
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果
正常运行串口输出内容参考如下
```
bare_core_matrix demo start!
[int8] gemm start
[m * k * n = 48 * 48 * 48] best execution time: 0.000ms, inf GFLOPS
[m * k * n = 96 * 96 * 96] best execution time: 3.000ms, 0.59 GFLOPS
[m * k * n = 144 * 144 * 144] best execution time: 5.000ms, 1.19 GFLOPS
[m * k * n = 192 * 192 * 192] best execution time: 11.000ms, 1.29 GFLOPS
[m * k * n = 240 * 240 * 240] best execution time: 20.000ms, 1.38 GFLOPS
[int8] gemm end
matrix runs successfully!
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

