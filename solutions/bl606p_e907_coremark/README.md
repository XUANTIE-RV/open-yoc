# 概述
bl606p_e907_coremark是CPU测评的demo。

# 使用
## CDK
在CDK的首页，通过搜索bl606p_e907_coremark，可以找到bl606p_e907_coremark，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install bl606p_e907_coremark
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

bl606p_e907_coremark只适用于bl606p_e907的CPU测评。

1. 编译

   ~~~bash
   make clean
   make
   ~~~

2. 烧写

   ~~~bash
   make flash
   ~~~

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后按下复位键，串口会有以下打印输出。

~~~uart
start coremark

[10:40:29.025]收←◆2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 19354307
Total time (secs): 19.354307
Iterations/Sec   : 1033.361722
Iterations       : 20000
Compiler version : 10.2.0
Compiler flags   :  
Memory location  : STACK
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x382f
Correct operation validated. See README.md for run and reporting rules.
CoreMark 1.0 : 1033.361722 10.2.0   / STACK
Score (Coremarks/MHz):   3.23
~~~
