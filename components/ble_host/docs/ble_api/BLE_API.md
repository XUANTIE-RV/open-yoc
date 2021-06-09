# BLE API

# 1. GAP API

该部分定义了蓝牙GAP接口，利用这些接口可以实现蓝牙协议栈初始化、设备I/O能力设置、启动设备广播、获取连接属性、设置安全模式等功能。

## 1.1 接口定义

### **ble_stack_init**

- 函数原型

```c
int ble_stack_init(init_param_t *param)
```

- 功能描述

蓝牙协议栈初始化，包括设备地址、设备名称、最大连接数等参数的设置

- 参数描述

| IN/OUT | NAME                | DESC                 |
| ------ | ------------------- | -------------------- |
| [in]   | init_param_t *param | 蓝牙初始化结构体指针 |

| init_param_t（结构体）定义 |                                                              |
| -------------------------- | ------------------------------------------------------------ |
| char *  dev_name           | 蓝牙设备名,允许传入NULL                                      |
| dev_addr_t * dev_addr      | 蓝牙设备地址类型及设备地址，NULL时使用静态随机地址；参见dev_addr_t结构体定义 |
| uint16_t conn_num_max      | 蓝牙设备最大连接数，取值范围：> 0                            |

| dev_addr_t（结构体）定义 |                                                    |
| ------------------------ | -------------------------------------------------- |
| uint8_t type             | 蓝牙设备地址类型，参见adv_addr_type_en（枚举）定义 |
| uint8_t val[6]           | 蓝牙设备地址值                                     |

| adv_addr_type_en（枚举）定义  |          |
| ----------------------------- | -------- |
| DEV_ADDR_LE_PUBLIC    =  0x00 | 公有地址 |
| DEV_ADDR_LE_RANDOM  =   0x01  | 随机地址 |

- 返回值

| 返回值 |            |
| ------ | ---------- |
| 0      | 初始化成功 |
| 非0    | 初始化失败 |

- 注意事项

  无

### **ble_stack_event_register**

- 函数原型

```c
int ble_stack_event_register(ble_event_cb_t *callback)
```

- 功能描述

蓝牙协议栈事件回调函数注册

- 参数描述

| IN/OUT | NAME                     | DESC                               |
| ------ | ------------------------ | ---------------------------------- |
| [in]   | ble_event_cb_t *callback | 回调函数指针，见ble_event_cb_t定义 |

| ble_event_cb_t（结构体）定义 |                              |
| ---------------------------- | ---------------------------- |
| callback                     | 用户定义的回调处理函数       |
| next                         | 链表指针，指向下一个事件回调 |

| typedef int (*event_callback_func_t)(ble_event_en event, void *event_data) |                                      |
| ------------------------------------------------------------ | ------------------------------------ |
| ble_event_en event                                           | 事件类型，见ble_event_en（枚举）定义 |
| void *event_data                                             | 数据，数据的定义见相应event说明      |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 注册成功 |
| 非0    | 注册失败 |

- 注意事项

  无

### **ble_stack_adv_start**

- 函数原型

```c
int ble_stack_adv_start(adv_param_t *param)
```

- 功能描述

开启蓝牙广播

- 参数描述

| IN/OUT | NAME               | DESC         |
| ------ | ------------------ | ------------ |
| [in]   | adv_param_t *param | 蓝牙广播参数 |

| adv_param_t（结构体）定义           |                                                        |
| ----------------------------------- | ------------------------------------------------------ |
| adv_type_en  type                   | 蓝牙广播类型                                           |
| ad_data_t *ad                       | 蓝牙广播数据数组指针                                   |
| ad_data_t *sd                       | 蓝牙广播响应包数组地址                                 |
| uint8_t   ad_num                    | 蓝牙广播数据数组大小                                   |
| uint8_t   sd_num                    | 蓝牙广播响应包数组大小                                 |
| uint16_t  interval_min              | 广播最小间隔                                           |
| uint16_t  interval_max              | 广播最大间隔                                           |
| adv_filter_policy_en  filter_policy | 广播过滤策略                                           |
| adv_chan_en  channel_map            | 广播信道配置，默认为同时使用37、38、39信道广播         |
| dev_addr_t  direct_peer_addr        | 对端地址，见ble_stack_init接口中的dev_addr_t结构体定义 |

| adv_type_en（枚举）定义        |                                          |
| ------------------------------ | ---------------------------------------- |
| ADV_IND =   0x00               | 可连接非定向广播                         |
| ADV_DIRECT_IND =   0x01        | 高速直连广播，使用蓝牙协议栈默认广播参数 |
| ADV_SCAN_IND =   0x02          | 可发现非定向广播                         |
| ADV_NONCONN_IND =   0x03       | 不可连接非定向广播                       |
| ADV_DIRECT_IND_LOW_DUTY = 0x04 | 低速直连广播, 使用蓝牙协议栈默认广播参数 |

| ad_data_t（结构体）定义 |                  |
| ----------------------- | ---------------- |
| uint8_t type            | 蓝牙广播数据类型 |
| uint8_t len             | 数据长度         |
| uint8_t *data           | 数据指针         |

| adv_filter_policy_en（枚举）定义 |                                              |
| -------------------------------- | -------------------------------------------- |
| ADV_FILTER_POLICY_ANY_REQ   = 0  | 禁用白名单机制，允许任何设备连接和扫描       |
| ADV_FILTER_POLICY_SCAN_REQ  = 1  | 允许任何设备连接，但只允许白名单中的设备扫描 |
| ADV_FILTER_POLICY_CONN_REQ  = 2  | 允许任何设备扫描，但只允许白名单中的设备连接 |
| ADV_FILTER_POLICY_ALL_REQ   = 3  | 只允许白名单中的设备扫描和连接               |

| adv_chan_en（枚举）定义 |                |
| ----------------------- | -------------- |
| ADV_CHAN_37 = 0x01      | 在37信道上广播 |
| ADV_CHAN_38 = 0x02      | 在38信道上广播 |
| ADV_CHAN_39 = 0x04      | 在39信道上广播 |

| 蓝牙广播数据包类型（枚举）定义        |                          |
| ------------------------------------- | ------------------------ |
| AD_DATA_TYPE_FLAGS=         0x01      | FLAGS标签                |
| AD_DATA_TYPE_UUID16_SOME=    0x02     | 16位UUID部分服务列表     |
| AD_DATA_TYPE_UUID16_ALL =     0x03    | 16位UUID完整服务列表     |
| AD_DATA_TYPE_UUID32_SOME =    0x04    | 32位UUID部分列表         |
| AD_DATA_TYPE_UUID32_ALL =     0x05    | 32位UUID完整列表         |
| AD_DATA_TYPE_UUID128_SOME =   0x06    | 128位UUID部分服务列表    |
| AD_DATA_TYPE_UUID128_ALL =    0x07    | 128位UUID完整服务列表    |
| AD_DATA_TYPE_NAME_SHORTENED = 0x08    | 部分设备名               |
| AD_DATA_TYPE_NAME_COMPLETE =  0x09    | 完整设备名               |
| AD_DATA_TYPE_TX_POWER =       0x0a    | 发射功率                 |
| AD_DATA_TYPE_SOLICIT16 =      0x14    | 16位UUID                 |
| AD_DATA_TYPE_SOLICIT128 =     0x15    | 128位UUID                |
| AD_DATA_TYPE_SVC_DATA16 =     0x16    | 16位UUID服务数据         |
| AD_DATA_TYPE_GAP_APPEARANCE = 0x19    | GAP外观特性              |
| AD_DATA_TYPE_SOLICIT32 =      0x1F    | 32位请求UUID             |
| AD_DATA_TYPE_SVC_DATA32 =     0x20    | 32位UUID服务数据         |
| AD_DATA_TYPE_SVC_DATA128 =    0x21    | 128位UUID服务数据        |
| AD_DATA_TYPE_URI =            0x24    | URI                      |
| AD_DATA_TYPE_MESH_PROV =      0x29    | Mesh Provisioning 数据包 |
| AD_DATA_TYPE_MESH_MESSAGE =   0x2A    | Mesh 网络数据包          |
| AD_DATA_TYPE_MESH_BEACON =    0x2B    | Mesh Beacon协议数据包    |
| AD_DATA_TYPE_MANUFACTURER_DATA = 0xFF | 制造商指定数据           |

蓝牙广播数据包类型可参考《Supplement to the Bluetooth Core Specification 7.0》里的定义。

- 返回值

| 返回值 |              |
| ------ | ------------ |
| 0      | 成功开启广播 |
| 非0    | 开启广播失败 |

- 注意事项

  无

### **ble_stack_adv_stop**

- 函数原型

```c
int ble_stack_adv_stop()
```

- 功能描述

停止蓝牙广播

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_scan_start**

- 函数原型

```c
int ble_stack_scan_start(scan_param_t *param)
```

- 功能描述

蓝牙设备开启扫描

- 参数描述

| IN/OUT | NAME                | DESC         |
| ------ | ------------------- | ------------ |
| [in]   | scan_param_t *param | 蓝牙扫描参数 |

| scan_param_t（结构体）定义        |                                  |
| --------------------------------- | -------------------------------- |
| scan_type_en  type                | 扫描类型                         |
| scan_filter_en filter_dup         | 是否过滤重复包                   |
| uint16_t interval                 | 扫描间隔，可参考协议文档自行设置 |
| uint16_t window                   | 扫描窗口，可参考协议文档自行设置 |
| scan_filter_policy_en scan_filter | 扫描过滤策略                     |

| scan_type_en（枚举）定义 |                                                            |
| ------------------------ | ---------------------------------------------------------- |
| SCAN_PASSIVE = 0x00      | 被动扫描，仅接收对端的广播数据包                           |
| SCAN_ACTIVE = 0x01       | 主动扫描，不仅可接收对端的广播数据包，还可检测到扫描响应包 |

| scan_filter_en（枚举）定义     |              |
| ------------------------------ | ------------ |
| SCAN_FILTER_DUP_DISABLE = 0x00 | 不过滤重复包 |
| SCAN_FILTER_DUP_ENABLE = 0x01  | 过滤重复包   |

| 扫描间隔（枚举）定义          |       |
| ----------------------------- | ----- |
| SCAN_FAST_INTERVAL   = 0x0060 | 60ms  |
| SCAN_SLOW_INTERVAL_1 = 0x0800 | 1.28s |
| SCAN_SLOW_INTERVAL_2 = 0x1000 | 2.56s |

| 扫描窗口（枚举）定义         |         |
| ---------------------------- | ------- |
| SCAN_FAST_WINDOW    = 0x0030 | 30ms    |
| SCAN_SLOW_WINDOW   = 0x0012  | 11.25ms |

| scan_filter_policy_en（枚举）定义      |                              |
| -------------------------------------- | ---------------------------- |
| SCAN_FILTER_POLICY_ANY_ADV         = 0 | 可以扫描任何设备的广播包     |
| SCAN_FILTER_POLICY_WHITE_LIST     = 1  | 只能扫描到白名单设备的广播包 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_scan_stop**

- 函数原型

```c
int ble_stack_scan_stop()
```

- 功能描述

停止扫描

- 参数描述

  无

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | 非0    | 失败 |

- 注意事项

  无

### **ble_stack_get_local_addr**

- 函数原型

```c
int ble_stack_get_local_addr(dev_addr_t *addr)
```

- 功能描述

获取蓝牙设备地址

- 参数描述

| IN/OUT | NAME             | DESC                                               |
| ------ | ---------------- | -------------------------------------------------- |
| [in]   | dev_addr_t *addr | 地址指针，参见ble_stack_init接口中dev_addr_t的定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_connect**

- 函数原型

```c
int ble_stack_connect(dev_addr_t *peer_addr, conn_param_t *param, uint8_t auto_connect)
```

- 功能描述

发起连接请求

- 参数描述

| IN/OUT | NAME                  | DESC                                                   |
| ------ | --------------------- | ------------------------------------------------------ |
| [in]   | dev_addr_t *peer_addr | 对端设备地址，参见ble_stack_init接口中dev_addr_t的定义 |
| [in]   | conn_param_t *param   | 连接参数                                               |
| [in]   | uint8_t auto_connect  | 是否使能自动连接，1使能；0不使能                       |

| conn_param_t（结构体）定义 |                                              |
| -------------------------- | -------------------------------------------- |
| uint16_t interval_min      | 蓝牙连接最小间隔时间，可参考协议文档自行定义 |
| uint16_t interval_max      | 蓝牙连接最大间隔时间，可参考协议文档自行定义 |
| uint16_t latency           | 可忽略的蓝牙连接事件个数                     |
| uint16_t timeout           | 蓝牙连接超时断开时间                         |

| 枚举定义                        |      |
| ------------------------------- | ---- |
| CONN_INT_MIN_INTERVAL =  0x0018 | 30ms |
| CONN_INT_MAX_INTERVAL = 0x0028  | 50ms |

- 返回值

| 返回值 |                                  |
| ------ | -------------------------------- |
| >= 0   | 成功, 返回蓝牙连接句柄conn_hanle |
| < 0    | 失败                             |

- 注意事项

  无

### **ble_stack_disconnect**

- 函数原型

```c
int ble_stack_disconnect(int16_t conn_handle)
```

- 功能描述

请求断开蓝牙连接

- 参数描述

| IN/OUT | NAME                | DESC           |
| ------ | ------------------- | -------------- |
| [in]   | int16_t conn_handle | 蓝牙连接的句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_connect_info_get**

- 函数原型

```c
int ble_stack_connect_info_get(int16_t conn_handle, connect_info_t *info)
```

- 功能描述

获取蓝牙连接信息

- 参数描述

| IN/OUT | NAME                 | DESC             |
| ------ | -------------------- | ---------------- |
| [in]   | int16_t conn_handle  | 蓝牙连接句柄     |
| [in]   | connect_info_t *info | 连接信息存储指针 |

| connect_info_t（结构体）定义 |                                                        |
| ---------------------------- | ------------------------------------------------------ |
| int16_t conn_handle          | 连接句柄                                               |
| uint8_t   role               | 设备主从模式，取值范围： master 0 ; slave：1           |
| uint16_t  interval           | 连接间隔                                               |
| uint16_t  latency            | 可忽略的蓝牙连接事件个数                               |
| uint16_t  timeout            | 连接超时时间                                           |
| dev_addr_t  local_addr       | 设备本机地址，参见ble_stack_init接口中dev_addr_t的定义 |
| dev_addr_t  peer_addr        | 对端设备地址，参见ble_stack_init接口中dev_addr_t的定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无


### **ble_stack_connect_param_update**

- 函数原型

```c
int ble_stack_connect_param_update(int16_t conn_handle, conn_param_t *param)
```

- 功能描述

蓝牙连接参数更新

- 参数描述

| IN/OUT | NAME                | DESC                                                      |
| ------ | ------------------- | --------------------------------------------------------- |
| [in]   | int16_t conn_handle | 蓝牙连接句柄                                              |
| [in]   | conn_param_t *param | 蓝牙连接参数, 参见ble_stack_connect接口中conn_param_t定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_white_list_clear**

- 函数原型

```c
int ble_stack_white_list_clear()
```

- 功能描述

  清空白名单

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_white_list_add**

- 函数原型

```c
int ble_stack_white_list_add(dev_addr_t *peer_addr)
```

- 功能描述

  将指定设备添加至白名单

- 参数描述

| IN/OUT | NAME                  | DESC                                                         |
| ------ | --------------------- | ------------------------------------------------------------ |
| [in]   | dev_addr_t *peer_addr | 指定设备的地址信息，参见ble_stack_init接口中dev_addr_t的定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_white_list_remove**

- 函数原型

```c
int ble_stack_white_list_remove(dev_addr_t *peer_addr)
```

- 功能描述

  将指定设备从白名单中移除

- 参数描述

| IN/OUT | NAME                  | DESC                                                         |
| ------ | --------------------- | ------------------------------------------------------------ |
| [in]   | dev_addr_t *peer_addr | 指定设备的地址信息，参见ble_stack_init接口中dev_addr_t的定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_white_list_size**

- 函数原型

```c
int ble_stack_white_list_size()
```

- 功能描述

  获取可保存白名单设备的个数

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### **ble_stack_pref_phy_set**

- 函数原型

```c
int ble_stack_pref_phy_set(int16_t conn_handle, pref_phy_en pref_tx_phy,
pref_phy_en pref_rx_phy)
```

- 功能描述

  设置默认的LE PHY层速率

- 参数描述

| IN/OUT | NAME                    | DESC     |
| ------ | ----------------------- | -------- |
| [in]   | int16_t conn_handle     | 连接句柄 |
| [in]   | pref_phy_en pref_tx_phy | 发送速率 |
| [in]   | pref_phy_en pref_rx_phy | 接收速率 |

| 枚举定义(pref_phy_en) |              |
| --------------------- | ------------ |
| PREF_PHY_1M =  0x01   | 1M PHY       |
| PREF_PHY_2M = 0x02    | 2M PHY       |
| PREF_PHY_CODED = 0x04 | 1M CODED PHY |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无
  
# 2. GATT API

该部分定义了蓝牙通用属性规范（GATT）主要服务接口，包括服务注册、服务特性的发现、读、写、通知、指示以及服务特性的广播。

## 2.1 接口定义

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



# 3. SMP API

SMP即Security Manage Protocol，是蓝牙用来进行安全管理得部分，其定义了配对和密钥分发的过程实现。SMP被用在LE-only设备或蓝牙双模设备中。

本部分定义了蓝牙SMP接口，利用这些接口可以实现配对和密钥分发等功能。

## 3.1 接口定义

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
  
# 4. 事件类型定义



| BLE Stack事件定义                      |                                                         |
| -------------------------------------- | ------------------------------------------------------- |
| EVENT_STACK_INIT = EVENT_BLE,          | 初始化事件                                              |
| EVENT_GAP_CONN_CHANGE,                 | 连接事件（连接，未连接,Err）                            |
| EVENT_GAP_DEV_FIND,                    | 发现设备事件                                            |
| EVENT_GAP_CONN_PARAM_REQ,              | 连接请求事件(Interval_min,Interval_max,latency,timeout) |
| EVENT_GAP_CONN_PARAM_UPDATE,           | 连接更新事件(Interval,latency,timeout)                  |
| EVENT_GAP_CONN_SECURITY_CHANGE,        | 加密等级变化事件(0,1,2,3)                               |
| EVENT_GATT_NOTIFY,                     | 通知事件                                                |
| EVENT_GATT_INDICATE=EVENT_GATT_NOTIFY, | 指向事件                                                |
| EVENT_GATT_CHAR_READ,                  | 特征读事件                                              |
| EVENT_GATT_CHAR_WRITE,                 | 特征写事件                                              |
| EVENT_GATT_INDICATE_CB,                | 指向返回事件                                            |
| EVENT_GATT_CHAR_READ_CB,               | 特征读返回事件                                          |
| EVENT_GATT_CHAR_WRITE_CB,              | 特征写返回事件                                          |
| EVENT_GATT_CHAR_CCC_CHANGE,            | 特征值变化事件                                          |
| EVENT_GATT_MTU_EXCHANGE,               | 最大传输单元交换事件                                    |
| EVENT_GATT_DISCOVERY_SVC,              | 发现服务事件                                            |
| EVENT_GATT_DISCOVERY_INC_SVC,          | 发现增加服务事件                                        |
| EVENT_GATT_DISCOVERY_CHAR,             | 发现特征事件                                            |
| EVENT_GATT_DISCOVERY_CHAR_DES,         | 特征描述事件                                            |
| EVENT_GATT_DISCOVERY_COMPLETE,         | 发现完成事件                                            |
| EVENT_SMP_PASSKEY_DISPLAY,             | 安全密钥显示事件                                        |
| EVENT_SMP_PASSKEY_CONFIRM,             | 安全密钥确认事件                                        |
| EVENT_SMP_PASSKEY_ENTER,               | 密钥输入事件（对应地址）                                |
| EVENT_SMP_PAIRING_CONFIRM,             | 配对确认事件                                            |
| EVENT_SMP_PAIRING_COMPLETE             | 配对完成事件                                            |
| EVENT_SMP_CANCEL,                      | 配对取消事件                                            |
| EVENT_STACK_UNKNOWN,                   | 无效事件                                                |

# 5. GATT Sevice 定义

该部分描述了特性值的定义并列举出标准UUID值

## **5.1 特征值定义**

### **GATT_PRIMARY_SERVICE_DEFINE**

- 函数原型

```c
uuid_t  GATT_PRIMARY_SERVICE_DEFINE(_uuid)
```

- 功能描述

定义服务类型

- 参数描述

| IN/OUT | NAME  | DESC                        |
| ------ | ----- | --------------------------- |
| [in]   | _uuid | uuid里的 服务类型结构体uuid |

- 返回值

  无

- 注意事项

  一种应用一般只有一个

### **GATT_CHAR_DEFINE**

- 函数原型

```c
#define GATT_CHAR_DEFINE(_uuid, _props)
```

- 功能描述

特征属性定义

- 参数描述

| IN/OUT | NAME   | DESC         |
| ------ | ------ | ------------ |
| [in]   | _uuid  | 特征类型UUID |
| [in]   | _props | 属性         |

| _props（枚举）定义                           |                                    |
| -------------------------------------------- | ---------------------------------- |
| GATT_CHRC_PROP_BROADCAST=BLE_BIT(0)          | 广播属性特征                       |
| GATT_CHRC_PROP_READ=BLE_BIT(1)               | 读属性                             |
| GATT_CHRC_PROP_WRITE_WITHOUT_RESP=BLE_BIT(2) | 无需回复的写入属性                 |
| GATT_CHRC_PROP_WRITE=BLE_BIT(3)              | 需要回复的写属性                   |
| GATT_CHRC_PROP_NOTIFY=BLE_BIT(4)             | 通知属性（允许未经接受的通知特性） |
| GATT_CHRC_PROP_INDICATE=BLE_BIT(5)           | 指向属性(允许未经接受的指向特性)   |
| GATT_CHRC_PROP_AUTH=BLE_BIT(6)               | 签名认证写属性（允许签名的写特性） |
| GATT_CHRC_PROP_EXT_PROP=BLE_BIT(7)           | 扩展属性特性(附加特征属性)         |

- 返回值

  无

- 注意事项

  无

### **GATT_CHAR_VAL_DEFINE**

- 函数原型

```c
#define GATT_CHAR_VAL_DEFINE(_uuid, _perm)
```

- 功能描述

特征值定义

- 参数描述

| IN/OUT | 4NAME | DESC             |
| ------ | ----- | ---------------- |
| [in]   | _uuid | 特征描述类UUID   |
| [in]   | _perm | GATT操作许可类型 |

| _perm（枚举）定义                  |                |
| ---------------------------------- | -------------- |
| GATT_PERM_NONE=0                   | 无(仅用于通知) |
| GATT_PERM_READ=BLE_BIT(0)          | 读属性许可     |
| GATT_PERM_WRITE=BLE_BIT(1)         | 写属性许可     |
| GATT_PERM_READ_ENCRYPT=BLE_BIT(2)  | 读加密许可     |
| GATT_PERM_WRITE_ENCRYPT=BLE_BIT(3) | 写加密许可     |
| GATT_PERM_READ_AUTHEN=BLE_BIT(4)   | 读验证许可     |
| GATT_PERM_WRITE_AUTHEN=BLE_BIT(5)  | 写验证许可     |
| GATT_PERM_PREPARE_WRITE=BLE_BIT(6) | 准备写入许可   |

- 返回值

  无

- 注意事项

  无

### **GATT_CHAR_CCC_DEFINE**

- 函数原型

```c
GATT_CHAR_CCC_DEFINE(_perm)
```

- 功能描述

客户端特征配置,可用于客户端配置触发的条件

- 参数描述

  | _perm（枚举）定义                  |                |
  | ---------------------------------- | -------------- |
  | GATT_PERM_NONE=0                   | 无(仅用于通知) |
  | GATT_PERM_READ=BLE_BIT(0)          | 读属性许可     |
  | GATT_PERM_WRITE=BLE_BIT(1)         | 写属性许可     |
  | GATT_PERM_READ_ENCRYPT=BLE_BIT(2)  | 读加密许可     |
  | GATT_PERM_WRITE_ENCRYPT=BLE_BIT(3) | 写加密许可     |
  | GATT_PERM_READ_AUTHEN=BLE_BIT(4)   | 读验证许可     |
  | GATT_PERM_WRITE_AUTHEN=BLE_BIT(5)  | 写验证许可     |
  | GATT_PERM_PREPARE_WRITE=BLE_BIT(6) | 准备写入许可   |

- 返回值

  无

- 注意事项

  用户可定义触发的类型

### **GATT_CHAR_CUD_DEFINE**

- 函数原型

```c
GATT_CHAR_CUD_DEFINE(_value, _perm)
```

- 功能描述

用户描述GATT特征

- 参数描述

| IN/OUT | NAME   | DESC               |
| ------ | ------ | ------------------ |
| [in]   | _value | 用户描述的特征数据 |
| [in]   | _perm  | GATT操作许可类型   |

| _perm（枚举）定义                  |                |
| ---------------------------------- | -------------- |
| GATT_PERM_NONE=0                   | 无(仅用于通知) |
| GATT_PERM_READ=BLE_BIT(0)          | 读属性许可     |
| GATT_PERM_WRITE=BLE_BIT(1)         | 写属性许可     |
| GATT_PERM_READ_ENCRYPT=BLE_BIT(2)  | 读加密许可     |
| GATT_PERM_WRITE_ENCRYPT=BLE_BIT(3) | 写加密许可     |
| GATT_PERM_READ_AUTHEN=BLE_BIT(4)   | 读验证许可     |
| GATT_PERM_WRITE_AUTHEN=BLE_BIT(5)  | 写验证许可     |
| GATT_PERM_PREPARE_WRITE=BLE_BIT(6) | 准备写入许可   |

- 返回值

  无

- 注意事项

  无

# 6. 标准UUID定义

| 服务类型UUID |        |                                   |
| ------------ | ------ | --------------------------------- |
| GAP          | 0x1800 | Generic Access                    |
| GATT         | 0x1801 | Generic Attribute                 |
| CTS          | 0x1805 | Current Time Service              |
| DIS          | 0x180a | Device Information Service        |
| HRS          | 0x180d | Heart Rate Service                |
| BAS          | 0x180f | Battery Service                   |
| HIDS         | 0x1812 | HID Service                       |
| CSC          | 0x1816 | Cycling Speed and Cadence Service |
| ESS          | 0x181a | Environmental Sensing Service     |
| IPSS         | 0x1820 | IP Support Service                |
| MESH_PROV    | 0x1827 | Mesh Provisioning Service         |
| MESH_PROXY   | 0x1828 | Mesh Proxy Service                |

| 属性类型UUID   |        |                        |
| -------------- | ------ | ---------------------- |
| GATT_PRIMARY   | 0x2800 | GATT Primary Service   |
| GATT_SECONDARY | 0x2801 | GATT Secondary Service |
| GATT_SECONDARY | 0x2801 | GATT Secondary Service |
| GATT_INCLUDE   | 0x2802 | GATT Include Service   |
| GATT_CHRC      | 0x2803 | GATT Characteristic    |

| 特性描述UUID       |        |                                                  |
| ------------------ | ------ | ------------------------------------------------ |
| GATT_CEP           | 0x2900 | GATT Characteristic Extended Properties          |
| GATT_CUD           | 0x2901 | GATT Characteristic User Description             |
| GATT_CCC           | 0x2902 | GATT Client Characteristic Configuration         |
| GATT_SCC           | 0x2903 | GATT Server Characteristic Configuration         |
| GATT_CPF           | 0x2904 | GATT Characteristic Presentation Format          |
| VALID_RANGE        | 0x2906 | Valid Range Descriptor                           |
| HIDS_EXT_REPORT    | 0x2907 | HID External Report Descriptor                   |
| HIDS_REPORT_REF    | 0x2908 | HID Report Reference Descriptor                  |
| ES_CONFIGURATION   | 0x290b | Environmental Sensing Configuration Descriptor   |
| ES_MEASUREMENT     | 0x290c | Environmental Sensing Measurement Descriptor     |
| ES_TRIGGER_SETTING | 0x290d | Environmental Sensing Trigger Setting Descriptor |

| 特征类型UUID                            |        |                                                              |
| --------------------------------------- | ------ | ------------------------------------------------------------ |
| GAP_DEVICE_NAME                         | 0x2a00 | GAP Characteristic Device Name                               |
| GAP_APPEARANCE                          | 0x2a01 | GAP Characteristic Appearance                                |
| GAP_PPCP                                | 0x2a04 | GAP Characteristic Peripheral Preferred Connection Parameter |
| GATT_SC                                 | 0x2a05 | GATT Characteristic Service Changed                          |
| BAS_BATTERY_LEVEL                       | 0x2a19 | BAS Characteristic Battery Level                             |
| HIDS_BOOT_KB_IN_REPORT                  | 0x2a22 | HID Characteristic Boot Keyboard Input Report                |
| DIS_SYSTEM_ID                           | 0x2a23 | DIS Characteristic System ID                                 |
| DIS_MODEL_NUMBER                        | 0x2a24 | IS Characteristic Model Number String                        |
| DIS_SERIAL_NUMBER                       | 0x2a25 | DIS Characteristic Serial Number String                      |
| DIS_FIRMWARE_REVISION                   | 0x2a26 | DIS Characteristic Firmware Revision String                  |
| DIS_HARDWARE_REVISION                   | 0x2a27 | DIS Characteristic Hardware Revision String                  |
| DIS_SOFTWARE_REVISION                   | 0x2a28 | IS Characteristic Software Revision String                   |
| DIS_MANUFACTURER_NAME                   | 0x2a29 | DIS Characteristic Manufacturer Name String                  |
| IEEE_REGULATORY_CERTIFICATION_DATA_LIST | 0x2a2a | IEEE Regulatory Certification Data List characteristic UUID  |
| DIS_PNP_ID                              | 0x2a50 | DIS Characteristic PnP ID                                    |
| CTS_CURRENT_TIME                        | 0x2a2b | CTS Characteristic Current Time                              |
| MAGN_DECLINATION                        | 0x2a2c | Magnetic Declination Characteristic                          |
| HIDS_BOOT_KB_OUT_REPORT                 | 0x2a32 | HID Boot Keyboard Output Report Characteristic               |
| HIDS_BOOT_MOUSE_IN_REPORT               | 0x2a33 | HID Boot Mouse Input Report Characteristic                   |
| HRS_MEASUREMENT                         | 0x2a37 | HRS Characteristic Measurement Interval                      |
| HRS_BODY_SENSOR                         | 0x2a38 | HRS Characteristic Body Sensor Location                      |
| HRS_CONTROL_POINT                       | 0x2a39 | HRS Characteristic Control Point                             |
| HIDS_INFO                               | 0x2a4a | HID Information Characteristic                               |
| HIDS_REPORT_MAP                         | 0x2a4b | HID Report Map Characteristic                                |
| HIDS_CTRL_POINT                         | 0x2a4c | HID Control Point Characteristic                             |
| HIDS_REPORT                             | 0x2a4d | HID Report Characteristic                                    |
| HIDS_PROTOCOL_MODE                      | 0x2a4e | HID Protocol Mode Characteristic                             |
| CSC_MEASUREMENT                         | 0x2a5b | CSC Measurement Characteristic                               |
| CSC_FEATURE                             | 0x2a5c | CSC Feature Characteristic                                   |
| SENSOR_LOCATION                         | 0x2a5d | Sensor Location Characteristic                               |
| SC_CONTROL_POINT                        | 0x2a55 | SC Control Point Characteristic                              |
| ELEVATION                               | 0x2a6c | Elevation Characteristic                                     |
| PRESSURE                                | 0x2a6d | Pressure Characteristic                                      |
| TEMPERATURE                             | 0x2a6e | Temperature Characteristic                                   |
| HUMIDITY                                | 0x2a6f | Humidity Characteristic                                      |
| TRUE_WIND_SPEED                         | 0x2a70 | True Wind Speed Characteristic                               |
| TRUE_WIND_DIR                           | 0x2a71 | True Wind Direction Characteristic                           |
| APPARENT_WIND_SPEED                     | 0x2a72 | Apparent Wind Speed Characteristic                           |
| APPARENT_WIND_DIR                       | 0x2a73 | Apparent Wind Direction Characteristic                       |
| GUST_FACTOR                             | 0x2a74 | Gust Factor Characteristic                                   |
| POLLEN_CONCENTRATION                    | 0x2a75 | Pollen Concentration Characteristic                          |
| UV_INDEX                                | 0x2a76 | UV Index Characteristic                                      |
| IRRADIANCE                              | 0x2a77 | Irradiance Characteristic                                    |
| RAINFALL                                | 0x2a78 | Rainfall Characteristic                                      |
| WIND_CHILL                              | 0x2a79 | Wind Chill Characteristic                                    |
| HEAT_INDEX                              | 0x2a7a | Heat Index Characteristic                                    |
| DEW_POINT                               | 0x2a7b | Dew Point Characteristic                                     |
| DESC_VALUE_CHANGED                      | 0x2a7d | Descriptor Value Changed Characteristic                      |
| MAGN_FLUX_DENSITY_2D                    | 0x2aa0 | Magnetic Flux Density - 2D Characteristic                    |
| MAGN_FLUX_DENSITY_3D                    | 0x2aa1 | Magnetic Flux Density - 3D Characteristic                    |
| BAR_PRESSURE_TREND                      | 0x2aa3 | Barometric Pressure Trend Characteristic                     |
| CENTRAL_ADDR_RES                        | 0x2aa6 | Central Address Resolution Characteristic                    |
| MESH_PROV_DATA_IN                       | 0x2adb | Mesh Provisioning Data In                                    |
| MESH_PROV_DATA_OUT                      | 0x2adc | Mesh Provisioning Data Out                                   |
| MESH_PROXY_DATA_IN                      | 0x2add | Mesh Proxy Data In                                           |
| MESH_PROXY_DATA_OUT                     | 0x2ade | Mesh Proxy Data Out                                          |

# 7. 错误码说明

| BLE Stack错误码说明       |              |
| ------------------------- | ------------ |
| BLE_STACK_OK = 0,         | 无错误       |
| BLE_STACK_ERR_NULL=1,     | 参数为空指针 |
| BLE_STACK_ERR_PARAM=2,    | 参数错误     |
| BLE_STACK_ERR_INTERNAL=3, | 内部错误     |
| BLE_STACK_ERR_INIT=4,     | 初始化错误   |
| BLE_STACK_ERR_CONN=5,     | 连接错误     |
| BLE_STACK_ERR_ALREADY=6,  | 已经处理过   |




