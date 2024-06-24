# 概述

`mcu_rtthread_mutex`   是一个玄铁最小系统RTOS SDK中面向mcu领域的mutex demo。该demo基于rtthread原生接口实现，可运行在QEMU环境。

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        e902 e902m e902t e902mt e906 e906f e906fd e906p e906fp e906fdp e907 e907f e907fd e907p e907fp e907fdp <br />
        r910 r920 <br />
        c906 c906fd c906fdv c908 c908v c908i c910 c910v2 c920 c920v2 <br />
        c907 c907fd c907fdv c907fdvm c907-rv32 c907fd-rv32 c907fdv-rv32 c907fdvm-rv32
- platform: <br />
        smartl xiaohui

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

如何基于FPGA平台运行请参考《玄铁最小系统RTOS SDK使用参考手册》

### 运行结果

串口看到以下内容输出，表示运行成功：

```
rt_thread_entry2 mutex protect ,number1 = mumber2 is 1
rt_thread_entry1 mutex protect ,number1 = mumber2 is 2
rt_thread_entry2 mutex protect ,number1 = mumber2 is 3
rt_thread_entry1 mutex protect ,number1 = mumber2 is 4
rt_thread_entry2 mutex protect ,number1 = mumber2 is 5
rt_thread_entry1 mutex protect ,number1 = mumber2 is 6
rt_thread_entry2 mutex protect ,number1 = mumber2 is 7
rt_thread_entry1 mutex protect ,number1 = mumber2 is 8
rt_thread_entry2 mutex protect ,number1 = mumber2 is 9
rt_thread_entry1 mutex protect ,number1 = mumber2 is 10
rt_thread_entry2 exit
rt_thread_entry1 mutex protect ,number1 = mumber2 is 11
rt_thread_entry1 mutex protect ,number1 = mumber2 is 12
rt_thread_entry1 mutex protect ,number1 = mumber2 is 13
rt_thread_entry1 mutex protect ,number1 = mumber2 is 14
rt_thread_entry1 mutex protect ,number1 = mumber2 is 15
rt_thread_entry1 mutex protect ,number1 = mumber2 is 16
rt_thread_entry1 mutex protect ,number1 = mumber2 is 17
rt_thread_entry1 mutex protect ,number1 = mumber2 is 18
rt_thread_entry1 mutex protect ,number1 = mumber2 is 19
rt_thread_entry1 mutex protect ,number1 = mumber2 is 20
rt_thread_entry1 exit
```

# 基于Windows IDE(CDS/CDK)编译运行

如何基于CDS/CDK等IDE编译运行具体请参考《玄铁最小系统RTOS SDK使用参考手册》

# 相关文档和工具

相关文档和工具下载请从玄铁官方站点 https://www.xrvm.cn 搜索下载

1、《玄铁最小系统RTOS SDK快速上手手册》

2、《玄铁最小系统RTOS SDK使用参考手册》

3、玄铁模拟器QEMU工具


# 注意事项

1、Linux平台下基础环境搭建请参考《玄铁最小系统RTOS SDK快速上手手册》

2、如果出现无法运行情况，请使用`sudo pip install yoctools -U`更新最新版本的yoctools再做尝试。

3、玄铁exx系列cpu仅支持smartl平台，cxx/rxx系列cpu仅支持xiaohui平台。

4、玄铁exx系列cpu支持Linux/CDK/CDS环境编译，cxx/rxx系列cpu支持Linux/CDS环境编译

5、某些示例并不是支持所有的玄铁cpu类型，更详细的信息请参考《玄铁最小系统RTOS SDK使用参考手册》

6、某些示例跟硬件特性相关，QEMU中相关功能可能未模拟，只能在相应FPGA硬件平台上运行。