## Title:  ACCESS

# 概述

本部分定义了蓝牙MESH ACCESS接口，利用这些接口可以实现蓝牙MESH协议栈初始化、特性配置、消息发送等功能。

# 接口定义

### bt_mesh_init

- 函数原型

```c
int bt_mesh_init(const struct bt_mesh_prov *prov, 
                 const struct bt_mesh_comp *comp,
                 const struct bt_mesh_provisioner *provisioner)
```

- 功能描述

蓝牙MESH协议栈初始化

- 参数描述

| IN/OUT | NAME                                          | DESC                                                      |
| ------ | --------------------------------------------- | --------------------------------------------------------- |
| [in]   | const struct bt_mesh_prov *prov               | Provisioning配置参数                                      |
| [in]   | const struct bt_mesh_comp *comp               | Composition配置参数                                       |
| [in]   | const struct bt_mesh_provisioner *provisioner | Provisioner配置参数，设备无需支持配网器功能时，设置为NULL |

| struct bt_mesh_prov（结构体）定义                            |                                                        |
| ------------------------------------------------------------ | ------------------------------------------------------ |
| const u8_t *uuid                                             | 待入网设备的通用唯一标识符                             |
| const char *uri                                              | 待入网设备的统一资源标识符                             |
| bt_mesh_prov_oob_info_t oob_info                             | OOB交换方式，参见bt_mesh_prov_oob_info_t枚举定义       |
| const u8_t *static_val                                       | 静态OOB值                                              |
| u8_t static_val_len                                          | 静态OOB值的长度                                        |
| u8_t output_size                                             | 输出OOB长度                                            |
| u16_t output_actions                                         | 参见bt_mesh_output_action_t枚举定义                    |
| u8_t input_size                                              | 输入OOB长度                                            |
| u16_t input_actions                                          | 参见bt_mesh_input_action_t枚举定义                     |
| int (*output_number)(bt_mesh_output_action_t act, u32_t num) | 回调函数，用于OOB数字信息的显示                        |
| int (*output_string)(const char *str)                        | 回调函数，用于OOB字符串信息的显示                      |
| int (*input)(bt_mesh_input_action_t act, u8_t size)          | 回调函数，用于OOB信息输入的通知                        |
| void (*link_open)(bt_mesh_prov_bearer_t bearer)              | 回调函数，通知节点连接已开启                           |
| void (*link_close)(bt_mesh_prov_bearer_t bearer)             | 回调函数，通知节点连接已关闭                           |
| void (*complete)(u16_t net_idx, u16_t addr)                  | 回调函数，通知节点配网成功                             |
| void (*reset)(void)                                          | 回调函数，通知节点配网信息重置成功，节点可以重新入网。 |

| bt_mesh_prov_oob_info_t（枚举）定义      |                  |
| ---------------------------------------- | ---------------- |
| BT_MESH_PROV_OOB_NONE   =  0x00          | 无               |
| BT_MESH_PROV_OOB_OTHER  =   BIT(0)       | 其他             |
| BT_MESH_PROV_OOB_URI   =   BIT(1)        | URI              |
| BT_MESH_PROV_OOB_2D_CODE   =   BIT(2)    | 二维码           |
| BT_MESH_PROV_OOB_BAR_CODE  =   BIT(3)    | 条形码           |
| BT_MESH_PROV_OOB_NFC   =   BIT(4)        | NFC              |
| BT_MESH_PROV_OOB_NUMBER   =   BIT(5)     | 数字             |
| BT_MESH_PROV_OOB_STRING   =   BIT(6)     | 字符串           |
| BT_MESH_PROV_OOB_ON_BOX   =   BIT(11)    | 位于包装盒外     |
| BT_MESH_PROV_OOB_IN_BOX  =   BIT(12)     | 位于包装盒内     |
| BT_MESH_PROV_OOB_ON_PAPER  =   BIT(13)   | 位于纸上         |
| BT_MESH_PROV_OOB_IN_MANUAL   =   BIT(14) | 位于盒内说明书上 |
| BT_MESH_PROV_OOB_ON_DEV   =   BIT(15)    | 位于设备上       |

| bt_mesh_output_action_t（枚举）定义  |            |
| ------------------------------------ | ---------- |
| BT_MESH_NO_OUTPUT  =  0x00           | 无         |
| BT_MESH_BLINK    =   BIT(0)          | 闪烁       |
| BT_MESH_BEEP    =   BIT(1)           | 蜂鸣       |
| BT_MESH_VIBRATE    =   BIT(2)        | 马达振动   |
| BT_MESH_DISPLAY_NUMBER    =   BIT(3) | 显示数字   |
| BT_MESH_DISPLAY_STRING    =   BIT(4) | 显示字符串 |

| bt_mesh_input_action_t（枚举）定义 |            |
| ---------------------------------- | ---------- |
| BT_MESH_NO_INPUT  =  0x00          | 无         |
| BT_MESH_PUSH    =   BIT(0)         | 按压       |
| BT_MESH_TWIST    =   BIT(1)        | 旋转       |
| BT_MESH_ENTER_NUMBER    =   BIT(2) | 输入数字   |
| BT_MESH_ENTER_STRING    =   BIT(3) | 输入字符串 |

| struct bt_mesh_comp（结构体）定义 |                                  |
| --------------------------------- | -------------------------------- |
| uint16_t cid                      | Company ID, 由 Bluetooth SIG分配 |
| uint16_t pid                      | 产品标识符                       |
| uint16_t vid                      | 版本标识符                       |
| size_t elem_count                 | 节点元素个数                     |
| bt_mesh_elem *elem                | 元素列表                         |

| bt_mesh_elem（结构体）定义        |                                                            |
| --------------------------------- | ---------------------------------------------------------- |
| u16_t addr                        | 入网成功后分配的unicast 地址                               |
| u16_t grop_addr                   | 组地址                                                     |
| u16_t loc                         | Location Descriptor (GATT Bluetooth Namespace Descriptors) |
| u8_t model_count                  | 元素中SIG Model的个数                                      |
| u8_t vnd_model_count              | 元素中Vendor Model的个数                                   |
| struct bt_mesh_model * models     | SIG Model数组，可使用BT_MESH_MODEL宏进行定义               |
| struct bt_mesh_model * vnd_models | Vendor Model数组，可使用BT_MESH_MODEL_VND宏进行定义        |

| bt_mesh_model（结构体）定义                                  |                                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| union {<br/>		u16_t id;<br/>		struct {<br/>			u16_t company;<br/>			u16_t id;<br/>		} vnd;<br/>	}; | SIG Model ID，占用1-2个字节；<br /> Vendor Model ID，占用2个字节。 |
| u8_t  elem_idx                                               | 记录该Model所归属的节点元素的索引值                          |
| u8_t  mod_idx                                                | 记录该Model的索引值                                          |
| u16_t flags                                                  | 记录该Model的状态                                            |
| struct bt_mesh_model_pub * pub                               | Publication的配置参数，无需支持消息发布时，可设置为NULL      |
| u16_t keys[CONFIG_BT_MESH_MODEL_KEY_COUNT]                   | APP Key列表，记录该Model绑定的APP key列表                    |
| u16_t groups[CONFIG_BT_MESH_MODEL_GROUP_COUNT]               | 订阅列表，记录该Model订阅的组地址和虚拟地址                  |
| void *user_data                                              | 该Model关联的用户数据指针                                    |

| struct bt_mesh_model_pub（结构体）定义                       |                                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| struct bt_mesh_model *mod                                    | 该消息发布配置所归属的Model指针                              |
| u16_t addr                                                   | 发布的地址                                                   |
| u16_t key                                                    | APP Key索引                                                  |
| u8_t  ttl                                                    | 消息的生存时间                                               |
| u8_t  retransmit                                             | 重传计数                                                     |
| u8_t  period                                                 | 发布周期                                                     |
| u8_t  period_div:4,   <br />         cred:1,<br />         fast_period:1<br />         count:3 | 分频系数<br />FriendShip建立凭证<br />使能快速分频<br />剩余重传次数 |
| u32_t period_start                                           | 当前周期的起始时间                                           |
| struct net_buf_simple *msg                                   | 发布的消息结构体指针                                         |
| int (*update)(struct bt_mesh_model *mod)                     | 回调函数，可实现周期性的状态更新；无需支持周期性发布时，可设置为NULL |
| struct k_delayed_work timer                                  | 定时器，用来实现周期性的发布                                 |

| bt_mesh_model_op（结构体）定义                               |                                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| u32_t  opcode                                                | 消息操作类型，使用BT_MESH_MODEL_OP_*宏进行定义<br />使用BT_MESH_MODEL_OP_1宏进行定义时，占用1个字节；<br />使用BT_MESH_MODEL_OP_2宏进行定义时，占用2个字节；<br />使用BT_MESH_MODEL_OP_3宏进行定义时，包含Company ID，占用4个字节。 |
| size_t min_len                                               | 消息的最小长度                                               |
| void (*func)(struct bt_mesh_model *model, <br /> struct bt_mesh_msg_ctx *ctx,  <br />struct net_buf_simple *buf) | 消息处理回调函数<br />ctx为发送消息的上下文（参见bt_mesh_msg_ctx），<br />buf为需要发送的消息 |
| void (*func2)(struct bt_mesh_model *model,<br/>			   struct bt_mesh_msg_ctx *ctx,<br/>			   struct net_buf_simple *buf, u32_t opcode) | 消息处理回调函数<br />ctx为发送消息的上下文（参见bt_mesh_msg_ctx），<br />buf为需要发送的消息 |

| bt_mesh_msg_ctx（结构体）定义 |                                          |
| ----------------------------- | ---------------------------------------- |
| u16_t net_idx                 | 所属MESH网络的Network Key 索引           |
| u16_t app_idx                 | 所属MESH网络的APP Key 索引               |
| u16_t addr                    | 远端地址                                 |
| u16_t recv_dst                | 接收消息的目的地址，发送消息时该值无效。 |
| u8_t  recv_ttl:7              | 接收消息的TTL值                          |
| u8_t  send_rel:1              | 通过段确认发送可靠                       |
| u8_t  send_ttl                | 发送消息的TTL值                          |

| bt_mesh_provisioner（结构体）定义                            |                                                              |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| const u8_t *prov_uuid                                        | 配网器的UUID                                                 |
| const u16_t prov_unicast_addr                                | 配网器Primary Element的地址                                  |
| u16_t prov_start_address                                     | 可分配的单播地址的起始地址                                   |
| u8_t  prov_attention                                         | 配网邀请阶段的提示定时器                                     |
| u8_t  prov_algorithm                                         | 配网算法                                                     |
| u8_t  prov_pub_key_oob                                       | 配网器的公钥OOB信息                                          |
| int  (*prov_pub_key_oob_cb)(u8_t remote_pub_key[64])         | 回调函数，用于读取设备的公钥OOB信息。<br />返回值： 0 - 成功； 非0  -  失败 |
| u8_t *prov_static_oob_val                                    | 配网器的静态OOB信息                                          |
| u8_t  prov_static_oob_len                                    | 配网器的静态OOB信息长度                                      |
| int  (*prov_input_num)(bt_mesh_output_action_t act, u8_t size) | 回调函数，用于输入OOB数字信息<br />act：设备的输出行为<br />size：设备的OOB信息长度 |
| int  (*prov_output_num)(bt_mesh_input_action_t act, u8_t size) | 回调函数，用于输出的OOB数字信息<br />act：设备的输入行为<br />size：设备的OOB信息长度 |
| u8_t  flags                                                  | 密钥更新和IV更新的标志位<br />BIT0：密钥更新标记<br />0：False   1：True<br />BIT1：IV更新标记<br />0: Normal Operation   1: IV Update active<br />BIT2 ~ BIT7:  RFU |
| u32_t iv_index                                               | 所属MESH网络的IV索引                                         |
| void (*prov_link_open)(bt_mesh_prov_bearer_t bearer)         | 回调函数，通知配置连接打开事件                               |
| void (*prov_link_close)(bt_mesh_prov_bearer_t bearer, u8_t reason) | 回调函数，通知配置连接关闭事件                               |
| void (*prov_complete)(<br />int node_idx，<br />const u8_t device_uuid[16]，u16_t unicast_addr，<br />u8_t element_num，<br />u16_t netkey_idx，<br />bool gatt_flag) | 回调函数，通知配网器当前节点配网成功，且该节点已经被正确分配了Network key索引和首要元素地址。<br />node_idx：在已配置节点队列中的节点索引。<br />device_uuid：已配置节点的uuid。<br />unicast_addr：已配置节点的单播地址。 |

- 返回值

| 返回值 |            |
| ------ | ---------- |
| 0      | 初始化成功 |
| 非0    | 初始化失败 |

- 注意事项

  MESH协议栈初始化成功后，需要调用bt_mesh_prov_enable（）函数，开启Unprovisioning Device Beacon广播

### **bt_mesh_reset**

- 函数原型

```c
void bt_mesh_reset(void)
```

- 功能描述

将节点移除出当前的MESH网络

- 参数描述

  无

- 返回值

  无

- 注意事项

  节点被移除出当前MESH网络后，可以通过调用bt_mesh_prov_enable（）函数，重新开启入网请求广播

### **bt_mesh_suspend**

- 函数原型

```c
int bt_mesh_suspend(void)
```

- 功能描述

暂停该节点在mesh网络中的功能

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  节点长时间进入暂停状态后，配网器将认为该节点永久掉线。

### **bt_mesh_resume**

- 函数原型

```c
int bt_mesh_resume(void)
```

- 功能描述

恢复暂停节点的功能

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  在调用bt_mesh_suspend()后，可使用bt_mesh_resume()函数，恢复节点的网络功能

### **bt_mesh_provision**

- 函数原型

```c
int bt_mesh_provision(const u8_t net_key[16], u16_t net_idx,
		      u8_t flags, u32_t iv_index, u16_t addr,
		      const u8_t dev_key[16])
```

- 功能描述

​      MESH device 收到prov data 后，保存net key 、unicast addr 、devkey等信息

- 参数描述

| IN/OUT | NAME                   | DESC                |
| ------ | ---------------------- | ------------------- |
| [in]   | const u8_t net_key[16] | Network key         |
| [in]   | u16_t net_idx          | Network key索引     |
| [in]   | u8_t flags             | 配置标志位          |
| [in]   | u32_t iv_index         | IV 索引             |
| [in]   | u16_t addr             | Primary Element地址 |
| [in]   | const u8_t dev_key[16] | 设备秘钥            |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  本接口用于获取到provisioner 发送的prov data 后,device 对相关信息进行保存

### **bt_mesh_is_provisioned**

- 函数原型

```c
bool bt_mesh_is_provisioned(void)
```

- 功能描述

获取节点配网状态

- 参数描述

  无

- 返回值

| 返回值 |            |
| ------ | ---------- |
| 0      | 未配网     |
| 1      | 已成功配网 |

- 注意事项

  无

### **bt_mesh_iv_update**

- 函数原型

```c
int bool bt_mesh_iv_update(void)
```

- 功能描述

更新网络中的IV值

- 参数描述

  无

- 返回值

| 返回值 |                |
| ------ | -------------- |
| 0      | 开启IV更新失败 |
| 1      | 开启IV更新成功 |

- 注意事项

  无

### **bt_mesh_lpn_set**

- 函数原型

```c
int bt_mesh_lpn_set(bool enable)
```

- 功能描述

开启或关闭低功耗特性

- 参数描述

| IN/OUT | NAME        | DESC           |
| ------ | ----------- | -------------- |
| [in]   | bool enable | 是否打开低功耗 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  该函数是一个具有实时性的函数，可以随时打开/关闭节点的低功耗特性，使用前节点需已完成prov

### **bt_mesh_lpn_poll**

- 函数原型

```c
int bt_mesh_lpn_poll(void)
```

- 功能描述

发送一个friend poll请求消息

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  当节点未建立Friend关系时，返回失败

### bt_mesh_lpn_set_cb

- 函数原型

```c
void bt_mesh_lpn_set_cb(void (*cb)(u16_t friend_addr, bool established))
```

- 功能描述

  回调函数注册，用于通知Friendship的改变，如建立或丢失

- 参数描述

| IN/OUT | NAME                                            | DESC           |
| ------ | ----------------------------------------------- | -------------- |
| [in]   | void (*cb)(u16_t friend_addr, bool established) | 注册的回调函数 |

- 返回值

  无

- 注意事项

  无

### bt_mesh_provisioner_enable

- 函数原型

```c
int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers)
```

- 功能描述

开启配网器功能

- 参数描述

| IN/OUT | NAME                          | DESC         |
| ------ | ----------------------------- | ------------ |
| [in]   | bt_mesh_prov_bearer_t bearers | 广播承载类型 |

| bt_mesh_prov_bearer_t（枚举）定义 |          |
| --------------------------------- | -------- |
| BT_MESH_PROV_ADV   =  BIT(0)      | ADV承载  |
| BT_MESH_PROV_GATT  = BIT(1)       | GATT承载 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用前需调用bt_mesh_init()进行协议栈初始化

### bt_mesh_provisioner_disable

- 函数原型

```c
int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers)
```

- 功能描述

  关闭配网器功能

- 参数描述

| IN/OUT | NAME                          | DESC         |
| ------ | ----------------------------- | ------------ |
| [in]   | bt_mesh_prov_bearer_t bearers | 广播承载类型 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### bt_mesh_input_string

- 函数原型

```c
int bt_mesh_input_string(const char *str)
```

- 功能描述

配置OOB字符串信息

- 参数描述

| IN/OUT | NAME            | DESC      |
| ------ | --------------- | --------- |
| [in]   | const char *str | OOB字符串 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

   prov 阶段使用，在收到配网器的OOB字符串输入请求时，可通过此函数传入设置的OOB信息

### bt_mesh_input_number

- 函数原型

```c
int bt_mesh_input_string(u32_t num)
```

- 功能描述

配置OOB数字信息

- 参数描述

| IN/OUT | NAME      | DESC    |
| ------ | --------- | ------- |
| [in]   | u32_t num | OOB数字 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  prov 阶段使用，在收到配网器的OOB数字输入请求时，可通过此函数传入设置的OOB信息

### bt_mesh_prov_enable

- 函数原型

```c
int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers)
```

- 功能描述

开启配网功能，等待配网

- 参数描述

| IN/OUT | NAME                          | DESC         |
| ------ | ----------------------------- | ------------ |
| [in]   | bt_mesh_prov_bearer_t bearers | 广播承载类型 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### bt_mesh_prov_disable

- 函数原型

```c
int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers)
```

- 功能描述

关闭配网功能

- 参数描述

| IN/OUT | NAME                          | DESC         |
| ------ | ----------------------------- | ------------ |
| [in]   | bt_mesh_prov_bearer_t bearers | 广播承载类型 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### bt_mesh_model_send

- 函数原型

```c
int bt_mesh_model_send(struct bt_mesh_model *model,
		       struct bt_mesh_msg_ctx *ctx,
		       struct net_buf_simple *msg,
		       const struct bt_mesh_send_cb *cb,
		       void *cb_data)
```

- 功能描述

消息发送

- 参数描述

| IN/OUT | NAME                             | DESC                                   |
| ------ | -------------------------------- | -------------------------------------- |
| [in]   | struct bt_mesh_model *model      | 发送消息的Model指针                    |
| [in]   | struct bt_mesh_msg_ctx *ctx      | 消息的上下文信息，包括                 |
| [in]   | struct net_buf_simple *msg       | 消息结构体指针                         |
| [in]   | const struct bt_mesh_send_cb *cb | 回调函数，通知消息发送的开始和结束事件 |
| [in]   | void *cb_data                    | 回调函数的数据指针                     |

| struct bt_mesh_send_cb（结构体）定义                  |                                |
| ----------------------------------------------------- | ------------------------------ |
| void (*start)(u16_t duration, int err, void *cb_data) | 回调函数，通知消息发送开始事件 |
| void (*end)(int err, void *cb_data)                   | 回调函数，通知消息发送结束事件 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  调用该接口前，节点应完成prov,并获取appkey

### bt_mesh_model_publish

- 函数原型

```c
int bt_mesh_model_publish(struct bt_mesh_model *model)
```

- 功能描述

消息发布，消息将被发送到配置的组地址或者虚拟地址中

- 参数描述

| IN/OUT | NAME                        | DESC            |
| ------ | --------------------------- | --------------- |
| [in]   | struct bt_mesh_model *model | 发送消息的Model |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  在调用该函数之前，应该确保Model中发布的消息（bt_mesh_model_pub.msg）包含一个正确的信息。此API只是用来发布非周期性的消息。如果需要发送周期性消息，只需要保证当bt_mesh_model_pub.update函数回调时，bt_mesh_model_pub.msg有合理的值。

### bt_mesh_model_elem

- 函数原型

```c
struct bt_mesh_elem *bt_mesh_model_elem(struct bt_mesh_model *mod);
```

- 功能描述

​       获取Model所在elem指针

- 参数描述

| IN/OUT | NAME                        | DESC            |
| ------ | --------------------------- | --------------- |
| [in]   | struct bt_mesh_model *model | 查询的Model指针 |

- 返回值

  返回Model的元素列表指针

- 注意事项

  无

