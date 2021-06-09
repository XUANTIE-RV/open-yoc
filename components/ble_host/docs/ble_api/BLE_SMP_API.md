## Title:  SMP

# 概述

SMP即Security Manage Protocol，是蓝牙用来进行安全管理得部分，其定义了配对和密钥分发的过程实现。SMP被用在LE-only设备或蓝牙双模设备中。

本部分定义了蓝牙SMP接口，利用这些接口可以实现配对和密钥分发等功能。

# 接口定义

### **ble_stack_iocapability_set**

- 函数原型

```c
int ble_stack_iocapability_set(uint8_t io_cap)
```

- 功能描述

蓝牙接口I/O能力参数设置

- 参数描述

| IN/OUT | NAME           | DESC                |
| ------ | -------------- | ------------------- |
| [in]   | uint8_t io_cap | 蓝牙I/O接口能力描述 |

| 蓝牙接口能力描述（枚举）定义 |                        |
| ---------------------------- | ---------------------- |
| IO_CAP_IN_NONE               | 蓝牙设备无输入能力     |
| IO_CAP_IN_YESNO              | 蓝牙设备支持确认或取消 |
| IO_CAP_IN_KEYBOARD           | 蓝牙设备支持键盘输入   |
| IO_CAP_OUT_DISPLAY           | 蓝牙设备支持输出显示   |
| IO_CAP_OUT_NONE              | 蓝牙设备没有输出能力   |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 设置成功 |
| 非0    | 设置失败 |

- 注意事项

  蓝牙接口能力描述值应该为IO_CAP_IN_...和IO_CAP_OUT_...参数的组合

### **ble_stack_smp_passkey_entry**

- 函数原型

```c
int ble_stack_smp_passkey_entry(int16_t conn_handle, uint32_t passkey)

```

- 功能描述

设置蓝牙设备passkey

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | int16_t conn_handle | 蓝牙连接句柄 |
| [in]   | uint32_t passkey    | 配对密钥     |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_smp_cancel**

- 函数原型

```c
int ble_stack_smp_cancel(int16_t conn_handle)

```

- 功能描述

取消蓝牙配对

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | int16_t conn_handle | 蓝牙连接句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_smp_passkey_confirm**

- 函数原型

```c
int ble_stack_smp_passkey_confirm(int16_t conn_handle)

```

- 功能描述

蓝牙连接密钥确认

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | int16_t conn_handle | 蓝牙连接句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_smp_pairing_confirm**

- 函数原型

```c
int ble_stack_smp_pairing_confirm(int16_t conn_handle)

```

- 功能描述

蓝牙连接配对确认

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | int16_t conn_handle | 蓝牙连接句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_setting_load**

- 函数原型

```c
int ble_stack_setting_load()

```

- 功能描述

协议栈设置信息（密钥等）初始化

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_dev_unpair**

- 函数原型

```c
int ble_stack_dev_unpair(dev_addr_t *peer_addr)

```

- 功能描述

取消蓝牙设备配对

- 参数描述

| IN/OUT | NAME                  | DESC                                                         |
| ------ | --------------------- | ------------------------------------------------------------ |
| [in]   | dev_addr_t *peer_addr | 已配对成功的蓝牙设备地址，参见ble_stack_init接口中dev_addr_t定义，传入NULL清除所有配对设备信息 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_enc_key_size_get**

- 函数原型

```c
int ble_stack_enc_key_size_get(int16_t conn_handle)
```

- 功能描述

获取连接密钥长度

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | int16_t conn_handle | 蓝牙连接句柄 |

- 返回值

| 返回值 |                                 |
| ------ | ------------------------------- |
| >= 0   | 成功，返回密钥长度。0表示未加密 |
| < 0    | 失败                            |

- 注意事项

  无

### **ble_stack_security**

- 函数原型

```c
int ble_stack_security(int16_t conn_handle, security_en level)
```

- 功能描述

设置蓝牙协议栈安全等级

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | int16_t conn_handle | 蓝牙连接句柄 |
| [in]   | security_en level   | 连接安全等级 |

| security_en（枚举）定义 |                  |
| ----------------------- | ---------------- |
| SECURITY_LOW            | 无加密无鉴权     |
| SECURITY_MEDIUM         | 有加密无鉴权     |
| SECURITY_HIGH           | 有加密有鉴权     |
| SECURITY_FIPS           | 采用FIPS加密算法 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无