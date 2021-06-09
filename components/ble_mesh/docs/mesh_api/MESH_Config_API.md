## Title:  Config

# 概述

本部分定义了蓝牙MESH Config接口，利用这些接口可以实现蓝牙MESH的配置信息的设置和读取。

# 接口定义

### bt_mesh_cfg_comp_data_get

- 函数原型

```c
int bt_mesh_cfg_comp_data_get(u16_t net_idx, 
                              u16_t addr, u8_t page,
                              u8_t *status, 
                              struct net_buf_simple *comp)
```

- 功能描述

获取节点的Composition数据

- 参数描述

| IN/OUT | NAME                        | DESC                                                    |
| ------ | --------------------------- | ------------------------------------------------------- |
| [in]   | u16_t net_idx               | Network Key索引                                         |
| [in]   | u16_t addr                  | 节点的unicast地址                                       |
| [in]   | u8_t page                   | 指定页，默认为0                                         |
| [out]  | u8_t *status                | 节点的Composition数据的状态<br />0：正常<br />非0：无效 |
| [out]  | struct net_buf_simple *comp | 获取到的Composition数据指针                             |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 获取成功 |
| 非0    | 获取失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_beacon_get**

- 函数原型

```c
int bt_mesh_cfg_beacon_get(u16_t net_idx, u16_t addr, u8_t *status)
```

- 功能描述

获取Beacon状态

- 参数描述

| IN/OUT | NAME          | DESC                                         |
| ------ | ------------- | -------------------------------------------- |
| [in]   | u16_t net_idx | Network Key索引                              |
| [in]   | u16_t addr    | 节点的unicast地址                            |
| [out]  | u8_t *status  | 节点的Beacon状态 <br />0： 关闭<br />1：开启 |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 获取成功 |
| 非0    | 获取失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_beacon_set**

- 函数原型

```c
int bt_mesh_cfg_beacon_set(u16_t net_idx, u16_t addr, u8_t val, u8_t *status)
```

- 功能描述

设置节点的Beacon状态

- 参数描述

| IN/OUT | NAME          | DESC                                  |
| ------ | ------------- | ------------------------------------- |
| [in]   | u16_t net_idx | Network Key索引                       |
| [in]   | u16_t addr    | 节点的unicast地址                     |
| [in]   | u8_t val      | 设置状态值<br />1： 开启<br />0：关闭 |
| [out]  | u8_t *status  | 节点的Beacon状态  0： 关闭 1：开启    |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_ttl_get**

- 函数原型

```c
int bt_mesh_cfg_ttl_get(u16_t net_idx, u16_t addr, u8_t *ttl)
```

- 功能描述

​       获取节点数据包的默认TTL值

- 参数描述

| IN/OUT | NAME          | DESC              |
| ------ | ------------- | ----------------- |
| [in]   | u16_t net_idx | Network Key索引   |
| [in]   | u16_t addr    | 节点的unicast地址 |
| [out]  | u8_t *ttl     | 网络数据包的TTL值 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_ttl_set**

- 函数原型

```c
int bt_mesh_cfg_ttl_set(u16_t net_idx, u16_t addr, u8_t val, u8_t *ttl)
```

- 功能描述

​       设置对端设备网络数据包默认TTL值

- 参数描述

| IN/OUT | NAME          | DESC              |
| ------ | ------------- | ----------------- |
| [in]   | u16_t net_idx | Network Key索引   |
| [in]   | u16_t addr    | 节点的unicast地址 |
| [in]   | u8_t val      | 设置的TTL值       |
| [out]  | u8_t *ttl     | 返回实际的TTL值   |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_friend_get**

- 函数原型

```c
int bt_mesh_cfg_friend_get(u16_t net_idx, u16_t addr, u8_t *status)
```

- 功能描述

​      获取对端节点friend属性状态

- 参数描述

| IN/OUT | NAME          | DESC                                 |
| ------ | ------------- | ------------------------------------ |
| [in]   | u16_t net_idx | Network Key索引                      |
| [in]   | u16_t addr    | 节点的unicast地址                    |
| [out]  | u8_t *status  | freind状态，0为关闭状态，1为开启状态 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_friend_set**

- 函数原型

```c
int bt_mesh_cfg_friend_set(u16_t net_idx, u16_t addr, u8_t val, u8_t *status)
```

- 功能描述

​       设置对端节点friend状态

- 参数描述

| IN/OUT | NAME          | DESC                              |
| ------ | ------------- | --------------------------------- |
| [in]   | u16_t net_idx | Network Key索引                   |
| [in]   | u16_t addr    | 节点的unicast地址                 |
| [in]   | u8_t val      | 设置的状态值， 1：开启； 0：关闭  |
| [out]  | u8_t *ttl     | 返回friend状态，1：开启； 0：关闭 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model 
  
  friend特性和lpn特性不能同时打开

### **bt_mesh_cfg_gatt_proxy_get**

- 函数原型

```c
int bt_mesh_cfg_gatt_proxy_get(u16_t net_idx, u16_t addr, u8_t *status)
```

- 功能描述

​       获取对端节点代理属性状态

- 参数描述

| IN/OUT | NAME          | DESC                                     |
| ------ | ------------- | ---------------------------------------- |
| [in]   | u16_t net_idx | Network Key索引                          |
| [in]   | u16_t addr    | 节点的unicast地址                        |
| [out]  | u8_t *status  | 代理节点状态<br />0： 关闭<br />1： 开启 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_gatt_proxy_set**

- 函数原型

```c
int bt_mesh_cfg_friend_set(u16_t net_idx, u16_t addr, u8_t val, u8_t *status)
```

- 功能描述

​      设置对端节点代理属性状态

- 参数描述

| IN/OUT | NAME          | DESC                                |
| ------ | ------------- | ----------------------------------- |
| [in]   | u16_t net_idx | Network Key索引                     |
| [in]   | u16_t addr    | 节点的unicast地址                   |
| [in]   | u8_t val      | 设置的状态值， 1：开启； 0：关闭    |
| [out]  | u8_t *status  | 返回代理节点状态，1：开启； 0：关闭 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### **bt_mesh_cfg_relay_get**

- 函数原型

```c
int bt_mesh_cfg_relay_get(u16_t net_idx,
                          u16_t addr,                                             u8_t *status,
			              u8_t *transmit)
```

- 功能描述

​       获取对端节点中继属性状态

- 参数描述

| IN/OUT | NAME           | DESC                                                  |
| ------ | -------------- | ----------------------------------------------------- |
| [in]   | u16_t net_idx  | Network Key索引                                       |
| [in]   | u16_t addr     | 节点的unicast地址                                     |
| [out]  | u8_t *status   | 中继状态值， 1：开启； 0：关闭                        |
| [out]  | u8_t *transmit | 网络传输计数，用于控制来自节点的网络PDU的消息传输数量 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model
  
  

### bt_mesh_cfg_relay_set

- 函数原型

```c
int bt_mesh_cfg_relay_set(u16_t net_idx, 
                        u16_t addr, 
                        u8_t new_relay,
                        u8_t new_transmit,
                        u8_t *status, 
                        u8_t *transmit)
```

- 功能描述

  设置对端节点中继属性状态

- 参数描述

| IN/OUT | NAME              | DESC                                           |
| ------ | ----------------- | ---------------------------------------------- |
| [in]   | u16_t net_idx     | Network Key索引                                |
| [in]   | u16_t addr        | 节点的unicast地址                              |
| [in]   | u8_t new_relay    | 需要设置的Relay状态<br />1： 开启<br />0：关闭 |
| [in]   | u8_t new_transmit | 需要设置的网络传输计数值                       |
| [out]  | u8_t *status      | 返回中继状态值                                 |
| [out]  | u8_t *transmit    | 返回网络传输计数                               |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_net_key_add

- 函数原型

```c
int bt_mesh_cfg_net_key_add(u16_t net_idx,
                            u16_t addr, 
                            u16_t key_net_idx,
                            const u8_t net_key[16], 
                            u8_t *status);
```

- 功能描述

​       增加一个netkey

- 参数描述

| IN/OUT | NAME                   | DESC                                    |
| ------ | ---------------------- | --------------------------------------- |
| [in]   | u16_t net_idx          | Network Key索引                         |
| [in]   | u16_t addr             | 节点的unicast地址                       |
| [in]   | u16_t key_net_idx      | net key的索引                           |
| [in]   | const u8_t net_key[16] | 需要设置的Netkey的值                    |
| [out]  | u8_t *status           | 返回状态；<br />0： 成功<br />非0：失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_app_key_add

- 函数原型

```c
int bt_mesh_cfg_app_key_add(u16_t net_idx,
                            u16_t addr, 
                            u16_t key_net_idx,
                            u16_t key_app_idx, 
                            const u8_t app_key[16],
                            u8_t *status)
```

- 功能描述

  增加一个APP Key

- 参数描述

| IN/OUT | NAME                   | DESC                                      |
| ------ | ---------------------- | ----------------------------------------- |
| [in]   | u16_t net_idx          | Network Key索引                           |
| [in]   | u16_t addr             | 节点的unicast地址                         |
| [in]   | u16_t key_net_idx      | net key的索引                             |
| [in]   | u16_t key_app_idx      | APP Key的索引                             |
| [in]   | const u8_t app_key[16] | 需要设置的APPkey的值                      |
| [out]  | u8_t *status           | 返回状态；<br /> 0： 成功<br /> 非0：失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_app_bind

- 函数原型

```c
int bt_mesh_cfg_mod_app_bind(u16_t net_idx,
                             u16_t addr,
                             u16_t elem_addr,
                             u16_t mod_app_idx, 
                             u16_t mod_id, 
                             u8_t *status)
```

- 功能描述

​       绑定一个SIG Model的APP Key

- 参数描述

| IN/OUT | NAME              | DESC                                             |
| ------ | ----------------- | ------------------------------------------------ |
| [in]   | u16_t net_idx     | Network Key索引                                  |
| [in]   | u16_t addr        | 节点的unicast地址                                |
| [in]   | u16_t elem_addr   | 元素地址                                         |
| [in]   | u16_t mod_app_idx | APP Key的索引                                    |
| [in]   | u16_t mod_id      | SIG Model的ID                                    |
| [out]  | u8_t *status      | 返回状态；<br />0： 绑定成功 <br />非0：绑定失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_app_bind_vnd

- 函数原型

```c
int bt_mesh_cfg_mod_app_bind_vnd(u16_t net_idx, 
                                 u16_t addr, 
                                 u16_t elem_addr,
                                 u16_t mod_app_idx, 
                                 u16_t mod_id, 
                                 u16_t cid,
                                 u8_t *status)
```

- 功能描述

​       绑定一个Vendor Model的APP Key

- 参数描述

| IN/OUT | NAME              | DESC                                   |
| ------ | ----------------- | -------------------------------------- |
| [in]   | u16_t net_idx     | Network Key索引                        |
| [in]   | u16_t addr        | 节点的unicast地址                      |
| [in]   | u16_t elem_addr   | 元素地址                               |
| [in]   | u16_t mod_app_idx | APP Key的索引                          |
| [in]   | u16_t mod_id      | Vendor Model的ID                       |
| [in]   | u16_t cid         | Vendor Model的Company ID               |
| [out]  | u8_t *status      | 返回状态； 0： 绑定成功  非0：绑定失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_pub_get

- 函数原型

```c
int bt_mesh_cfg_mod_pub_get(u16_t net_idx, u16_t addr, 
                            u16_t elem_addr,
			    			u16_t mod_id, 
                            struct bt_mesh_cfg_mod_pub *pub,
						    u8_t *status)
```

- 功能描述

​        获取SIG Model的发布配置参数

- 参数描述

| IN/OUT | NAME                            | DESC                                                       |
| ------ | ------------------------------- | ---------------------------------------------------------- |
| [in]   | u16_t net_idx                   | Network Key索引                                            |
| [in]   | u16_t addr                      | 节点的unicast地址                                          |
| [in]   | u16_t elem_addr                 | 元素地址                                                   |
| [in]   | u16_t mod_id                    | SIG Model的ID                                              |
| [out]  | struct bt_mesh_cfg_mod_pub *pub | 发布配置参数，参见struct bt_mesh_cfg_mod_pub（结构体）定义 |
| [out]  | u8_t *status                    | 返回状态； 0： 获取成功  非0：获取失败                     |

| struct bt_mesh_cfg_mod_pub（结构体）定义 |                                                              |
| ---------------------------------------- | ------------------------------------------------------------ |
| u16_t  addr                              | 发布地址                                                     |
| u16_t  app_idx                           | APP Key索引                                                  |
| bool   cred_flag                         | 安全机制<br />0：使用Master机制<br />1：使用Friend机制       |
| u8_t   ttl                               | 消息的生存时间                                               |
| u8_t   period                            | 发布周期                                                     |
| u8_t   transmit                          | 重传次数= transmit & 0x0111<br />重传间隔=(transmit / 8 +1)*10ms |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_pub_get_vnd

- 函数原型

```c
int bt_mesh_cfg_mod_pub_get_vnd(u16_t net_idx, 
                                u16_t addr, 
                                u16_t elem_addr,
                                u16_t mod_id, 
                                u16_t cid,
                                struct bt_mesh_cfg_mod_pub *pub, 
                                u8_t *status)
```

- 功能描述

​       获取Vendor Model的发布配置参数

- 参数描述

| IN/OUT | NAME                            | DESC                                                       |
| ------ | ------------------------------- | ---------------------------------------------------------- |
| [in]   | u16_t net_idx                   | Network Key索引                                            |
| [in]   | u16_t addr                      | 节点的unicast地址                                          |
| [in]   | u16_t elem_addr                 | 元素地址                                                   |
| [in]   | u16_t mod_id                    | Vendor Model的ID                                           |
| [in]   | u16_t cid                       | Vendor Model的Company ID                                   |
| [out]  | struct bt_mesh_cfg_mod_pub *pub | 发布配置参数，参见struct bt_mesh_cfg_mod_pub（结构体）定义 |
| [out]  | u8_t *status                    | 返回状态； 0： 获取成功  非0：获取失败                     |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_del

- 函数原型

```c
int bt_mesh_cfg_mod_sub_del(u16_t net_idx, 
                            u16_t addr, 
                            u16_t elem_addr,
                            u16_t sub_addr, 
                            u16_t mod_id, 
                            u8_t *status)
```

- 功能描述

​       删除SIG Model消息订阅地址

- 参数描述

| IN/OUT | NAME            | DESC                                   |
| ------ | --------------- | -------------------------------------- |
| [in]   | u16_t net_idx   | Network Key索引                        |
| [in]   | u16_t addr      | 节点的unicast地址                      |
| [in]   | u16_t elem_addr | 元素地址                               |
| [in]   | u16_t sub_addr  | 订阅地址                               |
| [in]   | u16_t mod_id    | SIG Model的ID                          |
| [out]  | u8_t *status    | 返回状态； 0： 取消成功  非0：取消失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_del_vnd

- 函数原型

```c
int bt_mesh_cfg_mod_sub_del_vnd(u16_t net_idx, 
                                u16_t addr, 
                                u16_t elem_addr,
                                u16_t sub_addr, 
                                u16_t mod_id, 
                                u16_t cid,
                                u8_t *status)
```

- 功能描述

​       删除vendor Model的消息订阅地址

- 参数描述

| IN/OUT | NAME            | DESC                                   |
| ------ | --------------- | -------------------------------------- |
| [in]   | u16_t net_idx   | Network Key索引                        |
| [in]   | u16_t addr      | 节点的unicast地址                      |
| [in]   | u16_t elem_addr | 元素地址                               |
| [in]   | u16_t sub_addr  | 订阅地址                               |
| [in]   | u16_t mod_id    | Vendor Model的ID                       |
| [in]   | u16_t cid       | Vendor Model的Company ID               |
| [out]  | u8_t *status    | 返回状态； 0： 取消成功  非0：取消失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_overwrite

- 函数原型

```c
int bt_mesh_cfg_mod_sub_overwrite(u16_t net_idx,
                                  u16_t addr,
                                  u16_t elem_addr,
                                  u16_t sub_addr, 
                                  u16_t mod_id, 
                                  u8_t *status)
```

- 功能描述

​       擦除SIG Model所有订阅非虚拟地址，并增加sub_addr订阅地址

- 参数描述

| IN/OUT | NAME            | DESC                                   |
| ------ | --------------- | -------------------------------------- |
| [in]   | u16_t net_idx   | Network Key索引                        |
| [in]   | u16_t addr      | 节点的unicast地址                      |
| [in]   | u16_t elem_addr | 元素地址                               |
| [in]   | u16_t sub_addr  | 订阅地址                               |
| [in]   | u16_t mod_id    | SIG Model的ID                          |
| [out]  | u8_t *status    | 返回状态； 0： 重写成功  非0：重写失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_overwrite_vnd

- 函数原型

```c
int bt_mesh_cfg_mod_sub_overwrite_vnd(u16_t net_idx,
                                  u16_t addr,
                                  u16_t elem_addr,
                                  u16_t sub_addr, 
                                  u16_t mod_id, 
                                  u16_t cid, 
                                  u8_t *status)
```

- 功能描述

​        擦除Vendor Model所有订阅非虚拟地址，并增加新的订阅地址sub_addr

- 参数描述

| IN/OUT | NAME            | DESC                                   |
| ------ | --------------- | -------------------------------------- |
| [in]   | u16_t net_idx   | Network Key索引                        |
| [in]   | u16_t addr      | 节点的unicast地址                      |
| [in]   | u16_t elem_addr | 元素地址                               |
| [in]   | u16_t sub_addr  | 订阅地址                               |
| [in]   | u16_t mod_id    | SIG Model的ID                          |
| [in]   | u16_t cid       | Vendor Model的Company ID               |
| [out]  | u8_t *status    | 返回状态； 0： 重写成功  非0：重写失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_va_add

- 函数原型

```c
int bt_mesh_cfg_mod_sub_va_add(u16_t net_idx,
                               u16_t addr, 
                               u16_t elem_addr,
                               const u8_t label[16],
                               u16_t mod_id,
                               u16_t *virt_addr,
                               u8_t *status)
```

- 功能描述

​        针对SIG Model增加一个虚拟地址的消息订阅

- 参数描述

| IN/OUT | NAME                 | DESC                                   |
| ------ | -------------------- | -------------------------------------- |
| [in]   | u16_t net_idx        | Network Key索引                        |
| [in]   | u16_t addr           | 节点的unicast地址                      |
| [in]   | u16_t elem_addr      | 元素地址                               |
| [in]   | const u8_t label[16] | Label UUID                             |
| [in]   | u16_t mod_id         | SIG Model的ID                          |
| [in]   | u16_t *virt_addr     | 返回Label UUID对应的虚拟地址           |
| [out]  | u8_t *status         | 返回状态； 0： 订阅成功  非0：订阅失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_va_add_vnd

- 函数原型

```c
int bt_mesh_cfg_mod_sub_va_add_vnd(u16_t net_idx, 
                                   u16_t addr, 
                                   u16_t elem_addr,
                                   const u8_t label[16], 
                                   u16_t mod_id,       
                                   u16_t cid,
                                   u16_t *virt_addr, 
                                   u8_t *status)
```

- 功能描述

​      针对Vendor Model增加一个虚拟地址的消息订阅

- 参数描述

| IN/OUT | NAME                 | DESC                                   |
| ------ | -------------------- | -------------------------------------- |
| [in]   | u16_t net_idx        | Network Key索引                        |
| [in]   | u16_t addr           | 节点的unicast地址                      |
| [in]   | u16_t elem_addr      | 元素地址                               |
| [in]   | const u8_t label[16] | Label UUID                             |
| [in]   | u16_t mod_id         | Vendor Model的ID                       |
| [in]   | u16_t cid            | Vendor Model的Company ID               |
| [in]   | u16_t *virt_addr     | 返回Label UUID对应的虚拟地址           |
| [out]  | u8_t *status         | 返回状态； 0： 订阅成功  非0：订阅失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_va_del

- 函数原型

```c
int bt_mesh_cfg_mod_sub_va_del(u16_t net_idx, 
                               u16_t addr, 
                               u16_t elem_addr,   
                               const u8_t label[16],
                               u16_t mod_id,
                               u16_t *virt_addr,
                               u8_t *status)
```

- 功能描述

​      取消一个SIG Model的虚拟地址消息订阅

- 参数描述

| IN/OUT | NAME                 | DESC                                           |
| ------ | -------------------- | ---------------------------------------------- |
| [in]   | u16_t net_idx        | Network Key索引                                |
| [in]   | u16_t addr           | 节点的unicast地址                              |
| [in]   | u16_t elem_addr      | 元素地址                                       |
| [in]   | const u8_t label[16] | Label UUID                                     |
| [in]   | u16_t mod_id         | Vendor Model的ID                               |
| [in]   | u16_t cid            | Vendor Model的Company ID                       |
| [in]   | u16_t *virt_addr     | 返回Label UUID对应的虚拟地址                   |
| [out]  | u8_t *status         | 返回状态； 0： 取消订阅成功  非0：取消订阅失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_va_del_vnd

- 函数原型

```c
int bt_mesh_cfg_mod_sub_va_del_vnd(u16_t net_idx, 
                                   u16_t addr, 
                                   u16_t elem_addr,
                                   const u8_t label[16], 
                                   u16_t mod_id,
                                   u16_t cid,
                                   u16_t *virt_addr, 
                                   u8_t *status)
```

- 功能描述

​       取消一个Vendor Model的虚拟地址消息订阅

- 参数描述

| IN/OUT | NAME                 | DESC                                           |
| ------ | -------------------- | ---------------------------------------------- |
| [in]   | u16_t net_idx        | Network Key索引                                |
| [in]   | u16_t addr           | 节点的unicast地址                              |
| [in]   | u16_t elem_addr      | 元素地址                                       |
| [in]   | const u8_t label[16] | Label UUID                                     |
| [in]   | u16_t mod_id         | Vendor Model的ID                               |
| [in]   | u16_t cid            | Vendor Model的Company ID                       |
| [in]   | u16_t *virt_addr     | 返回Label UUID对应的虚拟地址                   |
| [out]  | u8_t *status         | 返回状态； 0： 取消订阅成功  非0：取消订阅失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_va_overwrite

- 函数原型

```c
int bt_mesh_cfg_mod_sub_va_overwrite(u16_t net_idx, 
                                     u16_t addr,
                                     u16_t elem_addr,
                                     const u8_t label[16],
                                     u16_t mod_id,
                                     u16_t *virt_addr,
                                     u8_t *status)
```

- 功能描述

​       重写一个SIG Model的虚拟地址消息订阅

- 参数描述

| IN/OUT | NAME                 | DESC                                   |
| ------ | -------------------- | -------------------------------------- |
| [in]   | u16_t net_idx        | Network Key索引                        |
| [in]   | u16_t addr           | 节点的unicast地址                      |
| [in]   | u16_t elem_addr      | 元素地址                               |
| [in]   | const u8_t label[16] | Label UUID                             |
| [in]   | u16_t mod_id         | SIG Model的ID                          |
| [in]   | u16_t *virt_addr     | 返回Label UUID对应的虚拟地址           |
| [out]  | u8_t *status         | 返回状态； 0： 重写成功  非0：重写失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_mod_sub_va_overwrite_vnd

- 函数原型

```c
int bt_mesh_cfg_mod_sub_va_overwrite_vnd(u16_t net_idx, 
                                         u16_t addr,
                                         u16_t elem_addr, 
                                         const u8_t label[16],
                                         u16_t mod_id, 
                                         u16_t cid,
                                         u16_t *virt_addr, 
                                         u8_t *status)
```

- 功能描述

重写一个Vendor Model的虚拟地址消息订阅

- 参数描述

| IN/OUT | NAME                 | DESC                                   |
| ------ | -------------------- | -------------------------------------- |
| [in]   | u16_t net_idx        | Network Key索引                        |
| [in]   | u16_t addr           | 节点的unicast地址                      |
| [in]   | u16_t elem_addr      | 元素地址                               |
| [in]   | const u8_t label[16] | Label UUID                             |
| [in]   | u16_t mod_id         | Vendor Model的ID                       |
| [in]   | u16_t cid            | Vendor Model的Company ID               |
| [in]   | u16_t *virt_addr     | 返回Label UUID对应的虚拟地址           |
| [out]  | u8_t *status         | 返回状态； 0： 重写成功  非0：重写失败 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_hb_pub_set

- 函数原型

```c
int bt_mesh_cfg_hb_pub_set(u16_t net_idx, 
                           u16_t addr,
                           const struct bt_mesh_cfg_hb_pub *pub, 
                           u8_t *status)
```

- 功能描述

​      设置Heartbeat消息发布参数

- 参数描述

| IN/OUT | NAME                                 | DESC                                                         |
| ------ | ------------------------------------ | ------------------------------------------------------------ |
| [in]   | u16_t net_idx                        | Network Key索引                                              |
| [in]   | u16_t addr                           | 节点的unicast地址                                            |
| [in]   | const struct bt_mesh_cfg_hb_pub *pub | HeartBeat消息结构体指针，参见struct bt_mesh_cfg_hb_pub（结构体）定义 |
| [out]  | u8_t *status                         | 返回状态； 0： 设置成功  非0：设置失败                       |

| struct bt_mesh_cfg_hb_pub（结构体）定义 |                                            |
| --------------------------------------- | ------------------------------------------ |
| u16_t  dst                              | 心跳包的目标地址                           |
| u8_t  count                             | 剩余的待发送的心跳包个数                   |
| u8_t  period                            | 心跳包发送间隔                             |
| u8_t   ttl                              | 心跳包的生存时长                           |
| u8_t   feat                             | 心跳包发送时，改变的特征。参见feat参数说明 |
| u16_t net_idx                           | NetKey索引值                               |

| Feat参数说明           |                                              |
| ---------------------- | -------------------------------------------- |
| Relay = BIT(0)         | 0：未发生Relay变化；1：发生Relay变化         |
| Proxy = BIT(1)         | 0：未发生Proxy变化；1：发生Proxy变化         |
| Friend = BIT(2)        | 0：未发生Friend变化；1：发生Friend变化       |
| Low Power = BIT(3)     | 0：未发生low power变化；1：发生low power变化 |
| RFU = BIT(4) ~ BIT(15) | 预留                                         |



- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_hb_pub_get

- 函数原型

```c
int bt_mesh_cfg_hb_pub_get(u16_t net_idx,
                           u16_t addr,
                           struct bt_mesh_cfg_hb_pub *pub, 
                           u8_t *status)
```

- 功能描述

获取Heartbeat消息发布参数

- 参数描述

| IN/OUT | NAME                                 | DESC                                                         |
| ------ | ------------------------------------ | ------------------------------------------------------------ |
| [in]   | u16_t net_idx                        | Network Key索引                                              |
| [in]   | u16_t addr                           | 节点的unicast地址                                            |
| [in]   | const struct bt_mesh_cfg_hb_pub *pub | HeartBeat消息结构体指针，参见struct bt_mesh_cfg_hb_pub（结构体）定义 |
| [out]  | u8_t *status                         | 返回状态； 0： 获取成功  非0：获取失败                       |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_hb_sub_set

- 函数原型

```c
int bt_mesh_cfg_hb_sub_set(u16_t net_idx, 
                           u16_t addr,
                           struct bt_mesh_cfg_hb_sub *sub, 
                           u8_t *status)
```

- 功能描述

​      设置Heartbeat消息订阅参数

- 参数描述

| IN/OUT | NAME                           | DESC                                                         |
| ------ | ------------------------------ | ------------------------------------------------------------ |
| [in]   | u16_t net_idx                  | Network Key索引                                              |
| [in]   | u16_t addr                     | 节点的unicast地址                                            |
| [in]   | struct bt_mesh_cfg_hb_sub *sub | HeartBeat消息结构体指针，参见struct bt_mesh_cfg_hb_sub（结构体）定义 |
| [out]  | u8_t *status                   | 返回状态； 0： 设置成功  非0：设置失败                       |

| struct bt_mesh_cfg_hb_sub（结构体）定义 |                                      |
| --------------------------------------- | ------------------------------------ |
| u16_t  src                              | 心跳包的源地址                       |
| u16_t  dst                              | 心跳包的目的地址                     |
| u8_t  period                            | 心跳包发送间隔                       |
| u8_t   count                            | 已收到的心跳包个数                   |
| u8_t   min                              | 心跳包的最小TTL值，范围：0x00 ~ 0x7F |
| u8_t   max                              | 心跳包的最大TTL值，范围：0x00 ~ 0x7F |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_hb_sub_get

- 函数原型

```c
int bt_mesh_cfg_hb_sub_get(u16_t net_idx, 
                           u16_t addr,
                           struct bt_mesh_cfg_hb_sub *sub, 
                           u8_t *status)
```

- 功能描述

​      获取Heartbeat消息订阅参数

- 参数描述

| IN/OUT | NAME                           | DESC                                                         |
| ------ | ------------------------------ | ------------------------------------------------------------ |
| [in]   | u16_t net_idx                  | Network Key索引                                              |
| [in]   | u16_t addr                     | 节点的unicast地址                                            |
| [in]   | struct bt_mesh_cfg_hb_sub *sub | HeartBeat消息结构体指针，参见struct bt_mesh_cfg_hb_sub（结构体）定义 |
| [out]  | u8_t *status                   | 返回状态； 0： 设置成功  非0：设置失败                       |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model