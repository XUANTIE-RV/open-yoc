# 概述

`helloworld`是基于RVB2601开发板的最小系统Helloworld例程，其完成了AliOS Things的内核移植，最后在一个任务是实现周期性打印Helloworld的字符串。

## CDK
建议使用CDK集成开发环境统一开发编译下载。

## GCC

安装RISC-V工具链，可以直接从https://occ.t-head.cn/community/download?id=3873045063571677184 下载安装

### 编译

1. 利用一下指令完成编译。

```bash
make clean;make
```
### 烧录

1. 修改 `gdbinit` IP地址，IP地址从DebugServer处获取。
2. 利用以下指令完成镜像烧写

```bash
make flashall
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit_debug
```

## 启动

烧录完成之后按复位键，串口会有打印输出
