## Title:  HRS API
# 1. 概述

BLE SDK提供的HRS组件，完成了GATT HRS服务的注册和蓝牙协议栈的事件处理，应用程序只需实现心率值的采集，再通过HRS组件的API即可将心率值更新至Client端。

# 2. 服务介绍

HRS（Heart Rate Service）是Bluetooth SIG组织定义的标准服务之一，提供了心率测量、RR区间、能量累计、皮肤接触等功能。HRS服务不依赖任何其他服务可独立存在。一个设备可以有多个实例。

HRS服务Primary Service定义：

| 服务名称           | UUID   | Specification Level |
| ------------------ | ------ | ------------------- |
| Heart Rate Service | 0x180D | Adopted             |

HRS服务包含如下表所示特征：

| 编号 | 特征                     | UUID   | 属性   | 说明                                                         | Requirement |
| ---- | ------------------------ | ------ | ------ | ------------------------------------------------------------ | ----------- |
| 1    | Heart Rate Measurement   | 0x2A37 | NOTIFY | 该特征占2Bytes；<br />Byte0标识心率数据字节数、是否支持皮肤接触测试、是否支持RR测试及是否支持能量消耗等功能；<br />Byte1为传感器数据； | Mandatory   |
| 2    | Body Sensor Location     | 0x2A38 | READ   | 传感器位置                                                   | Optional    |
| 3    | Heart Rate Control Point | 0x2A39 | WRITE  | 用于Client端设置传感器位置信息                               | C.1         |

C.1: Mandatory if the Energy Expended feature is supported, otherwise excluded.

服务定义请参照协议文档[HEART RATE SERVICE](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=244369) 【Service Characteristics】章节。

目前HRS组件仅实现了心率测量功能，开发者可自行实现HRS服务的其他功能。

# 3.  交互流程

HRS组件处理流程如下图所示：

![交互流程图](../../../../assets/zh-cn/bluetooth/hrs/hrs_procedure.png)

- Server端应用程序初始化HRS组件
- HRS组件完成GATT HRS服务的注册、蓝牙协议栈事件回调函数的注册
- Server端应用程序发起广播
- Client端扫描蓝牙设备，并发起连接建立请求
- 连接成功建立后，HRS组件完成Primary Service以及该服务包含的各个特征属性的发现
- Server端应用程序实现心电传感器的驱动和数据采集
- Server端应用程序调用HRS组件的hrs_measure_level_update()函数，将采集数据记录至HRS组件
- Client端获取心率值，方式有两种：
  - 设置特征属性为Notify，心率值通过HRS组件的GATT Notify操作通知Client端
  - Client端通过GATT Read操作直接获取心率值

# 4. 接口定义

### **ble_prf_hrs_init**

- 函数原型

```c
hrs_handle_t ble_prf_hrs_init(hrs_t *hrs)
```

- 功能描述

完成HRS服务UUID码、特征定义和属性设置，完成蓝牙协议栈事件回调函数注册

- 参数描述

| IN/OUT | NAME       | DESC                                       |
| ------ | ---------- | ------------------------------------------ |
| [in]   | hrs_t *hrs | HRS服务结构体链表指针；参见hrs_t结构体定义 |

| hrs_t（结构体）定义  |                                                              |
| -------------------- | ------------------------------------------------------------ |
| uint16_t conn_handle | 蓝牙连接句柄，记录当前实例的蓝牙连接句柄                     |
| uint16_t svc_handle  | HRS服务句柄，记录当前实例的HRS服务句柄                       |
| int16_t ccc          | Client Characteristic Configuration，记录当前特征值的属性<br />（GATT Read、GATT Write、 GATT Notify、 GATT Indication） |
| uint8_t flag         | 标识心率数据字节数、是否支持皮肤接触测试、是否支持RR测试及是否支持能量消耗等功能 |
| uint8_t level        | 记录传感器数据                                               |
| slist_t next         | 链表指针                                                     |

- 返回值

| 返回值       |                     |
| ------------ | ------------------- |
| hrs_handle_t | 返回HRS组件句柄指针 |

- 注意事项

  无

### **ble_prf_hrs_measure_level_update**

- 函数原型

```c
int ble_prf_hrs_measure_level_update(hrs_handle_t handle, uint8_t *data, uint8_t length)
```

- 功能描述

心率值更新至HRS组件。如果Notify属性被Client端使能，则Notify心率值至Client端。

- 参数描述

| IN/OUT | NAME                | DESC            |
| ------ | ------------------- | --------------- |
| [in]   | hrs_handle_t handle | HRS组件句柄指针 |
| [in]   | uint8_t *data       | 传感器数据      |
| [in]   | uint8_t length      | 传感器数据长度  |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 更新成功 |
| 非0    | 更新失败 |

- 注意事项

  无

具体示例代码可以参见[BLE HRS Device应用开发](../../ble/BLE_HRS.md)中【心率数据的更新上报】章节。