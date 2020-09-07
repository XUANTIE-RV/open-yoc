# ALIMQTT 演示方案

该方案通过连接阿里云物联网linkkit mqtt，将模拟的传感器数据传到云端，同时云端可以下发数据控制端侧的led灯。

我们需要访问 [阿里云物联网网站](https://www.aliyun.com/product/iot-deviceconnect?spm=5176.224200.h2v3icoap.287.a41b6ed66rrpCI&aly_as=xGfvqdo-)

并完成 注册账号->创建产品->申请设备三元组，等一系列操作


# 参数设置
## 1.1 连接路由器

**命令行配置**
```
#重启生效
kv set wifi_ssid {wifi_ssid}
kv set wifi_psk {wifi_psk}
```
## 1.2 设置 阿里云物联网 三元组

**命令行配置**
```
#重启生效
kv set DEVICENAME {DEVICENAME}
kv set DEVICESECRET {DEVICESECRET}
kv set PRODUCTKEY {DEVICESECRET}
```

# Topic 设置与订阅

默认设备端的 publish 和 subscribe 是分別 
```
/{product_key}/{device_name}/thing/event/property/post
/{product_key]/{device_name}/thing/service/property/set
```

可以通过 `iot_channel_open` 接口传入参数修改

