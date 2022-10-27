# 概述

`arm_dummy_demo` 是一个简单的helloword demo，运行在QEMU环境。

使用之前请先在ubuntu上安装`qemu-system-arm`。

```bash
sudo apt install qemu-system-arm
```

# 编译

```bash
make clean;make
```

# 运行

```bash
qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -vga none -kernel yoc.elf -gdb tcp::1234
```

# 调试

终端1运行：
```bash
qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -vga none -kernel yoc.elf -gdb tcp::1234 -S
```

终端2运行：
```bash
arm-none-eabi-gdb yoc.elf -x gdbinit
```

gdbinit文件内容：
```
target remote localhost:1234
```

# QEMU

## 终端退出qemu

1. 先Ctrl+A
2. 松开所有按键， 再按下C键

## 常用命令

以下是一些常用选项，更多的选项可以参考《QEMU Emulator User Documentation》。
```
-help
显示帮助信息。
-version
显示版本信息。
-machine
选择模拟的开发板，可以输入-machine help 获取一个完整的开发板列表。
-cpu
选择CPU 类型（例如-cpu ck803），可以输入-cpu help 获取完整的CPU 列表。
-nographic
禁止所有的图形输出，模拟的串口将会重定向到命令行。
-gdb tcp::port
设置连接GDB 的端口，（例如-gdb tcp::23333, 将23333 作为GDB 的连接端口）
-S
在启动时冻结CPU ，（例如与-gdb 配合，通过GDB 控制继续执行）
```
