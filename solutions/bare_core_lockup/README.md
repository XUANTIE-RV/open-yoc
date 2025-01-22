# 概述

`bare_core_lockup` 是一个玄铁RTOS SDK中面向baremetal领域的cpu锁定lockup使用示例。

示例程序中首先使用TIMER4作为NMI中断的触发源(3s后触发)，然后访问非法地址进入到异常流程，之后在异常处理流程中再次访问非法地址使得cpu进入锁定状态，最后等待NMI中断触发。

注意：该示例当前仅支持在玄铁Exx处理器上编译运行

# 基于Linux编译运行

## 编译

```bash
./do_build.sh <cpu> <platform>
```
- cpu: <br />
        e902 e902m e902t e902mt e906 e906f e906fd e906p e906fp e906fdp e907 e907f e907fd e907p e907fp e907fdp <br />
- platform: <br />
        smartl

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
bare_core_lockup demo start!
first: access illegal address 0xf1234568, trigger exception!
CPU Exception: NO.0x5
x1: 00003AB2    x2: 20002380    x3: 20000004    x4: 00000000
x5: 000042B0    x6: 0000000F    x7: 200000C4    x8: 200023A0
x9: 00000000    x10: 0000003D   x11: 20002344   x12: 0000003D
x13: FFFFFFFF   x14: 000006B2   x15: FFFFFFFF   x16: 00030D40
x17: 00000000   x18: 00000000   x19: 00000000   x20: 00000000
x21: 00000000   x22: 00000000   x23: 00000000   x24: 00000000
x25: 00000000   x26: 00000000   x27: 00000000   x28: 00000000
x29: 00000000   x30: 00000000   x31: 00000000
mepc   : 00003AC8
mstatus: 00003800
entry exception callback: my_trap_c
second: access illegal address 0xf1234568 again, trigger lockup! and wait for nmi!
nmi has happened
bare_core_lockup runs success!
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

