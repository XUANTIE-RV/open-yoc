# 概述

`soc_core_dsp` 是一个玄铁RTOS SDK中面向soc领域(使用osal封装)的玄铁cpu dsp扩展多线程使用示例，可运行在QEMU环境。

示例程序中会创建多个线程调用libcsi_xt900p32fd_dsp.a(采用P扩展指令封装，实际存储在components/csi/csi2/dsp文件夹中)库中的csi_abs_f32函数计算。

注意：该示例仅支持玄铁处理器带P扩展的型号上编译运行

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform> <kernel>
```
- cpu: <br />
        e906p e906fp e906fdp e907p e907fp e907fdp
- platform: <br />
        smartl
- kernel: <br />
        freertos rtthread

以玄铁`e907fdp`类型为例，可以使用如下命令编译:
```bash
./do_build.sh e907fdp smartl rtthread
```

## 运行

示例可以基于玄铁QEMU或FPGA平台运行。

### 基于玄铁QEMU运行

```bash
qemu-system-riscv32 -machine smartl -nographic -kernel yoc.elf -cpu e907fdp
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果

串口看到以下内容输出，表示运行成功：

```cli
soc_core_dsp demo start!
(cli-uart)# dsp runs successfully!
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

