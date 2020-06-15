# 概述

基于cb2201的alimqtt上云示例, 示例中上云数据展示为温湿度及灯状态，可通过occ平台控制灯开关

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

```
kv set wifi_ssid TEST
```

```
kv set wifi_psk test1234
```

设置完毕重启开发板

### 设置mac地址

```
iwpriv wifi_debug set_mac 00e04c87A080
```

设置完毕重启开发板

### 三要素设置
打开从OCC平台获取的三要素文档`XXXX_cloudparm.txt`,使用 `factory setali` 对应的命令即可

设置完毕重启开发板

