# 概述
d1_boot_demo是基于D1平台的一个bootloader的DEMO，其基本功能有：下一级镜像的验签、搬运镜像到SDRAM、跳转到下一级镜像。

# 使用

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install d1_boot_demo
```

### 编译&安装

1. 编译

```bash
make clean
make
```
编译完成之后会在当前目录生成boot.elf、boot.bin文件。

2. 安装

```bash
make install
```
执行此命令可以将boot.elf、boot.bin自动拷贝到D1相关的board组件的bootimgs目录下。

### 调试

请注意当前目录下的`gdbinit`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

```bash
riscv64-unknown-elf-gdb boot.elf -x gdbinit
```

# 其他

此工程不需要单独烧写，而是将编译完成的boot.bin文件拷贝到相应的board组件下，然后通过其他demo进行验证。

