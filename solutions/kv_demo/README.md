# 概述
kv_demo是最小KV文件系统功能的操作例程。KV文件系统是基于Flash的一种Key-Value 数据存储系统，该系统采用极小的代码及内存开销（最小资源 rom：3K bytes，ram：100bytes），在小规模的Flash上实现数据的存储管理能力，支持断电保护、磨损均衡、坏块处理等功能。KV文件系统存储系统支持只读模式与读写模式共存，只读模式可以用于工厂生产数据，读写模式可用于运行时的数据存存储。

# 使用

## CDK

在CDK的首页，通过搜索kv_demo，可以找到kv_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行

需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install kv_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

#### D1平台

1. 编译

```bash
make clean
make SDK=sdk_chip_d1
```

2. 烧写

```bash
make flashall SDK=sdk_chip_d1
```

#### bl606P平台

1. 编译

```bash
make clean
make SDK=sdk_chip_bl606p_e907
```

2. 烧写

```bash
make flashall SDK=sdk_chip_bl606p_e907
```

#### ch2601平台

1. 编译

```bash
make clean
make SDK=sdk_chip_ch2601
```

2. 烧写

```bash
make flashall SDK=sdk_chip_ch2601
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
重新上电或按下RST键，系统启动，串口会有以下打印信息，表示系统运行成功。

```
cpu clock is 1008000000Hz
[ (cli-uart)#   0.190]<I>[app]<app_task>app start........

[   0.190]<I>[app]<app_task>start kv testing...
[   0.200]<I>[app]<app_task>find 6 partitions
[   0.210]<E>[app]<app_task>kv init successfully

write one integer value into kv 
k:v = key_int:11223344
Delete one key from kv 
Delete the key(key_int)
k:v = key_int:0

write one string into kv 
k:v = key_str:hello kv
reset kv to erase all values
The key name(key_str) does not exist 
```

用户可以在cli输入以下命令进行KV数据的存取。

列出所有KV数据：
```cli
kv list
```

设置字符串：
```cli
kv set key1 string1
```

获取字符串：
```cli
kv get key1
```

设置整型：
```cli
kv setint key2 123
```

获取整型：
```cli
kv getint key2
```

删除某个key：
```cli
kv del key3
```