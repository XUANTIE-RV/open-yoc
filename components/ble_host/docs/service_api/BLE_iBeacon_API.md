## Title:  iBeacon API
# 1. 概述

BLE SDK提供的iBeacon组件，完成了iBeacon包的封装和广播，应用程序只需关心何时启动或停止广播iBeacon包。

# 2. 功能介绍

iBeacon  是苹果公司2013年9月发布的移动设备用OS（iOS7）上配备的新功能。其工作方式是，配备有低功耗蓝牙（BLE）通信功能的设备使用BLE技术向周围发送自己特有的ID，接收到该ID的应用软件会根据该ID采取一些行动。iBeacon主要有四类数据构成，分别是UUID（通用唯一标识符）、Major、Minor、Measured Power。

- UUID是规定为ISO/IEC11578:1996标准的128位标识符。

- Major和Minor由iBeacon发布者自行设定，都是16位的标识符。比如，连锁店可以在Major中写入区域资讯，可在Minor中写入个别店铺的ID等。另外，在家电中嵌入iBeacon功能时，可以用Major表示产品型号，用Minor表示错误代码，用来向外部通知故障。

- Measured Power是iBeacon模块与接收器之间相距1m时的参考接收信号强度（RSSI：Received Signal Strength Indicator）。接收器根据该参考RSSI与接收信号的强度来推算发送模块与接收器的距离。

应用程序需要将这些参数设置到iBeacon组件，就可以开启蓝牙广播功能了。

# 3. 接口定义

### **ble_prf_ibeacon_start**

- 函数原型

```c
int ble_prf_ibeacon_start(uint8_t _id[2], uint8_t _uuid[16], uint8_t _major[2], 
                  uint8_t _minor[2], uint8_t _measure_power, char *_sd)
```

- 功能描述

设置iBeacon参数信息，并开启广播功能

- 参数描述

| IN/OUT | NAME                   | DESC                                              |
| ------ | ---------------------- | ------------------------------------------------- |
| [in]   | uint8_t _id[2]         | iBeacon ID                                        |
| [in]   | uint8_t _uuid[16]      | 用户自定义UUID                                    |
| [in]   | uint8_t _major[2]      | iBeacon数据主标识                                 |
| [in]   | uint8_t _minor[2]      | iBeacon数据副标识                                 |
| [in]   | uint8_t _measure_power | iBeacon模块与接收器之间相距1m时的参考接收信号强度 |
| [in]   | char *_sd              | 设备名称                                          |

- 返回值

| 返回值 |                     |
| ------ | ------------------- |
| 0      | 成功开启iBeacon功能 |
| ！= 0  | 失败                |

- 注意事项

  无

### **ble_prf_ibeacon_stop**

- 函数原型

```c
int ble_prf_ibeacon_stop(void)
```

- 功能描述

停止iBeacon广播

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无
