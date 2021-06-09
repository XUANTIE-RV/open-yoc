## Title:  Health

# 概述

本部分定义了蓝牙MESH Health接口，利用这些接口可以实现蓝牙MESH节点健康状态的操作。

# 接口定义

### bt_mesh_health_cli_set

- 函数原型

```c
int bt_mesh_health_cli_set(struct bt_mesh_model *model)
```

- 功能描述

​      设置节点为Health Model Client

- 参数描述

| IN/OUT | NAME                        | DESC                                                         |
| ------ | --------------------------- | ------------------------------------------------------------ |
| [in]   | struct bt_mesh_model *model | Health Model Client配置参数，参见[ACCESS](MESH_ACCESS_API.md)中struct bt_mesh_model（结构体）定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model

### **bt_mesh_health_fault_get**

- 函数原型

```c
int bt_mesh_health_fault_get(u16_t net_idx,
                             u16_t addr, 
                             u16_t app_idx,
                             u16_t cid,
                             u8_t *test_id,
                             u8_t *faults,
                             size_t *fault_count)
```

- 功能描述

​      获取对端节点注册的错误状态

- 参数描述

| IN/OUT | NAME                | DESC                 |
| ------ | ------------------- | -------------------- |
| [in]   | u16_t net_idx       | Network Key索引      |
| [in]   | u16_t addr          | 节点的unicast地址    |
| [in]   | u16_t app_idx       | APP Key索引          |
| [in]   | cid                 | 错误对应的Company ID |
| [out]  | u8_t *test_id       | 最后执行的测试ID     |
| [out]  | u8_t *faults        | 错误列表             |
| [out]  | size_t *fault_count | 错误个数             |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model，对端节点支持 health srv model

### bt_mesh_health_fault_clear

- 函数原型

```c
int bt_mesh_health_fault_clear(u16_t net_idx,
                             u16_t addr, 
                             u16_t app_idx,
                             u16_t cid,
                             u8_t *test_id,
                             u8_t *faults,
                             size_t *fault_count)
```

- 功能描述

​       清除注册的错误状态

- 参数描述

| IN/OUT | NAME                | DESC                 |
| ------ | ------------------- | -------------------- |
| [in]   | u16_t net_idx       | Network Key索引      |
| [in]   | u16_t addr          | 节点的unicast地址    |
| [in]   | u16_t app_idx       | APP Key索引          |
| [in]   | cid                 | 错误对应的Company ID |
| [out]  | u8_t *test_id       | 最后执行的测试ID     |
| [out]  | u8_t *faults        | 错误列表             |
| [out]  | size_t *fault_count | 错误个数             |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model，对端节点支持 health srv model

### **bt_mesh_health_fault_test**

- 函数原型

```c
int bt_mesh_health_fault_test(u16_t net_idx, 
                              u16_t addr, 
                              u16_t app_idx, 
                              u16_t cid, 
                              u8_t test_id, 
                              u8_t *faults,
                              size_t *fault_count)
```

- 功能描述

​       错误测试

- 参数描述

| IN/OUT | NAME                | DESC              |
| ------ | ------------------- | ----------------- |
| [in]   | u16_t net_idx       | Network Key索引   |
| [in]   | u16_t addr          | 节点的unicast地址 |
| [in]   | u16_t app_idx       | APP Key索引       |
| [in]   | cid                 | Company ID        |
| [in]   | u8_t test_id        | 测试ID            |
| [out]  | u8_t *faults        | 错误列表          |
| [out]  | size_t *fault_count | 错误个数          |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model，对端节点支持 health srv model

### **bt_mesh_health_period_get**

- 函数原型

```c
int bt_mesh_health_period_get(u16_t net_idx, 
                              u16_t addr, 
                              u16_t app_idx,
                              u8_t *divisor)
```

- 功能描述

​       获取Health Model消息发布的分频系数值

- 参数描述

| IN/OUT | NAME          | DESC               |
| ------ | ------------- | ------------------ |
| [in]   | u16_t net_idx | Network Key索引    |
| [in]   | u16_t addr    | 节点的unicast地址  |
| [in]   | u16_t app_idx | APP Key索引        |
| [out]  | u8_t *divisor | 分频系数（0 - 15） |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model，对端节点支持 health srv model

### **bt_mesh_health_period_set**

- 函数原型

```c
int bt_mesh_health_period_set(u16_t net_idx, 
                              u16_t addr, 
                              u16_t app_idx,
                              u8_t divisor, 
                              u8_t *updated_divisor)
```

- 功能描述

设置Health Model消息发布的分频系数值

- 参数描述

| IN/OUT | NAME                  | DESC                 |
| ------ | --------------------- | -------------------- |
| [in]   | u16_t net_idx         | Network Key索引      |
| [in]   | u16_t addr            | 节点的unicast地址    |
| [in]   | u16_t app_idx         | APP Key索引          |
| [in]   | u8_t divisor          | 分频系数值（0 - 15） |
| [out]  | u8_t *updated_divisor | 更新后的分频系数     |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model，对端节点支持 health srv model

### **bt_mesh_health_attention_get**

- 函数原型

```c
int bt_mesh_health_attention_get(u16_t net_idx,
                                 u16_t addr, 
                                 u16_t app_idx,
                                 u8_t *attention)
```

- 功能描述

​     获取Health Model的Attention定时器状态

- 参数描述

| IN/OUT | NAME            | DESC                                                        |
| ------ | --------------- | ----------------------------------------------------------- |
| [in]   | u16_t net_idx   | Network Key索引                                             |
| [in]   | u16_t addr      | 节点的unicast地址                                           |
| [in]   | u16_t app_idx   | APP Key索引                                                 |
| [out]  | u8_t *attention | 定时器状态<br />0x00：关闭<br />0x01 ~ 0xFF：剩余时间（秒） |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model，对端节点支持 health srv model

### **bt_mesh_health_attention_set**

- 函数原型

```c
int bt_mesh_health_attention_set(u16_t net_idx, 
                                 u16_t addr,
                                 u16_t app_idx,
                                 u8_t attention,
                                 u8_t *updated_attention)
```

- 功能描述

设置Health Model的Attention定时器状态

- 参数描述

| IN/OUT | NAME                    | DESC                                                    |
| ------ | ----------------------- | ------------------------------------------------------- |
| [in]   | u16_t net_idx           | Network Key索引                                         |
| [in]   | u16_t addr              | 节点的unicast地址                                       |
| [in]   | u16_t app_idx           | APP Key索引                                             |
| [in]   | u8_t *attention         | 定时器状态 <br />0x00：关闭 0x01 ~ 0xFF：剩余时间（秒） |
| [out]  | u8_t *updated_attention | 更新后的attention定时器状态                             |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本节点需支持health cli model，对端节点支持 health srv model

### **bt_mesh_health_cli_timeout_get**

- 函数原型

```c
s32_t bt_mesh_health_cli_timeout_get(void)
```

- 功能描述

​     获取本health cli超时时间

- 参数描述

  无

- 返回值

返回超时时间

- 注意事项

  本节点需支持health cli model
  
  

### **bt_mesh_health_cli_timeout_set**

- 函数原型

```c
void bt_mesh_health_cli_timeout_set(s32_t timeout)
```

- 功能描述

​       设置本helath cli超时时间

- 参数描述

| IN/OUT | NAME          | DESC     |
| ------ | ------------- | -------- |
| [in]   | s32_t timeout | 超时时长 |

- 返回值

  无

- 注意事项

  本节点需支持health cli model



