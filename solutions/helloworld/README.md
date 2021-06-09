# 概述

`helloworld` 是一个简单的demo应用

# CDK IDE编译
由于helloworld工程支持不同的CHIP SDK切换， 所以必须使用CDK v2.7.x以上的版本进行编译和下载。否则，可能会有下载失败的现象。

# 命令行编译

```bash
make clean;make
```

## 指定BOARD编译

比如选择cb5631的board进行编译，则使用以下命令：
```bash
make BOARD=cb5631
```

## 指定SDK编译
比如选择sdk_chip_ch2201进行编译。

```bash
make SDK=sdk_chip_ch2201
```

# 烧录


```bash
make flashall
```

## 指定芯片SDK进行烧录
```bash
make flashall SDK=sdk_chip_ch2201
```

# 启动

烧录完成之后按复位键或者插拔电源线，串口会有打印输出

```
[   0.001]<I>INIT Build:Dec 29 2020,17:18:30
[   0.005]<D>app Hello world! YoC
```


# QEMU运行

## QEMU安装

make过程中yoctools会自动安装qemu模拟器到~/.thead目录下，不需要单独安装。

### CSKY-QEMU

#### 编译

```bash
make SDK=sdk_chip_csky_dummy
```

#### 运行

```bash
$ ./run_csky_qemu.sh
qemu-system-cskyv2 -machine smartl -cpu ck804ef -nographic -kernel yoc.elf -gdb tcp::1234
[   0.103 ]<I>INIT Build:Sep 13 2020,12:47:23
Welcome to CLI...
> [   0.233 ]<D>app Hello world! YoC
```

### ARM-QEMU

#### 编译

```bash
make SDK=sdk_chip_arm_dummy
```

#### 运行

```bash
$ ./run_arm_qemu.sh
qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -vga none -kernel yoc.elf -gdb tcp::1234
[   0.103 ]<I>INIT Build:Sep 13 2020,12:47:23
Welcome to CLI...
> [   0.233 ]<D>app Hello world! YoC
```

### RISCV-QEMU

#### 编译

```bash
make SDK=sdk_chip_riscv_dummy
```

#### 运行

```bash
$ ./run_riscv_qemu.sh
qemu-system-riscv32 -machine smartl -cpu e906fd -nographic -kernel yoc.elf -gdb tcp::1234
[   0.103 ]<I>INIT Build:Sep 13 2020,12:47:23
Welcome to CLI...
> [   0.233 ]<D>app Hello world! YoC
```

## 退出

模拟器退出方式，分三步：

第一步、ctrl+a

第二步、放开

第三步、c

## 调试

需要开启两个终端，一个终端运行qemu命令，一个终端运行gdb命令

先在终端1上运行命令：

```bash
$ qemu-system-cskyv2 -machine smartl -cpu ck804ef -nographic -kernel yoc.elf -gdb tcp::1234 -S
```

后在终端2上运行命令：

```bash
$ csky-abiv2-elf-gdb yoc.elf -x gdbinit
```

终端2上运行如下，说明成功进入调试模式

```bash
GNU gdb (C-SKY Tools V3.10.21 Minilibc abiv2) 7.12
Copyright (C) 2016 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-linux --target=csky-elfabiv2".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from yoc.elf...done.
Reset_Handler () at /home/chenlf/sourcecode/yoc7/components/csi_csky_dummy/arch/csky/ck804ef/startup.S:101
101         psrclr  ie
Loading section .text, size 0x78c0 lma 0x0
section progress: 100.0%, total progress: 89.78%
Loading section .rodata, size 0xaec lma 0x78c0
section progress: 100.0%, total progress: 97.90%
Loading section .data, size 0xc4 lma 0x83ac
section progress: 100.0%, total progress: 98.47%
Loading section ._ram_code, size 0x210 lma 0x8470
section progress: 100.0%, total progress: 100.00%
Start address 0x144, load size 34432
Transfer rate: 16812 KB/sec, 1721 bytes/write.
(cskygdb)
```

## QEMU环境搭建贴心小助手

### CSKY QEMU

CSKY QEMU安装完成后会依赖很多库，安装命令如下：

```bash
sudo apt install libbrlapi-dev
sudo apt install libvdeplug-dev
sudo apt install libaio-dev
sudo apt install libpixman-1-dev
sudo apt install libnuma-dev
sudo apt install libbluetooth-dev
wget -q -O /tmp/libpng12.deb http://mirrors.kernel.org/ubuntu/pool/main/libp/libpng/libpng12-0_1.2.54-1ubuntu1_amd64.deb   && sudo dpkg -i /tmp/libpng12.deb   && rm /tmp/libpng12.deb
sudo apt install libjpeg-dev
sudo apt install liblzo2-dev
sudo apt install libsnappy-dev
sudo apt install libspice-server-dev
```

### ARM QEMU

请勿使用` apt get install qemu-system-arm`安装的QEMU，该QEMU有BUG，问题是UART接收中断无法清除，若已安装建议删除，建议使用yoctools自动安装的qemu。

#### 自己Build QEMU

也可以自己编译QEMU，方法如下：

##### 下载

https://github.com/qemu/qemu.git

##### 配置

```
./configure --target-list=arm-softmmu --audio-drv-list=  --static --prefix=/home/chenlf/qemu2/install
```

加上--static是为了不同linux平台通用性

##### Build安装

```
make;make install
```
