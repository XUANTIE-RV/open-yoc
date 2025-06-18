# 概述

`bare_core_ai` 是一个玄铁RTOS SDK中面向baremetal领域的ai扩展使用示例，可运行在QEMU环境。

# 基于Linux编译运行

## 编译

GCC使用以下命令编译：
```bash
./do_build.sh <cpu> <platform>
```
LLVM使用以下命令编译:
```bash
./do_build_llvm.sh <cpu> <platform>
```

`cpu`和`platform`支持列表：
- cpu: <br />
        `xiaohui`平台的cpu参考 [`components/xuantie_cpu_sdk/xt_rtos_sdk.csv`](../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv) 的 `cpu_list`
- platform: <br />
        xiaohui

以玄铁`c908x-cp-xt`类型为例，可以使用如下命令编译:
```bash
./do_build.sh c908x-cp-xt xiaohui
```

## 运行

示例可以基于玄铁QEMU或FPGA平台运行。

### 基于玄铁QEMU运行

```bash
qemu-system-riscv64 -machine xiaohui -nographic -kernel yoc.elf -cpu c908x-cp-xt
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果
正常运行串口输出内容参考如下
```
bare_core_ai demo start!
===vlen=128
--------- add start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- add end ---------
--------- exp start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- exp end ---------
--------- gather start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- gather end ---------
--------- layer_norm start ---------
[Successful] Check infer shape
<layer_norm fp32> execution cycles = 2691895
[Successful] The cos sim is: 1.000000
--------- layer_norm end ---------
--------- matmul start ---------
[Successful] Check infer shape
<matmul fp32> execution cycles = 7761698, 0.021 GFLOPS
[Successful] The cos sim is: 1.000000
--------- matmul end ---------
--------- mul start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- mul end ---------
--------- slice start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- slice end ---------
--------- softmax start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- softmax end ---------
--------- sub start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- sub end ---------
--------- where start ---------
[Successful] Check infer shape
[Successful] The cos sim is: 1.000000
--------- where end ---------
bare_core_ai demo end!
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

