# 概述

基于cb5654和盘古的wifi示例

# 编译

The demo uses pangu chip sdk by default, if you want to switch to another chip sdk, 
please select one chip sdk and update the dependent one in package.yaml

Note: please select CPU ID in compiler->CPUID tab when using CDK

```bash
make clean;make    //默认编译盘古
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




