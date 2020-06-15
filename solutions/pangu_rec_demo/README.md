# 概述

基于`pangu`的录制demo

# 编译

```bash
make clean;make
```

# 烧录

```bash
make flashall
```

# 服务器

服务器脚本在`server`目录下

## 启动服务器

```bash
python2 server.py 192.168.1.104:9090 ./download
```

`192.168.1.104`为服务器HOST的IP地址，断开固定为`9090`


# 应用

## 开始

待设备启动，且串口有如下打印时，说明网络已经准好了。
```
[     5.673028][I][netmgr  ]IP: 192.168.1.105
[     5.677022][D][app     ]Net up
```

cli输入：

```bash
rec start ws://192.168.1.104:9090 savenamexxdd.pcm
```

## 停止

cli输入：

```bash
rec stop
```