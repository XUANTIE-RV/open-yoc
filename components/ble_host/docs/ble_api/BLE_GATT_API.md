Title:  GATT
---

# 概述

该部分定义了蓝牙通用属性规范（GATT）主要服务接口，包括服务注册、服务特性的发现、读、写、通知、指示以及服务特性的广播。

# 接口定义

### **ble_stack_gatt_registe_service**

- 函数原型

```c
int ble_stack_gatt_registe_service(gatt_att_t attrs[], uint16_t attr_num)
```

- 功能描述

蓝牙GATT层服务注册

- 参数描述

| IN/OUT | NAME               | DESC         |
| ------ | ------------------ | ------------ |
| [in]   | gatt_att_t attrs[] | 服务属性数组 |
| [in]   | uint16_t attr_num  | 服务属性个数 |

| gatt_att_t（结构体）定义 |                        |
| ------------------------ | ---------------------- |
| uuid_t * uuid            | 蓝牙服务通用唯一识别码 |
| uint8_t  perm            | 属性值权限类型         |
| union{} uuid_t           | 服务数据               |
| char_read_func_t read    | 读事件回调函数指针     |
| char_write_func_t write  | 写事件回调函数指针     |

| uuid_t（union结构体）定义 |                  |
| ------------------------- | ---------------- |
| uint8_t type              | UUID数据长度类型 |
| union{} _u                | UUID数据         |

| _u（union结构体）定义 |      |
| --------------------- | ---- |
| uint8_t val           |      |
| uint16_t _u16         |      |
| uint32_t _u32         |      |
| uint8_t _u128[16]     |      |

| perm（枚举）定义                     |                  |
| ------------------------------------ | ---------------- |
| GATT_PERM_NONE = 0                   | 不支持读写       |
| GATT_PERM_READ=BLE_BIT(0)（0x01）    | 支持读           |
| GATT_PERM_WRITE=BLE_BIT(1)（0x02）   | 支持写           |
| GATT_PERM_READ_ENCRYPT=BLE_BIT(2)    | 支持读（需加密） |
| GATT_PERM_WRITE_ENCRYPT=BLE_BIT(3)   | 支持写（需加密） |
| GATT_PERM_READ_AUTHEN = BLE_BIT(4)   | 支持读（需认证） |
| GATT_PERM_WRITE_AUTHEN = BLE_BIT(5)  | 支持写（需认证） |
| GATT_PERM_PREPARE_WRITE = BLE_BIT(6) | 多数据包准备写入 |

- 返回值

| 返回值 |              |
| ------ | ------------ |
| >= 0   | 返回服务句柄 |
| < 0    | 失败         |

- 注意事项

  无

### **ble_stack_gatt_notificate**

- 函数原型

```c
int ble_stack_gatt_notificate(int16_t conn_handle, uint16_t char_handle, uint8_t *data, uint16_t len)
```

- 功能描述

发起特性数据通知流程

- 参数描述

| IN/OUT | NAME                 | DESC         |
| ------ | -------------------- | ------------ |
| [in]   | int16_t conn_handle  | 蓝牙连接句柄 |
| [in]   | uint16_t char_handle | 属性句柄     |
| [in]   | uint8_t *data        | 数据地址     |
| [in]   | uint16_t len         | 数据长度     |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_indicate**

- 函数原型

```c
int ble_stack_gatt_indicate(int16_t conn_handle, int16_t char_handle, uint8_t *data, uint16_t len)
```

- 功能描述

发起特性数据Indication流程

- 参数描述

| IN/OUT | NAME                 | DESC         |
| ------ | -------------------- | ------------ |
| [in]   | int16_t conn_handle  | 蓝牙连接句柄 |
| [in]   | uint16_t char_handle | 属性句柄     |
| [in]   | uint8_t *data        | 数据地址     |
| [in]   | uint16_t len         | 数据长度     |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_mtu_get**

- 函数原型

```c
int ble_stack_gatt_mtu_get(int16_t conn_handle)
```

- 功能描述

获取ATT MTU大小

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | int16_t conn_handle | 蓝牙连接句柄 |

- 返回值

| 返回值 |         |
| ------ | ------- |
| int    | MTU长度 |

- 注意事项

  无

### **ble_stack_gatt_mtu_exchange**

- 函数原型

```c
int ble_stack_gatt_mtu_exchange(int16_t conn_handle)
```

- 功能描述

发起MTU协商

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

### **ble_stack_gatt_discovery**

- 函数原型

```c
int ble_stack_gatt_discovery(int16_t conn_handle, gatt_discovery_type_en type, uuid_t *uuid, uint16_t start_handle, uint16_t end_handle)
```

- 功能描述

GATT服务发现

- 参数描述

| IN/OUT | NAME                        | DESC         |
| ------ | --------------------------- | ------------ |
| [in]   | int16_t conn_handle         | 蓝牙连接句柄 |
| [in]   | gatt_discovery_type_en type | 服务发现类型 |
| [in]   | uuid_t *uuid                | 服务UUID地址 |
| [in]   | uint16_t start_handle       | 属性开始句柄 |
| [in]   | uint16_t end_handle         | 属性结束句柄 |

| gatt_discovery_type_en（枚举）定义 |                    |
| ---------------------------------- | ------------------ |
| GATT_FIND_PRIMARY_SERVICE          | 首要服务发现       |
| GATT_FIND_INC_SERVICE              | 包含服务发现       |
| GATT_FIND_CHAR                     | 服务特性发现       |
| GATT_FIND_CHAR_DESCRIPTOR          | 服务特性描述符发现 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本接口需要等到EVENT_GATT_DISCOVERY_COMPLETE事件返回后，才能进行下一轮调用，不支持并发操作。

### **ble_stack_gatt_discovery_all**

- 函数原型

```c
#define ble_stack_gatt_discovery_all(conn_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_PRIMARY_SERVICE, 0, 0x0001, 0xffff)
```

- 功能描述

发现所有首要服务

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

### **ble_stack_gatt_discovery_primary**

- 函数原型

```c
#define ble_stack_gatt_discovery_primary(conn_handle, uuid, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_PRIMARY_SERVICE, uuid, start_handle, end_handle)
```

- 功能描述

根据服务UUID发现首要服务

- 参数描述

| IN/OUT | NAME                  | DESC         |
| ------ | --------------------- | ------------ |
| [in]   | int16_t conn_handle   | 蓝牙连接句柄 |
| [in]   | uuid_t *uuid          | 服务UUID地址 |
| [in]   | uint16_t start_handle | 属性开始句柄 |
| [in]   | uint16_t end_handle   | 属性结束句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_discovery_include**

- 函数原型

```c
#define ble_stack_gatt_discovery_include(conn_handle, uuid, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_INC_SERVICE, uuid, start_handle, end_handle)
```

- 功能描述

查找指定首要服务下的包含服务

- 参数描述

  | IN/OUT | NAME                  | DESC                       |
  | ------ | --------------------- | -------------------------- |
  | [in]   | int16_t conn_handle   | 蓝牙连接句柄               |
  | [in]   | uuid_t *uuid          | 服务UUID地址               |
  | [in]   | uint16_t start_handle | 指定首要服务的属性开始句柄 |
  | [in]   | uint16_t end_handle   | 指定首要服务的属性结束句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_discovery_char_all**

- 函数原型

```c
#define ble_stack_gatt_discovery_char_all(conn_handle, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR, 0, start_handle, end_handle)
```

- 功能描述

发现服务的所有特性

- 参数描述

| IN/OUT | NAME                  | DESC               |
| ------ | --------------------- | ------------------ |
| [in]   | int16_t conn_handle   | 蓝牙连接句柄       |
| [in]   | uint16_t start_handle | 指定服务的开始句柄 |
| [in]   | uint16_t end_handle   | 指定服务的结束句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_discovery_char**

- 函数原型

```c
#define ble_stack_gatt_discovery_char(conn_handle, uuid, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR, uuid, start_handle, end_handle)
```

- 功能描述

根据指定服务的UUID发现该服务特性

- 参数描述

| IN/OUT | NAME                  | DESC           |
| ------ | --------------------- | -------------- |
| [in]   | int16_t conn_handle   | 蓝牙连接句柄   |
| [in]   | uint16_t start_handle | 服务的开始句柄 |
| [in]   | uint16_t end_handle   | 服务的结束句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_discovery_descriptor**

- 函数原型

```c
#define ble_stack_gatt_discovery_descriptor(conn_handle, uuid,start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR_DESCRIPTOR, uuid, start_handle, end_handle)
```

- 功能描述

根据指定服务的UUID发现该服务的特性描述符

- 参数描述

| IN/OUT | NAME                  | DESC               |
| ------ | --------------------- | ------------------ |
| [in]   | int16_t conn_handle   | 蓝牙连接句柄       |
| [in]   | uuid_t *uuid          | 特性描述符UUID地址 |
| [in]   | uint16_t start_handle | 开始句柄           |
| [in]   | uint16_t end_handle   | 结束句柄           |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_discovery_descriptor_all**

- 函数原型

```c
#define ble_stack_gatt_discovery_descriptor_all(conn_handle, start_handle, end_handle) \
    ble_stack_gatt_discovery(conn_handle, GATT_FIND_CHAR_DESCRIPTOR, 0, start_handle, end_handle)
```

- 功能描述

发现指定服务start_handle到end_handle句柄之间所有特性描述符

- 参数描述

| IN/OUT | NAME                  | DESC         |
| ------ | --------------------- | ------------ |
| [in]   | int16_t conn_handle   | 蓝牙连接句柄 |
| [in]   | uint16_t start_handle | 开始句柄     |
| [in]   | uint16_t end_handle   | 结束句柄     |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_write**

- 函数原型

```c
int ble_stack_gatt_write(int16_t conn_handle, uint16_t attr_handle, uint8_t *data, uint16_t len, uint16_t offset, gatt_write_en type)
```

- 功能描述

发起属性写操作

- 参数描述

| IN/OUT | NAME                 | DESC         |
| ------ | -------------------- | ------------ |
| [in]   | int16_t conn_handle  | 蓝牙连接句柄 |
| [in]   | uint16_t attr_handle | 属性句柄     |
| [in]   | uint8_t *data        | 数据地址     |
| [in]   | uint16_t len         | 数据长度     |
| [in]   | uint16_t offset      | 数据偏移值   |
| [in]   | gatt_write_en type   | 操作类型     |

| gatt_write_en（枚举）定义   |                              |
| --------------------------- | ---------------------------- |
| GATT_WRITE                  | 要求对端响应的写操作         |
| GATT_WRITE_WITHOUT_RESPONSE | 无需对端响应的写操作         |
| GATT_WRITE_SINGED           | 带签名且无需对端响应的写操作 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  当type为GATT_WRITE时, 本接口应该到等到EVENT_GATT_CHAR_WRITE_CB事件产生后，才能进行下一次调用，不支持并发调用

### **ble_stack_gatt_write_response**

- 函数原型

```c
#define ble_stack_gatt_write_response(conn_handle, attr_handle, data, len, offset) \
    ble_stack_gatt_write(conn_handle, attr_handle, (uint8_t *)data, len, offset, GATT_WRITE)
```

- 功能描述

属性写操作，要求对端响应

- 参数描述

| IN/OUT | NAME                 | DESC         |
| ------ | -------------------- | ------------ |
| [in]   | int16_t conn_handle  | 蓝牙连接句柄 |
| [in]   | uint16_t attr_handle | 属性句柄     |
| [in]   | uint8_t *data        | 数据地址     |
| [in]   | uint8_t  len         | 数据长度     |
| [in]   | uint16_t offset      | 数据偏移值   |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  发起属性写操作后，直到EVENT_GATT_CHAR_WRITE_CB事件产生后，一次写操作才真正结束。此时才能进行下一次调用，不支持并发调用。

### **ble_stack_gatt_write_no_response**

- 函数原型

```c
#define ble_stack_gatt_write_no_response(conn_handle, attr_handle, data, len, offset) \
    ble_stack_gatt_write(conn_handle, attr_handle, (uint8_t *)data, len, offset, GATT_WRITE_WITHOUT_RESPONSE)
```

- 功能描述

无需对端响应的写操作

- 参数描述

| IN/OUT | NAME                 | DESC         |
| ------ | -------------------- | ------------ |
| [in]   | int16_t conn_handle  | 蓝牙连接句柄 |
| [in]   | uint16_t attr_handle | 属性句柄     |
| [in]   | uint8_t *data        | 数据地址     |
| [in]   | uint8_t  len         | 数据长度     |
| [in]   | uint16_t offset      | 数据偏移值   |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_write_signed**

- 函数原型

```c
#define ble_stack_gatt_write_signed(conn_handle, attr_handle, data, len, offset) \
    ble_stack_gatt_write(conn_handle, attr_handle, (uint8_t *)data, len, offset, GATT_WRITE_SINGED)
```

- 功能描述

数据签名后无需对端响应的写操作

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_gatt_read**

- 函数原型

```c
int ble_stack_gatt_read(int16_t conn_handle, uint16_t attr_handle, uint16_t offset)
```

- 功能描述

发起属性读操作

- 参数描述

| IN/OUT | NAME                 | DESC         |
| ------ | -------------------- | ------------ |
| [in]   | int16_t conn_handle  | 蓝牙连接句柄 |
| [in]   | uint16_t attr_handle | 属性句柄     |
| [in]   | uint16_t offset      | 数据偏移量   |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  发起属性读操作后，属性值将在EVENT_GATT_CHAR_READ_CB事件中返回。此后才能进行下一次调用，不支持并发调用。

### **ble_stack_gatt_read_multiple**

- 函数原型

```c
int ble_stack_gatt_read_multiple(int16_t conn_handle, uint16_t attr_count, uint16_t attr_handle[])
```

- 功能描述

发起多个属性读操作

- 参数描述

| IN/OUT | NAME                   | DESC               |
| ------ | ---------------------- | ------------------ |
| [in]   | int16_t conn_handle    | 蓝牙连接句柄       |
| [in]   | uint16_t attr_count    | 属性值个数         |
| [in]   | uint16_t attr_handle[] | 属性值句柄数组指针 |

- 返回值

| 返回值 |                                 |
| ------ | ------------------------------- |
| >= 0   | 成功，返回密钥长度。0表示未加密 |
| < 0    | 失败                            |

- 注意事项

  发起属性读操作后，属性值将在EVENT_GATT_CHAR_READ_CB事件中返回。此后才能进行下一次调用，不支持并发调用。

