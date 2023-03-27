# 概述

FOTA升级的DEMO。FOTA的云服务在OCC，包括固件的管理，许可证的管理，设备的管理等。
具体可以参考OCC上的博文《YoC RTOS 实战：FoTA系统升级》

# 使用

## 通过CDK

在CDK的首页，通过搜索关键字fota，可以找到fota_demo，然后创建工程。

CDK的使用可以参考YoCBook 《CDK开发快速上手》 章节， 链接 <https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html>

具体步骤参考OCC上的博文《YoC RTOS 实战：FoTA系统升级》

## 通过命令行

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install fota_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

烧录完成之后，按板子上的复位键即可看到串口输出。

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

由于CH2601 RAM资源比较有限，所以针对fota demo需要使用另一个连接脚本`gcc_flash_heap_bigger.ld`，具体在component/chip_ch2601下。需要替换

`component/chip_ch2601/package.ymal`文件中的如下部分：

```yaml
hw_info:

 arch_name: riscv

 vendor_name: thead

 cpu_name: e906

 ld_script: gcc_flash_heap_bigger.ld   # gcc_flash.ld

 toolchain_prefix: riscv64-unknown-elf

 flash_program: ch2601_flash.elf
```

1. 编译

```bash
make clean
make SDK=sdk_chip_ch2601
```

2. 烧写

```bash
make flashall SDK=sdk_chip_ch2601
```

#### cv181x平台

1. 编译

```bash
make clean
make SDK=sdk_chip_cv181xh
```

2. 烧写

```bash
make flashall SDK=sdk_chip_cv181xh
```

#### f133平台

1. 编译

```bash
make clean
make SDK=sdk_chip_f133
```

2. 烧写

```bash
make flashall SDK=sdk_chip_f133
```

### FOTA基础版本烧录

#### D1平台

从OCC服务器上下载之前上传的镜像包进行烧录。

```bash
product flash 20210304215611061_factory.zip -a -f ../../components/chip_d1/d1_flash.elf -x gdbinitflash
```

20210304215611061_factory.zip为从OCC上下载过来的基础镜像包。

**注意：**
本地编译之前需要先从OCC上下载kp文件，并替换至相应board组件bootimgs目录下的kp文件。否则会提示验签不过。

#### bl606p平台

直接烧录本地编译好的基础版本即可。

```bash
make flashall SDK=sdk_chip_bl606p_e907
```

#### ch2601平台

从OCC服务器上下载之前上传的镜像包进行烧录。

```bash
product flash 20220907183302785_factory.zip -a -f ../../components/chip_ch2601/ch2601_flash.elf -x gdbinitflash
```

20220907183302785_factory.zip为从OCC上下载过来的基础镜像包。

#### cv181x平台

直接烧录本地编译好的基础版本即可。

```bash
make flashall SDK=sdk_chip_cv181xh
```

#### f133平台

从OCC服务器上下载之前上传的镜像包进行烧录。

```bash
product flash 20221212160936892_factory.zip -a -f ../../components/chip_d1/d1_flash.elf -x gdbinitflash
```

20221212160936892_factory.zip为从OCC上下载过来的基础镜像包。

f133的fota升级是全量升级，与 D1 的操作一致

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 联网配置

通过串口终端输入:

```cli
ifconfig ap <ssid> <password>
```

其中<ssid>指的是WiFi名称，<password>指的是WiFi密码。配置完成之后输入reboot命令进行复位，或者通过开发板的复位键进行复位。

# fota配置

系统默认20秒检查一次升级，读写超时为20S，重试次数为0次。如果需要修改这些配置可以通过kv进行设置，比如：

```cli
kv setint fota_slptm 10000
kv setint fota_rtmout 5000
kv setint fota_wtmout 5000
kv setint fota_retry 3
```

- fota_slptm：循环检测升级时间，单位毫秒
- fota_rtmout：读超时时间，单位毫秒
- fota_wtmout：写超时时间，单位毫秒
- fota_retry：读写失败之后的重试次数

## 设置设备ID

ID号从OCC上获取。

通过串口终端输入:

```cli
kv set device_id 3a1ad548044000006230e778e3b3ec26
```

## 设置产品类型

通过串口终端输入:

```cli
kv set model xxx_model
```

## 使能FOTA检测

默认使能。

通过串口终端输入:

```cli
kv setint fota_en 0
```

或者

```cli
kv setint fota_en 1
```

配置完成之后需要复位。

# 运行

从OCC下载完FOTA相关的数据并且校验成功之后就会开始自动重启并且进行固件更新。
以下为串口的打印。

```cli
...
[15:49:09:054][boot][I] fota data hash verify ok
[15:49:09:055][boot][I] start to upgrade
[15:49:09:180][boot][I] fd:0x40025b20,fd_num:0
[15:49:09:251][boot][I] start FULL update
[15:49:25:111][boot][I] fd:0x40025b20,fd_num:0
[15:49:25:184][boot][I] fd:0x40025b20,fd_num:0
[15:49:25:338][boot][I] suc update ^_^
```
