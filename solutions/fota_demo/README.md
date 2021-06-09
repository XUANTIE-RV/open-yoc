# 概述

基于盘古C2和cb5654的fota示例

# 编译

The demo uses pangu chip sdk by default, if you want to switch to another chip sdk, 
please select one chip sdk and update the dependent one in package.yaml

Note: please select CPU ID in compiler->CPUID tab when using CDK

```bash
make clean;make
```
默认为盘古C2，如下更换开发板，增加BOARD参数
```bash
make BOARD=cb5654
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

## 云端配置

- 从occ上将许可证下载下来，对应的hex文件位kp，必须更新
- 将本地生成的image.zip，放入occ上并发布，重新下载occ生成的image.zip，并烧入板中

## fota配置
系统默认60秒检查一次升级，升级需要进行如下配置  
```
#设置设备ID
kv set device_id 3a1ad548044000006230e778e3b3ec26

**注意** tee版本的，设备id不用设置,比如盘古C2，需要将kp烧入至efuse

#设置产品类型
kv set model Model2

#使能FOTA检测
kv setint fota_en 0/1

#若调整ota服务器，参考如下命令，默认就是如下URL
kv set otaurl http://occ.t-head.cn/api/image/ota/pull

```
设置完毕重启开发板


