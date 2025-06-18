# 概述

`bare_pie_demo` 是一个玄铁RTOS SDK中面向baremetal领域的地址无关 demo，仅支持LLVM编译，可运行在QEMU环境。

# 基于Linux编译运行

## 编译

LLVM使用以下命令编译：
```bash
./do_build_llvm.sh <cpu> <platform>
```

`cpu`和`platform`支持列表：
- cpu: <br />
        `xiaohui`平台的cpu参考 [`components/xuantie_cpu_sdk/xt_rtos_sdk.csv`](../../components/xuantie_cpu_sdk/xt_rtos_sdk.csv) 的 `cpu_list`
- platform: <br />
        xiaohui

以玄铁`c908`类型为例，可以使用如下命令编译:
```bash
./do_build_llvm.sh c908 xiaohui
```

## 运行

示例可以基于玄铁QEMU或FPGA平台运行。

### 基于玄铁QEMU运行

#### 在链接脚本中固定的地址运行

```bash
qemu-system-riscv64 -machine xiaohui -cpu c908 -nographic -kernel yoc.elf
```

#### 用户指定地址运行

1. 首先编译后在一个终端中执行

```bash
qemu-system-riscv64 -machine xiaohui -cpu c908 -nographic -kernel yoc.elf -gdb tcp::1234 -S
```

2. 再开一个终端，运行`riscv64-unknown-elf-gdb`，在gdb中输入以下指令

```bash
target remote localhost:1234
load yoc.elf 0x100000
set $pc=0x50100000
c
```

此时在第一个终端中会显示执行成功的结果。
上述gdb命令中load_address（上面的0x100000）可以改变，只要还在机器的可使用地址区间内。
如果load_address变化，第三行的pc也要相应调整，为0x50000000+load_address，其中0x50000000来自链接器脚本中方的DRAM ORIGIN。

#### 拷贝镜像运行

通常的做法是将一个固件编译为地址无关的程序，可以放到随意的可执行的地址运行。
如要将地址无关固件放在`0x50100000`地址运行，那么可以在上一级固件里面添加如下代码将地址无关固件搬运到相应的位置，并跳转执行。
```c
printf("will go to the PIE firmware.\n");
memcpy((void *)0x50100000, yoc_bin, sizeof(yoc_bin));
csi_dcache_clean();
csi_icache_invalid();
void (*func)() = (void(*)())0x50100000;
func();
```

#### 终端退出qemu

1. 先Ctrl+a
2. 松开所有按键， 紧接着再按下x键

### 基于FPGA平台运行

如何基于FPGA平台运行请参考《玄铁RTOS SDK用户手册》

### 运行结果

串口输出参考如下：
```
hello PIE demo!
__dram_vma:         0x50000000
__stext(dram_lma):  0x50100000
__rodata_end__:     0x50102f70
__data_start__:     0x501038d8
__data_end__:       0x50103a20
__bss_start__:      0x50103cd8
__bss_end__:        0x50104170
g_heap_start:       0x5010a170
g_heap_end:         0x502ffff8
Global Variable in data section Address:    0x50103a1c
Global Variable in bss section Address:     0x50103d0c
malloc mem addr1: 0x5010a190
malloc mem addr2: 0x5010a5b0
PIE demo run ok.
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

