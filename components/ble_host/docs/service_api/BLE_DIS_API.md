## Title:  DIS API
---

# 1. 概述

BLE SDK提供的DIS组件，完成了GATT DIS服务的注册和蓝牙协议栈的事件处理，应用程序只需通过组件提供的API配置相关设备信息参数，Client端即可来读取信息内容。

# 2. 服务介绍

DIS（Device Information Service）是Bluetooth SIG组织定义的标准服务之一，开发者可通过DIS服务获取设备的制造商信息、软硬件版本信息以及生产信息。DIS服务不依赖任何其他服务可独立存在，一个设备只能有一个实例。

DIS服务Primary Service定义：

| 服务名称                   | UUID   | Specification Level |
| -------------------------- | ------ | ------------------- |
| Device Information Service | 0x180A | Adopted             |

DIS服务包含如下表所示特征：

| 编号 | 特征                                                | UUID   | 属性 | 说明                                                         | Requirement |
| ---- | --------------------------------------------------- | ------ | ---- | ------------------------------------------------------------ | ----------- |
| 1    | Manufacturer Name                                   | 0x2A29 | READ | 设备制造商的名称                                             | Optional    |
| 2    | Model Number                                        | 0x2A24 | READ | 设备型号                                                     | Optional    |
| 3    | Serial Number                                       | 0x2A25 | READ | 设备编号                                                     | Optional    |
| 4    | Hardware Revision                                   | 0x2A27 | READ | 设备硬件版本号                                               | Optional    |
| 5    | Firmware Revision                                   | 0x2A26 | READ | 设备固件版本号                                               | Optional    |
| 6    | Software Revision                                   | 0x2A28 | READ | 设备应用软件版本号                                           | Optional    |
| 7    | System ID                                           | 0x2A23 | READ | 包含OUI( Oranizationally Unique Identifier)和一个由制造商为该产品编排的唯一编号 | Optional    |
| 8    | IEEE 11073-20601 Regulatory Certification Data List | 0x2A2A | READ | 设备要求的一个混合结构体，长度不定。这里的数据通常用于监管或者认证 | Optional    |
| 9    | PnP ID                                              | 0x2A50 | READ | 包含由国标组织颁给制造商的编号，制造商定义的产品ID,由制造商定义的产品版本信息 | Optional    |

服务定义请参照协议文档[Device Information Service 1.1](https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=244369) 【Service Characteristics】章节。

# 3.  交互流程

DIS组件处理流程如下图所示：

![交互流程图](../../../../assets/zh-cn/bluetooth/dis/dis_procedure.png)

- Server端应用程序初始化时，将设备信息设置到DIS组件
- DIS组件完成GATT服务的注册、蓝牙协议栈事件回调函数的注册
- Server端应用程序发起广播
- Client端扫描蓝牙设备，并发起连接建立请求
- 连接成功建立后，DIS组件完成Primary Service以及该服务包含的各个特征属性的发现
- Client端通过GATT Read操作获取设备信息

# 4. 接口定义

### **ble_prf_dis_init**

- 函数原型

```c
dis_handle_t ble_prf_dis_init(dis_info_t *info)
```

- 功能描述

记录设备相关信息，完成DIS服务UUID码、特征定义和属性设置，完成蓝牙协议栈事件回调函数注册

- 参数描述

| IN/OUT | NAME             | DESC                                        |
| ------ | ---------------- | ------------------------------------------- |
| [in]   | dis_info_t *info | DIS服务结构体指针；参见dis_info_t结构体定义 |

| dis_info_t（结构体）定义                          |                                                       |
| ------------------------------------------------- | ----------------------------------------------------- |
| char *manufacturer_name                           | 设备制造商的名称                                      |
| char *model_number                                | 设备型号                                              |
| char *serial_number                               | 设备编号                                              |
| char *hardware_revison                            | 设备硬件版本                                          |
| char *firmware_revision                           | 设备固件版本                                          |
| char *software_revision                           | 设备应用软件版本                                      |
| system_id_t *system_id                            | 系统编号，见system_id_t结构体定义                     |
| regulatory_cert_data_list_t  *regu_cert_data_list | 监管认证信息，见regulatory_cert_data_list_t结构体定义 |
| pnp_id_t *pnp_id                                  | 制造商信息，见pnp_id_t结构体定义                      |

| system_id_t（结构体）定义           |                    |
| ----------------------------------- | ------------------ |
| uint64_t manufacturer_id            | 设备制造商ID       |
| uint32_t organizationally_unique_id | 设备制造商唯一编号 |

| regulatory_cert_data_list_t（结构体）定义 |                  |
| ----------------------------------------- | ---------------- |
| uint8_t *list                             | 监管认证信息     |
| uint8_t  list_len                         | 监管认证信息长度 |

| pnp_id_t（结构体）定义    |                          |
| ------------------------- | ------------------------ |
| uint8_t  vendor_id_source |                          |
| uint16_t vendor_id        | 国标组织颁给制造商的编号 |
| uint16_t product_id       | 制造商定义的产品ID       |
| uint16_t product_version  | 制造商定义的产品版本信息 |

- 返回值

| 返回值       |                            |
| ------------ | -------------------------- |
| dis_handle_t | 返回dis_handle_t结构体指针 |

- 注意事项

  无

具体示例代码可以参见[BLE HID Keyboard应用开发](../../ble/BLE_HID_Keyboard_Development.md)中【设备信息配置】章节。