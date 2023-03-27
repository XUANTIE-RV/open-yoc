## Title:  BAS API
# 1. 概述

BLE SDK提供的BAS组件，完成了GATT BAS服务的注册和蓝牙协议栈的事件处理，应用程序只需实现电池电量值的采集，再通过BAS组件的API即可将电量更新至Client端。

# 2. 服务介绍

BAS（Battery Service）是Bluetooth SIG组织定义的标准服务之一，开发者可通过BAS服务获取设备电池电量信息。BAS服务不依赖任何其他服务可独立存在，一个设备可以有多个实例。

BAS服务Primary Service定义：

| 服务名称        | UUID   | Specification Level |
| --------------- | ------ | ------------------- |
| Battery Service | 0x180F | Adopted             |

BAS服务包含如下表所示特征：

| 编号 | 特征          | UUID   | 说明       |
| ---- | ------------- | ------ | ---------- |
| 1    | Battery Level | 0x2A19 | 电量百分比 |

服务定义请参照协议文档[Battery Service 1.1](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=245138)【Service Characteristics】章节。

# 3.  交互流程

BAS组件处理流程如下图所示：

![交互流程图](../../../../assets/zh-cn/bluetooth//bas/bas_procedure.png)

- Server端应用程序初始化BAS组件
- BAS组件完成GATT服务的注册、蓝牙协议栈事件回调函数的注册
- Server端应用程序发起广播
- Client端扫描蓝牙设备，并发起连接建立请求
- 连接成功建立后，BAS组件完成Primary Service以及该服务包含的各个特征属性的发现
- Server端应用程序调用BAS组件的bas_level_update()函数，将设备电量值记录至BAS组件
- Server端应用程序实现电池电量的采集
- Client端获取电量，方式有两种：
  - 设置特征属性为Notify，设备电量值通过BAS组件的GATT Notify操作通知Client端
  - Client端通过GATT Read操作直接获取设备电量值

# 4. 接口定义

### **ble_prf_bas_init**

- 函数原型

```c
bas_handle_t ble_prf_bas_init(bas_t *bas)
```

- 功能描述

完成BAS服务UUID码、特征定义和属性设置，完成蓝牙协议栈事件回调函数注册

- 参数描述

| IN/OUT | NAME       | DESC                                       |
| ------ | ---------- | ------------------------------------------ |
| [in]   | bas_t *bas | BAS服务结构体链表指针；参见bas_t结构体定义 |

| bas_t（结构体）定义     |                                                              |
| ----------------------- | ------------------------------------------------------------ |
| uint16_t conn_handle    | 蓝牙连接句柄，记录当前实例的蓝牙连接句柄                     |
| uint16_t bas_svc_handle | BAS服务句柄，记录当前实例的BAS服务句柄                       |
| int16_t ccc             | Client Characteristic Configuration，记录当前特征的属性<br />（GATT Read、GATT Write、 GATT Notify、 GATT Indication） |
| uint8_t battery_level   | 电量百分比，记录当前电量值                                   |
| slist_t next            | 链表指针                                                     |

- 返回值

| 返回值       |                     |
| ------------ | ------------------- |
| bas_handle_t | 返回BAS组件句柄指针 |

- 注意事项

  无

### **ble_prf_bas_level_update**

- 函数原型

```c
int ble_prf_bas_level_update(bas_handle_t handle, uint8_t level)
```

- 功能描述

电量更新更新至BAS组件。如果Notify属性被Client端使能，则Notify电量值至Client端。

- 参数描述

| IN/OUT | NAME                | DESC            |
| ------ | ------------------- | --------------- |
| [in]   | bas_handle_t handle | BAS组件句柄指针 |
| [in]   | uint8_t level       | 电量百分比      |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 更新成功 |
| 非0    | 更新失败 |

- 注意事项

  无

具体示例代码可以参见[BLE HID Keyboard应用开发](../../ble/BLE_HID_Keyboard_Development.md)中【电池电量的更新上报】章节。

