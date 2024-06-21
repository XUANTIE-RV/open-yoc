# 概述
ch2601_boot是基于ch2601平台的一个bootloader的DEMO，其基本功能有：下一级镜像的验签、跳转到下一级镜像。

# 使用

## 通过命令行
需要先安装[yoctools](https://www.xrvm.cn/document?temp=yoctools&slug=yocbook)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install ch2601_boot
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
执行此命令可以将boot.elf、boot.bin自动拷贝到ch2601相关的board组件的bootimgs目录下。

### 调试

请注意当前目录下的`gdbinit`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的 XuanTie Debug Server 中显示的对应的内容。

```bash
riscv64-unknown-elf-gdb boot.elf -x gdbinit
```

# 其他

此工程不需要单独烧写，而是将编译完成的boot.bin文件拷贝到相应的board组件下，然后通过其他demo进行验证。

