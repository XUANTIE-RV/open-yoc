# 概述

`bare_helloworld` 是一个玄铁RTOS SDK中面向baremetal领域的helloword demo，可运行在QEMU环境。

其中对于玄铁E9xx系列仅支持M态，C9xx&R9xx仅支持M&S态(默认M态)，可通过修改 Makefile 文件，使能S态支持(开启CONFIG_RISCV_SMODE配置)


# 基于Linux编译运行

## 编译

GCC使用以下命令编译：
```bash
./do_build.sh <cpu> <platform>
```
LLVM使用以下命令编译：
```bash
./do_build_llvm.sh <cpu> <platform>
```

`cpu`和`platform`支持列表：
- cpu: <br />
        `smartl/xiaohui`平台的cpu参考 [`components/xuantie_cpu_sdk/xt_rtos_sdk.csv`](../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv) 的 `cpu_list` <br />
        `wujian300`平台的cpu参考 [`components/wujian300_soc_sdk/sdk.csv`](../../components/wujian300_soc_sdk/sdk.csv) 的 `cpu_list`
- platform: <br />
        smartl xiaohui wujian300

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
M态串口输出内容参考如下
```
Hello World!
bare_helloworld runs success!
```
S态串口输出内容参考如下
```
Hello World!
cpu work mode = Supervisor!
bare_helloworld runs success!
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

