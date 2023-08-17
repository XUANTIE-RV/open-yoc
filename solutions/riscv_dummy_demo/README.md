# 概述

`riscv_dummy_demo` 是一个简单的helloword demo，运行在QEMU环境。

使用之前请先在ubuntu上安装一些必要的库:
```bash
sudo apt install libsnappy-dev libpixman-1-dev libdaxctl-dev libvdeplug-dev libnuma-dev libpmem-dev libaio-dev libgbm-dev libepoxy-dev libjpeg8-dev
```

如果安装过程出错，可以尝试先使用`sudo apt update`，再行安装来解决。

# 编译

```bash
make clean;make
```

首次编译，会自动下载QEMU并设置好运行路径；如果出现无法运行情况，请使用`sudo pip install yoctools -U`更新最新版本的yoctools再做尝试。

# 运行

## e系列CPU

```bash
qemu-system-riscv32 -machine smartl -cpu e907fp -nographic -kernel yoc.elf -gdb tcp::1234
```

## c系列CPU

```bash
qemu-system-riscv64 -machine smarth -nographic -kernel yoc.elf -gdb tcp::1234 -cpu c906fd -m 1G
```

# 调试

## e系列CPU

终端1运行：
```bash
qemu-system-riscv32 -machine smartl -cpu e907fp -nographic -kernel yoc.elf -gdb tcp::1234 -S
```

终端2运行：
```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

## c系列CPU

终端1运行：
```bash
qemu-system-riscv64 -machine smarth -nographic -kernel yoc.elf -gdb tcp::1234 -cpu c906fd -m 1G -S
```

终端2运行：
```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

gdbinit文件内容：
```
target remote localhost:1234
```

# 注意

上面的`-cpu e907fp`需要根据用户当前编译时所选择的cpu来做实际的调整，需要和`chip_riscv_dummy`组件下`package.yaml`中的`cpu_name`字段匹配。
`ld_script`字段也需要根据CPU类型不同进行选择。

```yaml
hw_info:
  arch_name: riscv
  vendor_name: dummy
  cpu_name: e907fp
  ld_script: gcc_xip.ld       # e系列CPU使用
#   ld_script: gcc_xip_rv64.ld  # c系列CPU使用
  flash_program: nofile
```

目前支持的cpu型号：`e902 e906 e906f e906fd e907fp e907fdp c906fd c908 c908v`

# QEMU

## 终端退出qemu

在CLI Console环境下输入`quit`也可以退出qemu环境。

或者是：
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
