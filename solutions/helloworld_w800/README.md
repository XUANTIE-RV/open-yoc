# 概述

W800是一颗适用于物联网领域的Wi-Fi/蓝牙双模 SoC 芯片，适用于智能家电、智能家居、智能玩具、无线音视频、工业控制、医疗监护等广泛的物联网领域。

本文介绍基于W800芯片的Helloworld例程的运行。

# 环境搭建

windows及linux下环境搭建参考[W800快速上手手册](https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/userFiles/3717897501090217984/resource/3717897501090217984jKGhhefnxh.pdf)

# 编译

在linux下编译执行

```bash
make clean;make
```

使用CDK则直接点击编译按钮。

# 烧录

**使用CK-Link烧录**

在linux环境下烧录执行

```bash
make flashall
```

烧录成功后，可以看到所有分区烧写进度都已至100%。

```
Program partition: prim         address: 0x80d0400, size 664816 byte
erasing...
program 08172400, 100%
Program partition: imtb         address: 0x81c0000, size 4096 byte
erasing...
program 0081c1000, 100%
```

使用CDK则直接点击烧录下载按钮。

**使用串口烧录**

CB6201开发板支持不通过CK-Link而是直接通过串口进行烧录。按下CB6201开发板上BOOT按键后同时按RST按键，此时UART0输出CCCCC，表明进入烧录模式。然后通过串口工具（如SecureCRT）选取Xmodem方式将位于out/smart_lighting_w800目录下生成的yoc_total.fls文件烧录到开发板。

# 启动

烧录完成之后按复位键，串口会有打印输出。

```
[   0.000]<I>INIT Build:Jan 27 2021,20:44:34
[   0.020]<D>app Hello world! YoC

```

