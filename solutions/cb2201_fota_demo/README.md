# 概述

基于cb2201的fota示例

# 编译

```bash
make clean;make
```

# 烧录

烧写所有分区

```bash
make flashall
```

烧写prim分区

```bash
make flash
```

**注意** 当前目录下必须要有`.gdbinit`

# 命令

## 网络配置

### 设置wifi ssid和密码

比如待连接的wifi账号是“TEST”,密码为“test1234”

```cli
kv set wifi_ssid TEST
```

```cli
kv set wifi_psk test1234
```

设置完毕重启开发板.

## 云端配置

- 从occ上将许可证下载下来，对应的二进制/hex文件为kp内容，必须更新；将二进制文件重命名为dev_kp，hex文件重命名为dev_kp.hex，放当前目录下。

- 将本地生成的image.zip，放入occ上并发布，重新下载occ生成的image.zip，并烧入板中

## fota配置

系统默认60秒检查一次升级，升级需要进行如下配置

### 设置设备ID

```cli
kv set device_id 3a1ad548044000006230e778e3b3ec26
```

**注意** tee版本的，设备id不用设置， 默认为TEE版本。

使用CDK烧录完成dev_kp.hex之后，需要重新烧录整个镜像。

### 设置产品类型

```cli
kv set model ch2201
```

### 使能FOTA检测

kv setint fota_en 0/1

### fota url

若调整ota服务器，参考如下命令，默认就是如下URL

```cli
kv set otaurl http://occ.t-head.cn/api/image/ota/pull
```

设置完毕重启开发板.
