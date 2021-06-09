## Title:  Provisioner

# 概述

本部分定义了蓝牙MESH Provisioner接口，利用这些接口可以实现蓝牙MESH的配置器功能。

# 接口定义

### bt_mesh_provisioner_store_node_info

- 函数原型

```c
int bt_mesh_provisioner_store_node_info(struct bt_mesh_node_t *node_info)
```

- 功能描述

​       存储被prov devices节点信息

- 参数描述

| IN/OUT | NAME                             | DESC                                                  |
| ------ | -------------------------------- | ----------------------------------------------------- |
| [in]   | struct bt_mesh_node_t *node_info | MESH节点指针，参见struct bt_mesh_node_t（结构体）定义 |

| struct bt_mesh_node_t（结构体）定义 |                                                 |
| ----------------------------------- | ----------------------------------------------- |
| char  node_name[MESH_NAME_SIZE]     | 节点名称                                        |
| u8_t  dev_uuid[16]                  | 节点UUID                                        |
| u16_t oob_info                      | 节点OOB信息                                     |
| u16_t unicast_addr                  | 节点的Primary Element单播地址                   |
| u8_t  element_num                   | 节点的Element个数                               |
| u16_t net_idx                       | 节点的NetKey索引值                              |
| u8_t  flags                         | 节点的device key 及IV更新标记                   |
| u32_t iv_index                      | IV索引值                                        |
| u8_t  dev_key[16]                   | 节点的Device Key                                |
| bool  node_active                   | 节点激活标记<br />0：未激活<br />1：已激活      |
| u8_t  addr_val[6]                   | 节点的MAC地址                                   |
| u8_t  addr_type:4                   | 节点MAC地址类型<br />0 - Public<br />1 - Random |
| u8_t  flag:4                        | 节点信息是否已存在                              |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_get_all_node_unicast_addr

- 函数原型

```c
int bt_mesh_provisioner_get_all_node_unicast_addr(struct net_buf_simple *buf)
```

- 功能描述

​     获取配网器所有节点中Primary Element的单播地址

- 参数描述

| IN/OUT | NAME                       | DESC              |
| ------ | -------------------------- | ----------------- |
| [out]  | struct net_buf_simple *buf | 存储数据的buf指针 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_set_node_name

- 函数原型

```c
int bt_mesh_provisioner_set_node_name(int node_index, const char *name)
```

- 功能描述

​       provisioner设置节点名称

- 参数描述

| IN/OUT | NAME             | DESC     |
| ------ | ---------------- | -------- |
| [in]   | int node_index   | 节点索引 |
| [in]   | const char *name | 节点名称 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_get_node_name

- 函数原型

```c
const char *bt_mesh_provisioner_get_node_name(int node_index)
```

- 功能描述

​       获取索引值对应的节点的名称

- 参数描述

| IN/OUT | NAME           | DESC     |
| ------ | -------------- | -------- |
| [in]   | int node_index | 节点索引 |

- 返回值

  返回节点名称的字符串指针

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_get_node_index

- 函数原型

```c
int bt_mesh_provisioner_get_node_index(const char *name)
```

- 功能描述

​       根据节点名称获取对应的索引值

- 参数描述

| IN/OUT | NAME             | DESC                 |
| ------ | ---------------- | -------------------- |
| [in]   | const char *name | 节点名称的字符串指针 |

- 返回值

| 返回值 |        |
| ------ | ------ |
| >= 0   | 索引值 |
| < 0    | 失败   |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_get_node_info

- 函数原型

```c
struct bt_mesh_node_t *bt_mesh_provisioner_get_node_info(u16_t unicast_addr)
```

- 功能描述

​       通过节点地址获取节点信息

- 参数描述

| IN/OUT | NAME               | DESC            |
| ------ | ------------------ | --------------- |
| [in]   | u16_t unicast_addr | Network Key索引 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| NULL   | 失败 |
| 非NULL | 成功 |

- 注意事项

  使用前需使能provisioner功能

### **bt_mesh_provisioner_get_net_key_count**

- 函数原型

```c
u32_t bt_mesh_provisioner_get_net_key_count(void)
```

- 功能描述

​       获取MESH网络中Network Key的数量

- 参数描述

  无

- 返回值

| 返回值 |                  |
| ------ | ---------------- |
| 0      | 无               |
| > 0    | 网络中netkey数量 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_get_app_key_count

- 函数原型

```c
u32_t bt_mesh_provisioner_get_app_key_count(void)
```

- 功能描述

​       获取MESH网络中APP Key的数量

- 参数描述

  无

- 返回值

| 返回值 |                     |
| ------ | ------------------- |
| 0      | 无                  |
| > 0    | 网络中APP Key的数量 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_local_app_key_add

- 函数原型

```c
int bt_mesh_provisioner_local_app_key_add(const u8_t app_key[16], u16_t net_idx, u16_t *app_idx)
```

- 功能描述

​     添加配网器的APP Key

- 参数描述

| IN/OUT | NAME                   | DESC                          |
| ------ | ---------------------- | ----------------------------- |
| [in]   | const u8_t app_key[16] | APP Key值                     |
| [in]   | u16_t net_idx          | Network Key索引值             |
| [out]  | u16_t *app_idx         | APP Key添加成功后返回的索引值 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_local_app_key_get

- 函数原型

```c
const u8_t *bt_mesh_provisioner_local_app_key_get(u16_t net_idx, u16_t app_idx)
```

- 功能描述

​      根据Network Key索引和APP Key索引查找APP Key值

- 参数描述

| IN/OUT | NAME          | DESC            |
| ------ | ------------- | --------------- |
| [in]   | u16_t net_idx | Network Key索引 |
| [in]   | u16_t app_idx | APP Key索引     |

- 返回值

| 返回值 |                         |
| ------ | ----------------------- |
| NULL   | 失败                    |
| 非NULL | 16字节的APP Key数组指针 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_local_app_key_delete

- 函数原型

```c
int bt_mesh_provisioner_local_app_key_delete(u16_t net_idx, u16_t app_idx)
```

- 功能描述

  根据输入的netkey索引和appkey索引，删除appkey

- 参数描述

| IN/OUT | NAME          | DESC            |
| ------ | ------------- | --------------- |
| [in]   | u16_t net_idx | Network Key索引 |
| [in]   | u16_t app_idx | APP Key索引     |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_local_net_key_add

- 函数原型

```c
int bt_mesh_provisioner_local_net_key_add(const u8_t net_key[16], u16_t *net_idx)
```

- 功能描述

​       配网器添加一个netkey

- 参数描述

| IN/OUT | NAME                   | DESC                                |
| ------ | ---------------------- | ----------------------------------- |
| [in]   | const u8_t net_key[16] | Network Key值                       |
| [in]   | u16_t *net_idx         | 添加成功后，返回Network Key的索引值 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_local_net_key_get

- 函数原型

```c
const u8_t *bt_mesh_provisioner_local_net_key_get(u16_t net_idx)
```

- 功能描述

​       根据netkey索引获得netkey的值

- 参数描述

| IN/OUT | NAME          | DESC            |
| ------ | ------------- | --------------- |
| [in]   | u16_t net_idx | Network Key索引 |

- 返回值

| 返回值 |                             |
| ------ | --------------------------- |
| NULL   | 失败                        |
| 非NULL | 16字节的Network Key数组指针 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_local_net_key_delete

- 函数原型

```c
int bt_mesh_provisioner_local_net_key_delete(u16_t net_idx)
```

- 功能描述

​       根据输入的netkey索引，删除netkey

- 参数描述

| IN/OUT | NAME          | DESC            |
| ------ | ------------- | --------------- |
| [in]   | u16_t net_idx | Network Key索引 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_get_own_unicast_addr

- 函数原型

```c
int bt_mesh_provisioner_get_own_unicast_addr(u16_t *addr, u8_t *elem_num)
```

- 功能描述

​     获取配网器的单播地址和元素数量

- 参数描述

| IN/OUT | NAME           | DESC                 |
| ------ | -------------- | -------------------- |
| [out]  | u16_t *addr    | 将返回配网器单播地址 |
| [out]  | u8_t *elem_num | 将返回配网器元素数量 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_bind_local_model_app_idx

- 函数原型

```c
int bt_mesh_provisioner_bind_local_model_app_idx(u16_t elem_addr, u16_t mod_id,
        u16_t cid, u16_t app_idx)
```

- 功能描述

​      配网器本地model绑定appkey

- 参数描述

| IN/OUT | NAME            | DESC                                                         |
| ------ | --------------- | ------------------------------------------------------------ |
| [in]   | u16_t elem_addr | 元素地址                                                     |
| [in]   | u16_t mod_id    | Model ID                                                     |
| [in]   | u16_t cid       | 该值为0xFFFF时，为SIG Model；<br />非0xFFFF时，为Vendor Model |
| [in]   | u16_t app_idx   | APP Key索引值                                                |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_print_local_element_info

- 函数原型

```c
int bt_mesh_provisioner_print_local_element_info(void)
```

- 功能描述

​       打印配网器的所有元素信息，如：cid、pid、vid、元素数量以及元素内的所有信息

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_print_node_info

- 函数原型

```c
int bt_mesh_provisioner_print_node_info(void)
```

- 功能描述

​      打印入网节点的信息，如节点名称、UUID、MAC地址、MAC地址类型、单播地址、元素数量、Network Key索引等

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能
  
  

### bt_mesh_is_provisioner_en

- 函数原型

```c
bool bt_mesh_is_provisioner_en(void)
```

- 功能描述

​       配网功能是否使能

- 参数描述

  无

- 返回值

| 返回值 |        |
| ------ | ------ |
| 0      | 未使能 |
| 1      | 已使能 |

- 注意事项

  无

### bt_mesh_provisioner_p_app_key_alloc

- 函数原型

```c
struct bt_mesh_app_key  *bt_mesh_provisioner_p_app_key_alloc()
```

- 功能描述

申请16字节的APP Key内存空间

- 参数描述

  无

- 返回值

| 返回值 |                 |
| ------ | --------------- |
| NULL   | APP Key存储已满 |
| 非NULL | 申请成功        |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_get_node_info_by_id

- 函数原型

```c
struct bt_mesh_node_t *bt_mesh_provisioner_get_node_info_by_id(int node_index)
```

- 功能描述

根据节点索引获取节点信息

- 参数描述

| IN/OUT | NAME           | DESC     |
| ------ | -------------- | -------- |
| [in]   | int node_index | 节点索引 |

- 返回值

| 返回值 |                      |
| ------ | -------------------- |
| NULL   | 成功                 |
| 非NULL | 返回查询到的节点指针 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_add_unprov_dev

- 函数原型

```c
int bt_mesh_provisioner_add_unprov_dev(struct bt_mesh_unprov_dev_add *add_dev, u8_t flags)
```

- 功能描述

​      将未入网设备信息加入未入网设备列表

- 参数描述

| IN/OUT | NAME                                   | DESC                                                         |
| ------ | -------------------------------------- | ------------------------------------------------------------ |
| [in]   | struct bt_mesh_unprov_dev_add *add_dev | 未入网设备列表                                               |
| [in]   | u8_t flags                             | BIT0：在设备成功入网后，从未入网设备列表中清除该设备的信息<br />BIT1:当设备被加入到未入网设备列表后，立即进行配置<br />BIT2: 未入网设备列表已满时可以刷新设备 |

| struct bt_mesh_unprov_dev_add（结构体）定义 |                    |
| ------------------------------------------- | ------------------ |
| u8_t  addr[6]                               | 设备的MAC地址      |
| u8_t  addr_type                             | 设备的MAC地址类型  |
| u8_t  uuid[16]                              | 设备的UUID         |
| u16_t oob_info                              | 设备的OOB信息      |
| u8_t  bearer                                | 设备的广播承载类型 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  如果加入的设备的UUID、MAC地址、MAC地址类型已经存在于队列中，但是广播承载类型不一致，增加设备的操作仍然会成功，并且会更新广播承载类型的信息

### bt_mesh_provisioner_delete_device

- 函数原型

```c
int bt_mesh_provisioner_delete_device(struct bt_mesh_device_delete *del_dev)
```

- 功能描述

​        从队列中删除设备，重置当前的配置信息和节点信息

- 参数描述

| IN/OUT | NAME                                  | DESC                                                         |
| ------ | ------------------------------------- | ------------------------------------------------------------ |
| [in]   | struct bt_mesh_device_delete *del_dev | 即将删除的设备指针，参见struct bt_mesh_device_delete（结构体）定义 |

| struct bt_mesh_device_delete（结构体）定义 |                   |
| ------------------------------------------ | ----------------- |
| u8_t  addr[6]                              | 节点的MAC地址     |
| u8_t  addr_type                            | 节点的MAC地址类型 |
| u8_t  uuid[16]                             | 节点的UUID        |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_set_dev_uuid_match

- 函数原型

```c
int bt_mesh_provisioner_set_dev_uuid_match(u8_t offset, 
                                           u8_t length,
                                           const u8_t *match, 
                                           bool prov_flag)
```

- 功能描述

​        在设备入网前，比较设备UUID的信息

- 参数描述

| IN/OUT | NAME              | DESC                                                         |
| ------ | ----------------- | ------------------------------------------------------------ |
| [in]   | u8_t offset       | 要比较的uuid的offset值                                       |
| [in]   | u8_t length       | 要比较的uuid的长度                                           |
| [in]   | const u8_t *match | 比较的值                                                     |
| [in]   | bool prov_flag    | 该标识指示接收到uuid_match的adv包时，设备需要立即进行provision还是给应用层上报。<br />0：给应用层上报；1：立即进行provision |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### prov_adv_pkt_cb

- 函数原型

```c
typedef void (*prov_adv_pkt_cb)(const u8_t addr[6], 
                                const u8_t addr_type,
                                const u8_t adv_type, 
                                const u8_t dev_uuid[16],
                                u16_t oob_info, 
                                bt_mesh_prov_bearer_t bearer)
```

- 功能描述

​         定义了一个回调函数，当配网器收到未入网设备的adv包，且该设备不在provisioner的未配置设备队列中

- 参数描述

| IN/OUT | NAME                         | DESC                                                         |
| ------ | ---------------------------- | ------------------------------------------------------------ |
| [in]   | const u8_t addr[6]           | 未入网设备的MAC地址                                          |
| [in]   | const u8_t addr_type         | 未入网设备的MAC地址类型                                      |
| [in]   | const u8_t adv_type          | 广播类型<br />0x00: ADV_IND<br />0x01: ADV_DIRECT_IND<br />0x02: ADV_SCAN_IND<br />0x03: ADV_NONCONN_IND<br />0x04: ADV_DIRECT_IND_LOW_DUTY |
| [in]   | const u8_t dev_uuid[16]      | 未入网设备的UUID                                             |
| [in]   | u16_t oob_info               | 未入网设备的OOB信息                                          |
| [in]   | bt_mesh_prov_bearer_t bearer | 广播承载类型<br />0：ADV<br />1：GATT                        |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_prov_adv_pkt_cb_register

- 函数原型

```c
int bt_mesh_prov_adv_pkt_cb_register(prov_adv_pkt_cb cb)
```

- 功能描述

​       用来通知应用层收到了mesh组网过程中的adv包或者未入网设备发送的beacon包（设备不在未入网设备列表中）

- 参数描述

| IN/OUT | NAME               | DESC                            |
| ------ | ------------------ | ------------------------------- |
| [in]   | prov_adv_pkt_cb cb | 参见prov_adv_pkt_cb回调函数定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_provisioner_set_prov_data_info

- 函数原型

```c
int bt_mesh_provisioner_set_prov_data_info(struct bt_mesh_prov_data_info *info)
```

- 功能描述

​        设置配置信息中的Network Key索引或者IV索引

- 参数描述

| IN/OUT | NAME                                | DESC                                  |
| ------ | ----------------------------------- | ------------------------------------- |
| [in]   | struct bt_mesh_prov_data_info *info | 包含了Network Key索引或者IV索引的信息 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能

### bt_mesh_prov_input_data

- 函数原型

```c
int bt_mesh_prov_input_data(u8_t *num, u8_t size, bool num_flag)
```

- 功能描述

​       OOB信息输入

- 参数描述

| IN/OUT | NAME          | DESC                                       |
| ------ | ------------- | ------------------------------------------ |
| [in]   | u8_t *num     | 输入的数据内容                             |
| [in]   | u8_t size     | 输入数据的大小                             |
| [in]   | bool num_flag | 输入的数据类型<br />0：字符串<br />1：数字 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能 ; prov 认证方式是 OUTPUT OOB时使用

### bt_mesh_prov_output_data

- 函数原型

```c
int bt_mesh_prov_output_data(u8_t *num, u8_t size, bool num_flag)
```

- 功能描述

​       OOB信息输出

- 参数描述

| IN/OUT | NAME          | DESC                                         |
| ------ | ------------- | -------------------------------------------- |
| [in]   | u8_t *num     | 输出的数据内容                               |
| [in]   | u8_t size     | 输出数据的大小                               |
| [in]   | bool num_flag | 输出的数据类型<br /> 0：字符串 <br />1：数字 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需使能provisioner功能 ; prov 认证方式是 INPUT OOB时使用