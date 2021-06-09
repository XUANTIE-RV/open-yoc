# 概述

基于`ch2601`芯片的FOTA升级的DEMO。FOTA的云服务在OCC，包括固件的管理，许可证的管理，设备的管理等。
具体可以参考OCC上的博文《RVB2601应用开发实系列四: FOTA镜像升级》

# 使用

## 通过CDK

在CDK的首页，通过搜索ch2601，可以找到CH2601_FOTA_DEMO，然后创建工程。

CDK的使用可以参考YoCBook 《CDK开发快速上手》 章节， 链接 https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html

具体步骤参考OCC上的博文《RVB2601应用开发实系列四: FOTA镜像升级》
## 通过命令行

### 下载

```bash
mkdir workspace
cd workspace
yoc init
yoc install ch2601_fota_demo
```

### 编译

```bash
make clean;make
```

### 烧录

```bash
make flashall
```

从服务器上下载镜像包镜像烧录：

```bash
product flash 20210304215611061_factory.zip -a -f ch2601_flash.elf -x gdbinit
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit_debug
```

# 联网配置

通过串口终端输入:

```cli
kv set wifi_ssid <your_wifi_ssid>
kv set wifi_psk <your_wifi_password>
```

- your_wifi_ssid：你的wifi名字
- your_wifi_password：你的wifi密码

# fota配置

系统默认60秒检查一次升级，升级需要进行如下配置

## 设置设备ID

ID号从OCC上获取。

通过串口终端输入:

```cli
kv set device_id 3a1ad548044000006230e778e3b3ec26
```

## 设置产品类型

通过串口终端输入:

```cli
kv set model ch2601
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
16:03:45:058--[ 109.770]<D>fota ##read: 4096
16:03:46:076--[ 110.780]<D>app_fota FOTA PROGRESS :2, 188416, 193736
16:03:46:081--[ 110.790]<D>fota FOTA:97%
16:03:46:103--[ 110.810]<D>w800_api w800 start send(221)(time:9990)
16:03:46:103--
16:03:46:210--[ 110.920]<D>w800_api w800 end send(ret:0)
16:03:46:210--
16:03:48:375--[ 113.090]<D>fota ##read: 4096
16:03:49:388--[ 114.100]<D>app_fota FOTA PROGRESS :2, 192512, 193736
16:03:49:388--[ 114.100]<D>fota FOTA:99%
16:03:49:412--[ 114.120]<D>w800_api w800 start send(221)(time:9990)
16:03:49:412--
16:03:49:520--[ 114.230]<D>w800_api w800 end send(ret:0)
16:03:49:520--
16:03:49:715--[ 114.430]<D>fota ##read: 1224
16:03:50:048--[ 114.750]<D>app_fota FOTA PROGRESS :2, 193736, 193736
16:03:50:048--[ 114.750]<D>fota FOTA:100%
16:03:50:052--[ 114.760]<D>fota http_read done: 193736 193736
16:03:50:052--[ 114.760]<D>fota ##read: 0
16:03:50:052--[ 114.760]<D>app_fota FOTA VERIFY :4
16:03:50:053--[ 114.770]<D>fotav start fota verify...
16:03:50:076--[ 114.770]<D>fotav image_size:193324
16:03:50:076--[ 114.770]<D>fotav digest_type:1
16:03:50:076--[ 114.780]<D>fotav sign_type:1
16:03:50:076--[ 114.780]<D>fotav hash_len:20
16:03:50:076--[ 114.780]<D>fotav signature_len:128
16:03:50:076--[ 114.780]<D>fotav signature_offset:201516
16:03:50:083--[ 114.790]<D>fotav hash_offset:201772
16:03:50:083--[mtb][D] [hash_calc_start, 215]ds:1, ilen:193324, from_mem:0
16:03:50:224--[mtb][D] [hash_calc_start, 242]sha ok, type:1, outlen:20
16:03:50:224--[ 114.940]<I>fotav ###fota data hash v ok.
16:03:50:249--[ 114.960]<D>w800_api socket[0] closed
16:03:50:249--
16:03:50:263--[ 114.980]<D>app_fota FOTA FINISH :4
16:03:50:339--
16:03:50:339--Welcome boot2.0!
16:03:50:339--build: Mar  5 2021 15:34:41
16:03:50:468--[boot][I] fota data hash verify ok
16:03:50:468--[boot][I] start to upgrade
16:03:52:469--[boot][I] fd:20000d20,fd_num:0
16:03:54:447--[boot][I] start FULL update
16:04:40:223--[boot][I] fd:20000d20,fd_num:0
16:04:42:201--[boot][I] fd:20000d20,fd_num:0
16:04:46:163--[boot][I] suc update ^_^
16:04:46:163--
16:04:46:718--
16:04:46:718--Welcome boot2.0!
16:04:46:718--build: Mar  5 2021 15:34:41
16:04:46:725--load img & jump to [prim]
16:04:46:725--load&jump 0x18017000,0x18017000,189228
16:04:46:725--xip...
16:04:46:726--j 0x18017044
```