# 概述

`bare_core_ecc` 是一个玄铁RTOS SDK中面向baremetal领域的ECC校验使用示例。

注意：
- 当前仅玄铁C9xx & R9xx部分型号支持ecc功能

- 当运行出现失败打印时，可能的原因是在制作bit时未使能L1 or L2 ecc功能，属于正常现象。此时可通过jtag连接时的打印信息确认

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
        `wujian300`平台的cpu参考 [`components/wujian300_soc_sdk/sdk.csv`](../../components/wujian300_soc_sdk/sdk.csv) 的 `cpu_list`
- platform: <br />
        xiaohui wujian300
- libc: <br />
        newlib minilibc <br />
        默认使用newlib，E系列处理器可支持minilibc

以玄铁`c907fdvm`类型为例，可以使用如下命令编译:
```bash
./do_build.sh c907fdvm xiaohui
```

## 运行

示例可以基于玄铁FPGA平台运行。

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果
R908正确运行将会输出如下日志。对于不支持TCM ECC的处理器，只会输出CACHE ECC的日志。
```
bare_core_ecc demo start!
=========CACHE ECC============
Before 1-bit error inject to L1 Cache, mcer = 0x8000000c2e000000.
After 1-bit error inject to L1 Cache, mcer = 0x8000000c2e000000.
L1 ecc failed to correct 1-bit eror! This cpu may not support ecc module.
Before 1-bit error inject to L2 Cache, mcer2 = 0x20000008004c00.
After 1-bit error inject to L2 Cache, mcer2 = 0x8130000000002100.
L2 ecc 1-bit error corrected!
come to ecc_l1_irqhandler
L1 ecc 2-bits error detected!
come to ecc_l2_irqhandler
L2 ecc 2-bits error detected!
=========TCM ECC============
dtcm size:0x80000
itcm size:0x100000
DTCM ECC: before 1-bit error inject, mcer = 0x6000b00
trigger ecc in DTCM:
read value[0x9898989898989898] in dtcm[0x50300000].
DTCM ECC: after 1-bit error inject, mcer = 0x800000081c000000
DTCM ECC: 1-bit error corrected!
DTCM ECC: 2-bit error inject, mcer = 0x800000081c000000
come to ecc_l1_irqhandler
read value[0x9898989898989898] in dtcm[0x50300000].
DTCM ECC: after 2-bit error inject, mcer = 0xc000000
DTCM ECC: 2bit runs successfully!
ITCM ECC: before 1-bit error inject, mcer = 0xc000000
trigger ecc in ITCM:
[9]: execute in tcm region
[8]: execute in tcm region
[7]: execute in tcm region
[6]: execute in tcm region
[5]: execute in tcm region
[4]: execute in tcm region
[3]: execute in tcm region
[2]: execute in tcm region
[1]: execute in tcm region
[0]: execute in tcm region
ITCM ECC: after 1-bit error inject, mcer = 0x800000081e000000
ITCM ECC: 1-bit error corrected!
ITCM ECC: before 2-bit error inject, mcer = 0x800000081e000000
CPU Exception(mcause);: NO.0x38000001
x1: 000000005000241C    x2: 0000000050010AC0    x3: 0000000050003D58    x4: 0000000000000000
x5: 0000000000000000    x6: 0000000000000010    x7: 0000000000000030    x8: 800000081E000000
x9: 000000000C000000    x10: 000000000000003F   x11: 0000000050010A60   x12: 000000000000003F
x13: FFFFFFFFFFFFFFFF   x14: FFFFFFFFFFFFFFFF   x15: 00000000E0000003   x16: 0000000000000000
x17: 0000000000000300   x18: 0000000006000B00   x19: 0000000000000003   x20: 0000000000000000
x21: 0000000000000000   x22: 0000000000000000   x23: 0000000000000000   x24: 0000000000000000
x25: 0000000000000000   x26: 0000000000000000   x27: 0000000000000000   x28: 0000000000000020
x29: 0000000000000000   x30: 0000000000000000   x31: 0000000000000020
mepc   : 0000000050200000
mstatus: 0000000200001880
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

