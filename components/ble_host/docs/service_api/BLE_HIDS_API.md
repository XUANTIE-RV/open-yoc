## Title:  HIDS API
# 1. 概述

BLE SDK提供的HIDS组件，完成了GATT HIDS服务的注册、蓝牙协议栈的事件处理以及输出报告的事件上报，应用程序只需关心报告描述符的定义、输出报告事件的处理，通过HIDS组件的API将设备数据更新至Client端，即可实现人机交互设备的开发。

# 2. 服务介绍

HIDS（Human Interface Device Service）是Bluetooth SIG组织定义的标准服务之一，定义了常见的鼠标、键盘、游戏手柄等人机交互设备的蓝牙数据通信规范。

## 2.1. Primary Service定义

| 服务名称                       | UUID   | Specification Level |
| ------------------------------ | ------ | ------------------- |
| Human Interface Device Service | 0x1812 | Adopted             |

## 2.2. 特征定义

| 特征                        | UUID   | 属性                              | 说明                                                         | Requirement |
| --------------------------- | ------ | --------------------------------- | ------------------------------------------------------------ | ----------- |
| Protocol Mode               | 0x2A4E | READ、WriteWithoutResponse        | 指示当前HIDS服务是Boot Protocol模式还是Report Protocol模式   | C.4         |
| Report                      | 0x2A4D |                                   | 该特征用来传输输入报告、输出报告和Feature Report数据，通过Report Reference Characteristic Descriptor区分不同Report类型 | Optional    |
| Report:Input Report Type    |        | READ、NOTIFY                      | 输入报告                                                     | C.1         |
| Report:Output Report Type   |        | READ、WRITE、WriteWithoutResponse | 输出报告                                                     | C.1         |
| Report:Feature Report Type  |        | READ、WRITE                       | Feature Report                                               | C.1         |
| Report Map                  | 0x2A4B | READ                              | 描述Report Protocol模式下传输的Report的数据格式和用途        | Mandatory   |
| Boot Keyboard Input Report  | 0x2A22 | READ、NOTIFY                      | 当 BLE HID设备是鼠标或者键盘时，并且工作在Boot Protocol 模式下时，可以用该特征来传输键盘按键值，其格式和长度是固定的 | C.2         |
| Boot Keyboard Output Report | 0x2A32 | READ、WRITE、WriteWithoutResponse | 当 BLE HID设备是鼠标或者键盘时，并且工作在Boot Protocol 模式下时，可以用该特征来传输键盘指示灯状态，其格式和长度是固定的 | C.2         |
| Boot Mouse Input Report     | 0x2A33 | READ、WRITE                       | 当 BLE HID 设备是鼠标或者键盘时，并且工作在Boot Protocol 模式下时，可以用该特征来传输鼠标数据，其格式和长度是固定的 | C.3         |
| HID Information             | 0x2A4A | READ                              | 记录BLE HID设备信息，包括版本号、设备厂商的国家识别码、标识等 | Mandatory   |
| HID Control Point           | 0x2A4C | WriteWithoutResponse              | 指示 HID HOST 的状态，如挂起或者唤醒状态                     | Mandatory   |

```
C.1: Mandatory to support at least one Report Type if the Report characteristic is supported
C.2: Mandatory for HID Devices operating as keyboards, else excluded.
C.3: Mandatory for HID Devices operating as mice, else excluded.
C.4: Mandatory for HID Devices supporting Boot Protocol Mode, otherwise optional.
```

服务定义请参照协议文档[Human Interface Device Service 1.0](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=245140)【Service Characteristics】章节。

## 2.2.1. Protocol Mode

HID设备存在两种协议模式：Boot Protocol和Report Protocol。该特征设置为Boot Protocol的HID设备，报告描述符的数据格式固定，所以无须设置报告描述符。而设置为Report Protocol的HID设备，需要定义报告描述符的数据格式，这样Host端才能正确解析报告数据的内容。Protocol Mode为Read属性。

## 2.2.2. Report

该特征用来传输Input Report、Output Report和Feature Report。一般BLE HIDS服务下会存在多个Report，需要通过Report的描述符（Report Reference Characteristic Descriptor）来区分不同的Report。比如蓝牙键盘通常会传输键盘按键和键盘指示灯控制，那么可以设置BLE HIDS服务包含两个Report，一个是用来传输键盘按键的Input Report，需要设置Notify属性；另一个是用来传输键盘指示灯控制的Output Report，需要设置Write属性。

Report特新的描述符占位2字节，分别表示Report ID和Report Type，如下图所示：

![Report Property](../../../../assets/zh-cn/bluetooth//hids/HID_keyboard_report_desc.png)

下图是蓝牙键盘的Report特征属性表：

![Report Property](../../../../assets/zh-cn/bluetooth//hids/HID_keyboard_report_property.png)

## 2.2.3. Report Map

Report Map特征被称为报告描述符，其作用是定义HID设备工作在Report Protocol模式下时，传输的Report的数据格式和用途，比如设备用途定义为键盘等。

# 3.  交互流程

上一章节介绍了HIDS服务的基本知识，本章节将简单描述下HIDS组件的业务流程，如下图所示：

![交互流程图](../../../../assets/zh-cn/bluetooth//hids/hids_procedure.png)

- Server端应用程序调用hids_init（）接口函数初始化HIDS组件，设置工作模式为Boot Protocol或者Report Protocol
- HIDS组件完成GATT服务的注册、蓝牙协议栈事件回调函数的注册
- Server端应用程序调用set_data_map（）接口函数设置自定义的报告描述符、Input Report和Output Report的数据缓冲区至HIDS组件
- Server端应用程序注册Output Report事件的回调处理函数
- Server端应用程序发起广播
- Client端扫描蓝牙设备，并发起连接建立请求
- 连接成功建立后，HIDS组件完成Primary Service以及该服务包含的各个特征属性的发现
- Client端设置Input Report特征的属性为Notify，使得Server端应用程序在需要键值传送时可以主动发送至Client端
- Server端应用程序调用ble_prf_hids_key_send（）接口函数将采集的数值传递至HIDS组件，该数值将通过GATT Notify操作通知到Client端。以蓝牙键盘为例，应用程序只需实现键盘驱动和键值采集，通过HIDS组件的接口即可发送键值至Client端。
- Client端通过GATT Write操作设置Output Report特征，比如蓝牙键盘的指示灯控制
- Client端还可以通过GATT Read操作来读取Server端各个特征的Value和Description，比如Input/Output Report的Value和Description、Protocol Mode Value、HID Information、HID Control Point等

# 4. 接口定义

### **ble_prf_hids_init**

- 函数原型

```c
hids_handle_t ble_prf_hids_init(uint8_t mode)
```

- 功能描述

设置HID设备工作模式，完成HIDS服务UUID码、特征值定义和属性设置，完成蓝牙协议栈事件回调函数注册

- 参数描述

| IN/OUT | NAME         | DESC                                                      |
| ------ | ------------ | --------------------------------------------------------- |
| [in]   | uint8_t mode | HID设备工作模式；Boot protocol模式或者Report Protocol模式 |

| 工作模式枚举                     |
| -------------------------------- |
| HIDS_BOOT_PROTOCOL_MODE = 0x00   |
| HIDS_REPORT_PROTOCOL_MODE = 0x01 |

- 返回值

| 返回值       |                  |
| ------------ | ---------------- |
| hrs_handle_t | 返回HIDS组件句柄 |

- 注意事项

  无

### **ble_prf_hids_set_data_map**

- 函数原型

```c
int ble_prf_hids_set_data_map(uint8_t u_data[], uint16_t len, uint8_t u_type)
```

- 功能描述

设置报告描述符、Input Report、Output Report数据

- 参数描述

| IN/OUT | NAME             | DESC                                        |
| ------ | ---------------- | ------------------------------------------- |
| [in]   | uint8_t u_data[] | 报告描述符、Input Report、Output Report数据 |
| [in]   | uint16_t len     | 数据长度                                    |
| [in]   | uint8_t u_type   | 数据类型                                    |

| 数据类型枚举          |                          |
| --------------------- | ------------------------ |
| REPORT_MAP = 0x00     | 数据类型为Report Map     |
| REPORT_INPUT = 0x01   | 数据类型为Input Report   |
| REPORT_OUTPUT = 0x02  | 数据类型为Output Report  |
| REPORT_FEATURE = 0x03 | 数据类型为Feature Report |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 设置成功 |
| 非0    | 设置失败 |

- 注意事项

  无

### ble_prf_hids_key_send

- 函数原型

```c
int ble_prf_hids_key_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len)
```

- 功能描述

发送数据至Client端，数据格式由Input Report定义

- 参数描述

| IN/OUT | NAME                 | DESC             |
| ------ | -------------------- | ---------------- |
| [in]   | hids_handle_t handle | HIDS组件句柄     |
| [in]   | uint8_t *key_code    | Input Report数据 |
| [in]   | uint16_t us_len      | 数据长度         |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 更新成功 |
| 非0    | 更新失败 |

- 注意事项

  无

### ble_prf_hids_notify_send

- 函数原型

```c
int ble_prf_hids_notify_send(hids_handle_t handle, uint8_t *key_code, uint16_t us_len)
```

- 功能描述

notify形式发送数据至Client端，数据格式由Input Report定义

- 参数描述

| IN/OUT | NAME                 | DESC             |
| ------ | -------------------- | ---------------- |
| [in]   | hids_handle_t handle | HIDS组件句柄     |
| [in]   | uint8_t *key_code    | Input Report数据 |
| [in]   | uint16_t us_len      | 数据长度         |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 更新成功 |
| 非0    | 更新失败 |

- 注意事项

  无

### ble_prf_hids_regist

- 函数原型

```c
int ble_prf_hids_regist(int32_t idx, hids_event_cb cb_event)
```

- 功能描述

Hid事件回调函数注册

- 参数描述

| IN/OUT | NAME                   | DESC                                           |
| ------ | ---------------------- | ---------------------------------------------- |
| [in]   | int32_t idx            | HID事件索引                                    |
| [in]   | hids_event_cb cb_event | HID回调函数类型，参见hids_event_cb回调函数定义 |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 注册成功 |
| 非0    | 注册失败 |

- 注意事项

  无

### hids_event_cb

- 函数原型

```c
typedef void (*hids_event_cb)(hids_event_e event, void *event_data)
```

- 功能描述

HID回调函数类型，

- 参数描述

| IN/OUT | NAME               | DESC                                           |
| ------ | ------------------ | ---------------------------------------------- |
| [in]   | hids_event_e event | HID事件索引                                    |
| [in]   | void *event_data   | HID回调函数类型，参见hids_event_cb回调函数定义 |

- 返回值

  无

- 注意事项

  无

具体示例代码可以参见[BLE HID Keyboard应用开发](../../ble/BLE_HID_Keyboard_Development.md)中【键盘指示灯控制】和【键值上报】章节。

