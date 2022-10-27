<table>
<tr><td>文档版本</td><td>v1.1</td></tr>
<tr><td>发布日期</td><td>2021-05-14</td></tr>
</table>
<table height=200>
<tr></tr>
</table>
<div align=center><font size=6>BLE MESH API</font>
<table height=550>
<tr></tr>
</table>
<img src="../../../../assets/zh-cn/cover/occ_icon.png" width=30% align=center position=fixed bottom=0/>
<div STYLE="page-break-after: always;"></div>






<div align=left>

<div align=center><font size=6>目录</font>

[TOC]


<div align=left>

<div STYLE="page-break-after: always;"></div>

## BLE MESH API

# 1. 概述

本文主要介绍Mesh协议栈和Mesh组件API。

Mesh协议栈部分将分章节介绍

- Access层API
- Config Model API
- Health Model API
- Provisioner API

Mesh组件部分将分章节介绍

- Mesh Node组件API
- Mesh Models组件API
- Mesh Provisioner组件API

# 2. MESH协议栈API

## 2.1 Access API

本章节介绍了BLE Mesh Access接口，利用这些接口可以实现蓝牙MESH协议栈初始化、特性配置、消息发送等功能。

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

      MESH device 收到prov data 后，保存net key 、unicast addr 、devkey等信息

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

      获取Model所在elem指针

- 参数描述

| IN/OUT | NAME                        | DESC            |
| ------ | --------------------------- | --------------- |
| [in]   | struct bt_mesh_model *model | 查询的Model指针 |

- 返回值

  返回Model的元素列表指针

- 注意事项

  无

## 2.2. Config Model API

本章节介绍了蓝牙Mesh Config Model接口，利用这些接口可以实现蓝牙MESH的配置信息的设置和读取。

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

      获取节点数据包的默认TTL值

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

      设置对端设备网络数据包默认TTL值

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

      获取对端节点friend属性状态

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

      设置对端节点friend状态

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

      获取对端节点代理属性状态

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

      设置对端节点代理属性状态

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

      获取对端节点中继属性状态

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

      增加一个netkey

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

      绑定一个SIG Model的APP Key

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

      绑定一个Vendor Model的APP Key

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

      获取SIG Model的发布配置参数

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

      获取Vendor Model的发布配置参数

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

      删除SIG Model消息订阅地址

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

      删除vendor Model的消息订阅地址

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

      擦除SIG Model所有订阅非虚拟地址，并增加sub_addr订阅地址

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

      擦除Vendor Model所有订阅非虚拟地址，并增加新的订阅地址sub_addr

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

      针对SIG Model增加一个虚拟地址的消息订阅

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

      针对Vendor Model增加一个虚拟地址的消息订阅

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

      取消一个SIG Model的虚拟地址消息订阅

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

      取消一个Vendor Model的虚拟地址消息订阅

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

      重写一个SIG Model的虚拟地址消息订阅

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

      设置Heartbeat消息发布参数

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

      设置Heartbeat消息订阅参数

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

      获取Heartbeat消息订阅参数

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

### bt_mesh_cfg_ctrl_relay_set

- 函数原型

```c
int bt_mesh_cfg_ctrl_relay_set(u16_t net_idx, u16_t addr,
							   const struct ctrl_relay_param *cr, u8_t *status)
```

- 功能描述

      设置Controlled Relay配置参数

- 参数描述

| IN/OUT | NAME                           | DESC                                                                   |
| ------ | ------------------------------ | ---------------------------------------------------------------------- |
| [in]   | u16_t net_idx                  | Network Key索引                                                         |
| [in]   | u16_t addr                     | 节点的unicast地址                                                        |
| [in]   | struct ctrl_relay_param *cr    | Controlled Relay配置参数结构体指针，参见struct ctrl_relay_param（结构体）定义 |
| [out]  | u8_t *status                   | 返回状态； 0： 设置成功  非0：设置失败                                       |

| struct ctrl_relay_param（结构体）定义 |                            |
| ----------------------------------- | ----------------------------------------------------------------  |
| u8_t enable                         | Controlled Relay功能是否打开，1表示打开，0表示关闭                      |
| u8_t trd_n                          | 阀值N，表示周围有N个Relay才会关闭自身的Relay                            |
| u8_t rssi                           | 信号强度，检测到周围邻居Relay的信号强度大于此值时，才会将该邻居Relay计数加1   |
| u8_t sta_period                     | 发送周期，表示节点发出Relay状态消息的周期，单位秒                         |
| u8_t chk_period                     | 决策周期，节点决策的是否关闭此Relay功能的周期，单位秒                      |
| u8_t req_period                     | 主动扫描请求周期，节点多长时间没有收到邻居的状态消息，就主动发出扫描请求，单位秒 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model

### bt_mesh_cfg_ctrl_relay_get

- 函数原型

```c
int bt_mesh_cfg_ctrl_relay_get(u16_t net_idx, u16_t addr,
							   struct ctrl_relay_param *cr, u8_t *status)
```

- 功能描述

      获取Controlled Relay配置参数

- 参数描述

| IN/OUT | NAME                           | DESC                                                                   |
| ------ | ------------------------------ | ---------------------------------------------------------------------- |
| [in]   | u16_t net_idx                  | Network Key索引                                                         |
| [in]   | u16_t addr                     | 节点的unicast地址                                                        |
| [in]   | struct ctrl_relay_param *cr    | Controlled Relay配置参数结构体指针，参见struct ctrl_relay_param（结构体）定义 |
| [out]  | u8_t *status                   | 返回状态； 0： 设置成功  非0：设置失败                                       |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  使用该接口的前提是当前节点支持cfg cli model，对端设备则支持cfg srv model



## 2.3. Health Model API

本章节介绍了BLE Mesh Health Model接口，利用这些接口可以实现蓝牙MESH节点健康状态的操作。

### bt_mesh_health_cli_set

- 函数原型

```c
int bt_mesh_health_cli_set(struct bt_mesh_model *model)
```

- 功能描述

      设置节点为Health Model Client

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

      获取对端节点注册的错误状态

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

      清除注册的错误状态

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

      错误测试

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

      获取Health Model消息发布的分频系数值

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

  获取Health Model的Attention定时器状态

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

  获取本health cli超时时间

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

      设置本helath cli超时时间

- 参数描述

| IN/OUT | NAME          | DESC     |
| ------ | ------------- | -------- |
| [in]   | s32_t timeout | 超时时长 |

- 返回值

  无

- 注意事项

  本节点需支持health cli model

## 2.4. Provisioner API

本章节介绍BLE Mesh Provisioner接口，利用这些接口可以实现蓝牙MESH的Provision功能。

### bt_mesh_provisioner_store_node_info

- 函数原型

```c
int bt_mesh_provisioner_store_node_info(struct bt_mesh_node_t *node_info)
```

- 功能描述

      存储被prov devices节点信息

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

  获取配网器所有节点中Primary Element的单播地址

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

      provisioner设置节点名称

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

      获取索引值对应的节点的名称

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

      根据节点名称获取对应的索引值

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

      通过节点地址获取节点信息

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

      获取MESH网络中Network Key的数量

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

      获取MESH网络中APP Key的数量

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

  添加配网器的APP Key

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

      根据Network Key索引和APP Key索引查找APP Key值

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

      配网器添加一个netkey

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

      根据netkey索引获得netkey的值

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

      根据输入的netkey索引，删除netkey

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

  获取配网器的单播地址和元素数量

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

      配网器本地model绑定appkey

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

      打印配网器的所有元素信息，如：cid、pid、vid、元素数量以及元素内的所有信息

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

      打印入网节点的信息，如节点名称、UUID、MAC地址、MAC地址类型、单播地址、元素数量、Network Key索引等

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

      配网功能是否使能

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

      将未入网设备信息加入未入网设备列表

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

      从队列中删除设备，重置当前的配置信息和节点信息

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

      在设备入网前，比较设备UUID的信息

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

      定义了一个回调函数，当配网器收到未入网设备的adv包，且该设备不在provisioner的未配置设备队列中

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

      用来通知应用层收到了mesh组网过程中的adv包或者未入网设备发送的beacon包（设备不在未入网设备列表中）

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

      设置配置信息中的Network Key索引或者IV索引

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

      OOB信息输入

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

      OOB信息输出

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

# 3. MESH组件API

## 3.1 Mesh Models API

Mesh Models组件中实现了SIG定义的Generic Onoff Model,Generic Level Model, Light Lightness Model，Light CTL Model，开发者可以方便的组合各个Model，实现不同的功能。

### ble_mesh_model_init

- 函数原型

```c
int ble_mesh_model_init(const struct bt_mesh_comp *comp)
```

- 功能描述

​       MESH Models 组件初始化

- 参数描述

| IN/OUT | NAME                            | DESC                                                         |
| ------ | ------------------------------- | ------------------------------------------------------------ |
| [in]   | const struct bt_mesh_comp *comp | MESH model 组件初始化结构体，参见struct bt_mesh_comp（结构体）定义 |

- 返回值

    | 返回值 |      |
    | ------ | ---- |
    | 0      | 成功 |
    | < 0    | 失败 |
  
- 注意事项

  无

  


### ble_mesh_model_get_comp_data

- 函数原型

  ```c
  const struct bt_mesh_comp * ble_mesh_model_get_comp_data()
  ```

- 功能描述

    获取节点的Composition data，包括节点信息、Models的组成等

- 参数描述

    | IN/OUT | NAME                        | DESC                                                         |
    | ------ | --------------------------- | ------------------------------------------------------------ |
    | [out]  | const struct bt_mesh_comp * | MESH model 组件初始化结构体，参见struct bt_mesh_comp（结构体）定义 |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 非NULL | 成功 |
  | NULL   | 失败 |

- 注意事项
    无

### ble_mesh_model_set_cb

- 函数原型
  
    ```c
    int ble_mesh_model_set_cb(model_event_cb event_cb)
    ```
    
- 功能描述

    设置MESH Models事件回调函数

- 参数描述

    | IN/OUT | NAME                    | DESC                                                       |
    | ------ | ----------------------- | ---------------------------------------------------------- |
    | [in]   | model_event_cb event_cb | model event 回调函数，参见model_event_cb回调函数定义返回值 |

    -返回值

    | 返回值 |      |
    | ------ | ---- |
    | 0      | 成功 |
    | < 0    | 失败 |



- 回调函数

    ```c
    void (*model_event_cb)(mesh_model_event_en event, void *p_arg)
    ```

- 参数描述

    | IN/OUT | NAME                      | DESC                                                         |
    | ------ | ------------------------- | ------------------------------------------------------------ |
    | [in]   | mesh_model_event_en event | model model 事件，参见mesh_model_event_en(枚举)定义          |
    | [in]   | void *p_arg               | 事件相关数据，具体数据类型参见mesh_provisioner_event_en（枚举）定义 |

- 返回值
  
    无
    
    | mesh_model_event_en(枚举)定义                      |                                     |                                                              |
    | -------------------------------------------------- | ----------------------------------- | ------------------------------------------------------------ |
    | BT_MESH_MODEL_CFG_APP_KEY_ADD   = 0x00             | appkey 设置消息                     | p_arg对应appkey_status结构体，参见该结构体定义               |
    | BT_MESH_MODEL_CFG_COMP_DATA_STATUS = 0x02          | comp data 消息                      | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_HEARTBEAT_PUB_STATUS = 0x06      | heartbeat pub 设置消息              | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_APPKEY_STATUS = 0x8003           | appkey设置状态消息                  | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_BEACON_STATUS = 0x800b           | beacon设置状态消息                  | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_TTL_STATUS = 0x800e              | TTL设置状态消息                     | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_FRIEND_STATUS = 0x8011           | FRIEND设置状态消息                  | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_PROXY_STATUS = 0x8014            | PROXY设置状态消息                   | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_NET_KRP_STATUS = 0x8017          | KRP设置状态消息                     | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_PUB_STATUS = 0x8019              | PUB设置状态消息                     | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_SUB_STATUS = 0x801f              | SUB设置状态结果                     | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_SUB_LIST   = 0x802a              | SUB LIST状态消息                    | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_SUB_LIST_VND = 0x802c            | SUB VND状态消息                     | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_RELAY_STATUS = 0x8028            | RELAY设置状态消息                   | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_HEARTBEAT_SUB_STATUS = 0x803c    | heartbeat sub设置状态消息           | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_APPKEY_BIND_STATUS = 0x803e      | appkey bind设置状态消息             | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_RST_STATUS     = 0x804a          | node rst消息                        | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_CFG_NET_KEY_STATUS = 0x8044          | netkey 设置状态消息                 | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_ONOFF_SET =        0x8202            | onoff设置消息                       | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的status_data表示 |
    | BT_MESH_MODEL_ONOFF_STATUS =     0x8204            | onoff设置状态消息                   | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LEVEL_SET =        0x8206            | level设置消息                       | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LEVEL_MOVE_SET =   0x820B            | level设置状态消息                   | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LEVEL_STATUS =     0x8208            | level设置状态消息                   | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LEVEL_DELTA_SET=   0x8209            | level delta设置状态消息             | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_SET=     0x824C            | lightness设置消息                   | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_STATUS     = 0x824E        | lightness设置状态消息               | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_LINEAR_SET = 0x8250        | lightness linear设置消息            | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_LINEAR_STATUS = 0x8252     | lightness linear设置状态消息        | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_LAST_STATUS   =0x8254      | lightness last状态消息              | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_DEF_STATUS    =0x8256      | lightess设置状态消息                | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_RANGE_STATUS  = 0x8258     | lightness range设置状态消息         | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_DEF_SET  = 0x8259          | lightness default 设置消息          | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHTNESS_RANGE_SET  = 0x825B        | lightness range设置消息             | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_SET           =0x825E      | light ctl 设置消息                  | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_STATUS        =0x8260      | light ctl 设置状态消息              | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_TEMP_RANGE_STATUS  =0x8263 | light temperature range设置状态消息 | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_TEMP_SET      =0x8264      | light temperature设置消息           | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_TEMP_STATUS   =0x8266      | light temperature设置状态消息       | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_DEF_STATUS    =0x8268      | light ctl default设置状态消息       | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_DEF_SET       =0x8269      | light ctl default设置消息           | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_LIGHT_CTL_RANGE_SET     =0x826B      | light ctl range设置消息             | p_arg对应model_message结构体，参见该结构体定义，状态数据使用其中的user_data表示 |
    | BT_MESH_MODEL_VENDOR_MESSAGES          =0xcf       | 透传消息                            | p_arg对应model_message结构体，参见该结构体定义，数据使用其中的ven_data表示 |
    | BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG   = 0xD6      | 入网自动配置消息                    | p_arg对应model_message结构体，参见该结构体定义，数据使用其中的ven_data表示 |
    | BT_MESH_MODEL_VENDOR_MESH_AUTOCONFIG_STATUS =0xD8  | 入网自动配置状态消息                | p_arg对应model_message结构体，参见该结构体定义，数据使用其中的ven_data表示 |
    
    | appkey_status（结构体）定义 |                       |
    | --------------------------- | --------------------- |
    | uint8_t status              | appkey添加状态        |
    | uint16_t netkey_idx         | appkey 对应netkey idx |
    | uint16_t appkey_idx         | appkey idx            |
    
    | model_message（结构体）定义        |                                                              |
    | ---------------------------------- | ------------------------------------------------------------ |
    | uint16_t source_addr               | 数据源地址                                                   |
    | struct net_buf_simple *status_data | 标准model状态数据，用于mesh基础model ,cfg model、health model |
    | void *user_data                    | sig model状态数据,用于sig model如generic model、light model等 |
    | vendor_data ven_data               | 厂家自定义数据，参见vendor_data结构体定义                    |
    
    | vendor_data（结构体）定义 |                        |
    | ------------------------- | ---------------------- |
    | void *user_data           | 厂家自定义数据数据内容 |
    | uint16_t data_len         | 厂家自定义数据数据长度 |



| S_ELEM_STATE（结构体）定义 |                                                   |
| -------------------------- | ------------------------------------------------- |
| S_MESH_STATE state         | sig model 状态，参见S_MESH_STATE结构体定义        |
| S_MESH_POWERUP powerup     | sig moldel 默认状态，参见S_MESH_POWERUP结构体定义 |



| S_MESH_STATE（结构体）定义       |                                                          |
| -------------------------------- | -------------------------------------------------------- |
| u8_t onoff[TYPE_NUM]             | onoff srv status,其中TYPE_NUM参见TYPE_NUM枚举定义        |
| s16_t level[TYPE_NUM]            | level srv status,其中TYPE_NUM参见TYPE_NUM枚举定义        |
| u16_t lightness_linear[TYPE_NUM] | lightness linear status,其中TYPE_NUM参见TYPE_NUM枚举定义 |
| u16_t lightness_actual[TYPE_NUM] | lightness actual status,其中TYPE_NUM参见TYPE_NUM枚举定义 |
| s16_t level[TYPE_NUM]            | level status,其中TYPE_NUM参见TYPE_NUM枚举定义            |
| u16_t lightness_linear[TYPE_NUM] | lightness linear status,其中TYPE_NUM参见TYPE_NUM枚举定义 |
| u16_t lightness[TYPE_NUM]        | ctl lightness status,其中TYPE_NUM参见TYPE_NUM枚举定义    |
| u16_t temp[TYPE_NUM]             | ctl temperature status,其中TYPE_NUM参见TYPE_NUM枚举定义  |
| u16_t UV[TYPE_NUM]               | ctl UV status,其中TYPE_NUM参见TYPE_NUM枚举定义           |

| TYPE_NUM（枚举）定义 |          |
| -------------------- | -------- |
| T_CUR = 0            | 当前状态 |
| T_TAR                | 目标状态 |
| TYPE_NUM             | TYPE种类 |

| S_MESH_POWERUP (结构体)定义   |                                                   |
| ----------------------------- | ------------------------------------------------- |
| uint16_t lightness_actual_def | lightness actual 默认值                           |
| u16_t lightness_last          | lightness 上次值                                  |
| RANGE_STATUS lightness_range  | lightness 范围值，参见RANGE_STATUS结构体定义      |
| uint16_t lightness_default    | ctl lightness 默认值                              |
| uint16_t temp_default         | ctl temperature 默认值                            |
| uint16_t UV_default           | ctl UV 默认值                                     |
| RANGE_STATUS ctl_temp_range   | ctl temperature范围值，参见RANGE_STATUS结构体定义 |

| RANGE_STATUS（结构体）定义 |                                                 |
| -------------------------- | ----------------------------------------------- |
| _STATUS_CODES  code        | range 设置结果状态码，参见_STATUS_CODES枚举定义 |
| u16_t range_min            | range 最小值                                    |
| u16_t range_max            | range 最大值                                    |

| _STATUS_CODES（枚举）定义 |                |
| ------------------------- | -------------- |
| SUCCESS = 0               | 设置成功       |
| SET_MIN_FAIL              | 设置最小值失败 |
| SET_MAX_FAIL              | 设置最大值失败 |
| RFU                       | 保留           |



### ble_mesh_model_find

- 函数原型

    ```c
    struct bt_mesh_model *ble_mesh_model_find(uint16_t elem_idx, uint16_t mod_idx, uint16_t CID)
    ```

- 功能描述

    获取指定Model结构体指针

- 参数描述

    | IN/OUT | NAME                   |                                           |
    | ------ | ---------------------- | ----------------------------------------- |
    | [in]   | elem_idx               | elem id 索引                              |
    | [in]   | uint16_t mod_id        | model id标识,目前支持的model id如下表所示 |
    | [in]   | uint16_t CID           | vendor model company id 标识              |
    | [out]  | struct bt_mesh_model * | mesh model，参见bt_mesh_model(结构体)定义 |

    

    | model id      | 说明   |
    | :------------ | ------ |
    | cfg srv       | 0x0000 |
    | cfg cli       | 0x0001 |
    | health srv    | 0x0002 |
    | health cli    | 0x0003 |
    | onoff srv     | 0x1000 |
    | onoff cli     | 0x1001 |
    | level srv     | 0x1002 |
    | level cli     | 0x1003 |
    | lightness srv | 0x1300 |
    | lightness cli | 0x1302 |
    | ctl srv       | 0x1303 |
    | ctl cli       | 0x1305 |

    

- 返回值

    | 返回值 |      |
    | ------ | ---- |
    | NULL   | 失败 |
    | 非NULL | 成功 |

- 注意事项
    无

### ble_mesh_model_status_get

- 函数原型

  ```c
  int ble_mesh_model_status_get(uint16_t netkey_idx, uint16_t appkey_idx, uint16_t unicast_addr,struct bt_mesh_model *model,uint16_t op_code)
  ```

- 功能描述

  获取指定Model的状态值

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |
  | [in]   | uint16_t op_code            | status get 操作码         |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  查询相应状态所对应的model 及opcode对应如下表所示
  
  ## model 状态查询表
  
  | status                   | query model id | query opcode |
  | ------------------------ | -------------- | ------------ |
  | friend status            | 0x0001         | 0x800f       |
  | proxy status             | 0x0001         | 0x8012       |
  | relay status             | 0x0001         | 0x8026       |
  | onoff status             | 0x1001         | 0x8201       |
  | level status             | 0x1003         | 0x8205       |
  | lightness status         | 0x1302         | 0x824b       |
  | lightness linear status  | 0x1302         | 0x824f       |
  | lightness last status    | 0x1302         | 0x8253       |
  | lightness default status | 0x1302         | 0x8255       |
  | lightness range status   | 0x1302         | 0x8257       |
  | ctl status               | 0x1305         | 0x825D       |
  | ctl temperature status   | 0x1305         | 0x8261       |
  | ctl default status       | 0x1305         | 0x8267       |
  | ctl range status         | 0x1305         | 0x8262       |
  
  
### ble_mesh_generic_onoff_get

- 函数原型
  
  ```c
  int ble_mesh_generic_onoff_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```
  
- 功能描述

  获取Generic OnOff Server Model的Status信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  无

### ble_mesh_generic_onoff_set

- 函数原型

  ```c
  int ble_mesh_generic_onoff_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_onoff_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Generic OnOff Server Model的Status信息

- 参数描述

  | IN/OUT | NAME                        |                                                     |
  | ------ | --------------------------- | --------------------------------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                         |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                         |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                    |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                           |
  | [in]   | set_onoff_arg *send_arg     | 发送参数设置，参见set_onoff_arg *send_arg结构体定义 |
| [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                     |
  
| set_onoff_arg（结构体）定义 |                |
| --------------------------- | -------------- |
| uint8_t onoff               | onff 设置值    |
| uint8_t tid                 | 数据TID        |
| uint8_t send_trans          | 是否发送trans  |
| uint8_t trans               | trans 值       |
| uint8_t delay               | 状态转换延时值 |

返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| < 0    | 失败 |

- 注意事项
  无
### ble_mesh_generic_onoff_cli_publish

  - 函数原型
  
    ```c
    int ble_mesh_generic_onoff_cli_publish(struct bt_mesh_model *model, set_onoff_arg *send_arg, bool ack)
    ```
  
  - 功能描述
  
    设置Generic OnOff Server Model的Status信息
  
  - 参数描述
  
    | IN/OUT | NAME                        |                                                     |
    | ------ | --------------------------- | --------------------------------------------------- |
    | [in]   | struct bt_mesh_model *model | 发送status get使用的model                           |
    | [in]   | set_onoff_arg *send_arg     | 发送参数设置，参见set_onoff_arg *send_arg结构体定义 |
    | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                     |
    
    返回值
  
    | 返回值 |      |
    | ------ | ---- |
    | 0      | 成功 |
    | < 0    | 失败 |
    
  - 注意事项
    无

  

### ble_mesh_generic_level_get

- 函数原型

  ```c
  int ble_mesh_generic_level_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Generic Level Server Model的Status信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  无

### ble_mesh_generic_level_set

- 函数原型

  ```c
  int ble_mesh_generic_level_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Generic Level Server Model的 level Status信息

- 参数描述

  | IN/OUT | NAME                        |                                                     |
  | ------ | --------------------------- | --------------------------------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                         |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                         |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                    |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                           |
  | [in]   | set_level_arg *send_arg     | 发送参数设置，参见set_level_arg *send_arg结构体定义 |
| [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                     |
  
| set_level_arg（结构体）定义 |                |
| --------------------------- | -------------- |
| uint16_t level              | level 设置值   |
| uint16_t def                | level默认值    |
| uint16_t move               | level move值   |
| uint8_t tid                 | level tid 值   |
| uint8_t send_trans          | 是否发送trans  |
| uint8_t trans               | trans 值       |
| uint8_t delay               | 状态转换延时值 |
| s32_t delta                 | level delta值  |

返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| < 0    | 失败 |

- 注意事项
  无

### ble_mesh_generic_level_move_set

- 函数原型

  ```c
  int ble_mesh_generic_level_move_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Generic Level Server Model的level Move信息

- 参数描述

  | IN/OUT | NAME                        |                                                     |
  | ------ | --------------------------- | --------------------------------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                         |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                         |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                    |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                           |
  | [in]   | set_level_arg *send_arg     | 发送参数设置，参见set_level_arg *send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                     |

  返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |
  
- 注意事项
  无

### ble_mesh_generic_level_delta_set

- 函数原型

  ```c
  int ble_mesh_generic_level_delta_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_level_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Generic Level Server Model的Delta信息

- 参数描述

  | IN/OUT | NAME                        |                                                     |
  | ------ | --------------------------- | --------------------------------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                         |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                         |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                    |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                           |
  | [in]   | set_level_arg *send_arg     | 发送参数设置，参见set_level_arg *send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                     |

  返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |
  
- 注意事项

### ble_mesh_light_lightness_get

- 函数原型

  ```c
  int ble_mesh_light_lightness_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light Lightness Server Model的Status信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  无

### ble_mesh_light_lightness_set

- 函数原型

  ```c
  int ble_mesh_light_lightness_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_lightness_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light Lightness Server Model的Status信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_lightness_arg*send_arg  | 发送参数设置，参见set_lightness_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

  | set_lightness_arg（结构体）定义 |                    |
| ------------------------------- | ------------------ |
  | uint16_t lightness              | lightness设置值    |
  | uint16_t lightness_linear       | lightness linear值 |
  | uint16_t def                    | lightness默认值    |
  | uint16_t range_min              | lightness 最小值   |
  | uint16_t range_max              | lightness最大值    |
  | uint8_t tid                     | lightness tid 值   |
  | uint8_t send_trans              | 是否发送trans      |
  | uint8_t trans                   | trans 值           |
  | uint8_t delay                   | 状态转换延时值     |
  
  返回值

  | 返回值 |      |
| ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |
  
- 注意事项
  无

### ble_mesh_light_lightness_linear_get

- 函数原型

  ```c
  int ble_mesh_light_lightness_linear_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light Lightness Server Model的Lightness Linear信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  无

### ble_mesh_light_lightness_linear_set

- 函数原型

  ```c
  int ble_mesh_light_lightness_linear_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_lightness_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light Lightness Server Model的Lightness Linear信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_lightness_arg*send_arg  | 发送参数设置，参见set_lightness_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  无

### ble_mesh_light_lightness_def_get

- 函数原型

  ```c
  int ble_mesh_light_lightness_def_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light Lightness Server Model的Lightness Default信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  无

### ble_mesh_light_lightness_def_set

- 函数原型

  ```c
  int ble_mesh_light_lightness_def_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_lightness_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light Lightness Server Model的Lightness Default信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_lightness_arg*send_arg  | 发送参数设置，参见set_lightness_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

### ble_mesh_light_lightness_range_get

- 函数原型

  ```c
  int ble_mesh_light_lightness_range_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light Lightness Server Model的Lightness Range信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  无

### ble_mesh_light_lightness_range_set

- 函数原型

  ```c
  int ble_mesh_light_lightness_range_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_lightness_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light Lightness Server Model的Lightness Range信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_lightness_arg*send_arg  | 发送参数设置，参见set_lightness_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

### ble_mesh_light_lightness_last_get

- 函数原型

  ```c
  int ble_mesh_light_lightness_last_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light Lightness Server Model的Lightness Last信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  无
  
### ble_mesh_light_ctl_get

- 函数原型
  
  ```c
  int ble_mesh_light_ctl_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```
  
- 功能描述

  获取Light CTL Server Model的Status信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  无

### ble_mesh_light_ctl_set

- 函数原型

  ```c
  int ble_mesh_light_ctl_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light CTL Server Model的Status信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_light_ctl_arg*send_arg  | 发送参数设置，参见set_light_ctl_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

  | set_lightness_arg（结构体）定义 |                       |
  | ------------------------------- | --------------------- |
  | uint16_t lightness              | lightness设置值       |
  | uint16_t tempature              | light tempature值     |
  | uint16_t delta_uv               | light delta_uv默认值  |
  | uint16_t range_min              | light tempature最小值 |
  | uint16_t range_max              | light tempature最大值 |
  | uint8_t tid                     | lightness tid 值      |
  | uint8_t send_trans              | 是否发送trans         |
  | uint8_t trans                   | trans 值              |
  | uint8_t delay                   | 状态转换延时值        |

  

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  ### ble_mesh_light_ctl_temp_get

- 函数原型

  ```c
  int ble_mesh_light_ctl_temp_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light CTL Server Model的Temperature信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项



### ble_mesh_light_ctl_temp_set

- 函数原型

  ```c
  int ble_mesh_light_ctl_temp_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model,set_light_ctl_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light CTL Server Model的Temperature信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_light_ctl_arg*send_arg  | 发送参数设置，参见set_light_ctl_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

  

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  ### ble_mesh_light_ctl_def_get

- 函数原型

  ```c
  int ble_mesh_light_ctl_def_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light CTL Server Model的Default信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项



### ble_mesh_light_ctl_def_set

- 函数原型

  ```c
  int ble_mesh_light_ctl_def_set(struct bt_mesh_model *model, uint16_t unicast_addr,uint16_t netkey_idx, uint16_t appkey_idx, set_light_ctl_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light CTL Server Model的Default信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_light_ctl_arg*send_arg  | 发送参数设置，参见set_light_ctl_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

  

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  ### ble_mesh_light_ctl_temp_range_get

- 函数原型

  ```c
  int ble_mesh_light_ctl_temp_range_get(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model)
  ```

- 功能描述

  获取Light CTL Server Model的Temperature Range信息

- 参数描述

  | IN/OUT | NAME                        |                           |
  | ------ | --------------------------- | ------------------------- |
  | [in]   | uint16_t netkey_idx         | netkey 索引               |
  | [in]   | uint16_t appkey_idx         | appkey 索引               |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr          |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项



### ble_mesh_light_ctl_temp_range_set

- 函数原型

  ```c
  int ble_mesh_light_ctl_temp_range_set(uint16_t netkey_idx, uint16_t appkey_idx,uint16_t unicast_addr,struct bt_mesh_model *model, set_light_ctl_arg *send_arg, bool ack)
  ```

- 功能描述

  设置Light CTL Server Model的Temperature Range信息

- 参数描述

  | IN/OUT | NAME                        |                                                        |
  | ------ | --------------------------- | ------------------------------------------------------ |
  | [in]   | uint16_t netkey_idx         | netkey 索引                                            |
  | [in]   | uint16_t appkey_idx         | appkey 索引                                            |
  | [in]   | uint16_t unicast_addr       | 目标unicast_addr                                       |
  | [in]   | struct bt_mesh_model *model | 发送status get使用的model                              |
  | [in]   | set_light_ctl_arg*send_arg  | 发送参数设置，参见set_light_ctl_arg*send_arg结构体定义 |
  | [in]   | bool ack                    | 是否应答，0（不应答）/1（应答）                        |

  

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  无

### ble_mesh_vendor_cli_model_msg_send

- 函数原型

  ```c
  int ble_mesh_vendor_cli_model_msg_send(vnd_model_msg *model_msg)
  ```

- 功能描述

  Vendor Client Model的消息发送接口

- 参数描述

  | IN/OUT | NAME                     |                                               |
  | ------ | ------------------------ | --------------------------------------------- |
  | [in]   | vnd_model_msg *model_msg | 发送vnd msg参数,参见vnd_model_msg(结构体)定义 |

  | vnd_model_msg（结构体）定义 |                                                 |
  | --------------------------- | ----------------------------------------------- |
  | struct bt_mesh_model *model | 发包使用的model,参见bt_mesh_model（结构体）定义 |
  | uint16_t netkey_idx         | 发包使用的netkeyid                              |
  | uint16_t appkey_idx         | 发包使用的appkey id                             |
  | uint16_t dst_addr           | 发包的目标地址                                  |
  | uint16_t len                | 发包的长度                                      |
  | uint8_t  retry              | 发包重试次数                                    |
  | uint8_t  retry_period       | 发包重试周期                                    |
  | uint8_t  opid               | 发包使用的opcode                                |
  | uint8_t  tid                | 发包使用的tid                                   |
  | uint8_t  *data              | 发包数据                                        |

  

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  无

### ble_mesh_vendor_srv_model_msg_send

- 函数原型

  ```c
  int ble_mesh_vendor_srv_model_msg_send(vnd_model_msg *model_msg)
  ```

- 功能描述

  Vendor Server Model的消息发送接口

- 参数描述

  | IN/OUT | NAME                     |                                               |
  | ------ | ------------------------ | --------------------------------------------- |
  | [in]   | vnd_model_msg *model_msg | 发送vnd msg参数,参见vnd_model_msg(结构体)定义 |

  

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项

  无



## 3.2. Mesh Node API

Mesh Node组件是对Mesh节点的一层抽象定义，使用Mesh Node组件开发者可以方便的实现节点的配置和开发。

### ble_mesh_node_init

- 函数原型

```c
int ble_mesh_node_init(node_config_t *param)
```

- 功能描述

​       mesh NODE 组件初始化

- 参数描述

| IN/OUT | NAME                 | DESC                                                         |
| ------ | -------------------- | ------------------------------------------------------------ |
| [in]   | node_config_t *param | MESH NODE 组件初始化结构体，参见struct node_config_t（结构体）定义 |

| struct node_config_t（结构体）定义       |                                                              |
| ---------------------------------------- | ------------------------------------------------------------ |
| node_role_en role                        | NODE 节点角色，定义参见node_role_en（枚举）定义              |
| provisioner_node *provisioner_config     | provisioner node 配置，见provisioner_node结构体定义          |
| uint8_t dev_uuid[16]                     | NODE节点设备UUID                                             |
| uint8_t dev_name[DEVICE_NAME_MAX_LENGTH] | NODE节点设备名称,DEVICE_NAME_MAX_LENGTH为28                  |
| model_event_cb user_model_cb             | NODE节点MODEL消息回调函数，定义参见 MESH MODEL MODULE 文档model_event_cb（函数指针定义） |
| prov_event_cb user_prov_cb               | NODE节点入网过程回调函数，定义参见prov_event_cb（函数指针）定义 |
| health_srv_cb *health_cb                 | health srv 回调函数，见health_srv_cb结构体定义               |
| oob_method node_oob                      | 节点OOB信息，定义参见oob_method（结构体）定义                |

 

| health_srv_cb（结构体）定义 |                                                              |
| --------------------------- | ------------------------------------------------------------ |
| attn_cb att_on              | health srv attention on 回调函数，见attn_cb（回调函数定义）  |
| attn_cb att_off             | health srv attention off 回调函数，见attn_cb（回调函数定义） |

回调函数原型

```c
void (*attn_cb)(void)
```

- 功能描述

​       用于node节点配网过程中attention信息回调

- 参数描述

  无

- 返回值

  无

- 注意事项

  只有provisioner attention time设置为非零值，node节点才会在配网过程中回调该函数



| provisioner_node（结构体）定义 |                                                    |
| ------------------------------ | -------------------------------------------------- |
| provisioner_config_t config    | provisioner 配置，见provisioner_config_t结构体定义 |
| uint16_t local_sub             | provisioner node 本地订阅地址                      |
| uint16_t local_pub             | provisioner node 本地发布地址                      |

| node_role_en（枚举）定义 |                            |
| ------------------------ | -------------------------- |
| PROVISIONER              | 使能为PROVISIONER NODE节点 |
| NODE                     | 使能为普通NODE节点         |

| struct oob_method(结构体)定义 |                                                        |
| ----------------------------- | ------------------------------------------------------ |
| uint8_t* static_oob_data      | NODE节点static oob 信息                                |
| oob_action_en input_action    | NODE节点 input OOB方法，参见oob_action_en（枚举）定义  |
| uint8_t input_max_size        | NODE节点INPUT OOB最大长度                              |
| oob_action_en output_action   | NODE节点 output OOB方法，参见oob_action_en（枚举）定义 |
| uint8_t output_max_size       | NODE节点OUTPUT OOB最大长度                             |

| oob_action_en(枚举)定义 |            |
| ----------------------- | ---------- |
| ACTION_NULL             | 无OOB      |
| ACTION_NUM              | OOB NUM    |
| ACTION_STR              | OOB STRING |

回调函数原型

```c
void (*prov_event_cb)(mesh_prov_event_en event, void *p_arg)
```

- 功能描述

​       用于MODEL层向上层APP上报 prov过程相关事件

- 参数描述

| IN/OUT | NAME               | DESC                                                         |
| ------ | ------------------ | ------------------------------------------------------------ |
| [in]   | mesh_prov_event_en | MESH PROV 事件，参见mesh_prov_event_en（枚举）定义           |
| [in]   | void *p_arg        | 事件相关数据，具体数据类型参见mesh_prov_event_en（枚举）定义 |

| struct mesh_prov_event_en（枚举）定义 |                  |                                                              |
| ------------------------------------- | ---------------- | ------------------------------------------------------------ |
| BT_MESH_EVENT_NODE_REST               | NODE  REST消息   | NULL                                                         |
| BT_MESH_EVENT_NODE_PROV_COMP          | NODE设备入网成功 | mesh_node_local_t* node，参见mesh_node_local_t（结构体）定义 |
| BT_MESH_EVENT_NODE_OOB_INPUT_NUM      | NODE设备输入NUM  | uint8_t size,输如num 个数                                    |
| BT_MESH_EVENT_NODE_OOB_INPUT_STRING   | NODE设备输入STR  | uint8_t size,输入str 个数                                    |

- 返回值

  无

- 注意事项

  若需将NODE节点使能为PROVISIONER节点，需先使用MESH PROVISIONER Module相关API，具体使用方法请参考该API说明文档

  


### ble_mesh_node_OOB_input_num

- 函数原型

  ```c
  int ble_mesh_node_OOB_input_num(uint32_t num)
  ```

- 功能描述

  NODE节点 输入OOB num

- 参数描述

  | IN/OUT | NAME         | DESC         |
  | ------ | ------------ | ------------ |
  | [in]   | uint32_t num | oob num 个数 |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  用户在接收到BT_MESH_EVENT_NODE_OOB_INPUT_NUM事件后，需调用该接口出入指定数量的num，其它时间调用该接口无效

### ble_mesh_node_OOB_input_string

- 函数原型

  ```c
  int ble_mesh_node_OOB_input_string(const char *str)
  ```

- 功能描述

  provisioner 输入OOB string

- 参数描述

  | IN/OUT | NAME            | DESC           |
  | ------ | --------------- | -------------- |
  | [in]   | const char *str | oob string字符 |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  用户在接收到BT_MESH_EVENT_NODE_OOB_INPUT_STRING事件后，需调用该接口出入指定数量的string，其它时间调用该接口无效

### ble_mesh_node_appkey_add

  - 函数原型

    ```c
    int ble_mesh_model_appkey_add(uint16_t netkey_idx,uint16_t appkey_idx,uint16_t unicast_addr)
    ```

  - 功能描述

    给对端节点添加appkey

  - 参数描述

    | IN/OUT | NAME                  |                  |
    | ------ | --------------------- | ---------------- |
    | [in]   | uint16_t netkey_idx   | netkey 索引      |
    | [in]   | uint16_t appkey_idx   | appkey 索引      |
    | [in]   | uint16_t unicast_addr | 目标unicast_addr |

  - 返回值

    | 返回值 |      |
    | ------ | ---- |
    | 0      | 成功 |
    | < 0    | 失败 |

  - 注意事项
    该接口只适合provisioner节点使用

### ble_mesh_node_model_autoconfig

  - 函数原型

    ```c
    int ble_mesh_node_model_autoconfig(uint16_t netkey_idx,uint16_t appkey_idx, uint16_t unicast_addr,model_auto_config_t config)
    ```

  - 功能描述

    给node节点发送model自动配置消息

  - 参数描述

    | IN/OUT | NAME                       |                                           |
    | ------ | -------------------------- | ----------------------------------------- |
    | [in]   | uint16_t netkey_idx        | 发送消息使用的netkey 索引                 |
    | [in]   | uint16_t appkey_idx        | 发送消息使用的appkey 索引                 |
    | [in]   | uint16_t unicast_addr      | 目标unicast_addr                          |
    | [in]   | model_auto_config_t config | 自动配置信息，参见model_auto_config_t定义 |

| model_auto_config_t（结构体）定义 |                  |
| --------------------------------- | ---------------- |
| uint16_t sub_addr                 | 自动绑定sub addr |

 

  - 返回值

    | 返回值 |      |
    | ------ | ---- |
    | 0      | 成功 |
    | < 0    | 失败 |

  - 注意事项
    无

## 3.3. Mesh Provisioner API

Mesh Provisioner组件是对Provisioner节点的抽象定义，该组件实现了未入网设备的发现，入网，配置和管理功能。

### ble_mesh_provisioner_init

- 函数原型

```c
int ble_mesh_provisioner_init(provisioner_config_t *param)
```

- 功能描述

​       mesh provisioner 组件初始化

- 参数描述

| IN/OUT | NAME                        | DESC                                                         |
| ------ | --------------------------- | ------------------------------------------------------------ |
| [in]   | provisioner_config_t *param | MESH Provisioner 组件初始化结构体，参见struct provisioner_config_t（结构体）定义 |

| struct provisioner_config_t（结构体）定义 |                                         |
| ----------------------------------------- | --------------------------------------- |
| uint16_t unicast_addr_local               | Provisioner 节点本地地址                |
| uint16_t unicast_addr_start               | Provisioner节点分配给NODE节点的起始地址 |
| uint8_t attention_time                    | Provisioner配网过程attention时间        |
| provisioner_cb cb                         | Provisioner节点回调函数，定义如下       |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | =0     | 成功 |
  | 非0    | 失败 |

- 注意事项

  注意unicast_addr_start - unicast_addr_local的值应大于provisioner node节点本地elem 数目

  

- 回调函数原型

```c
void (*provisioner_cb)(mesh_provisioner_event_en event, void *p_arg)
```

- 功能描述

​       用于向上层应用上报PROVISIONER 事件

- 参数描述

| IN/OUT | NAME                      | DESC                                                         |
| ------ | ------------------------- | ------------------------------------------------------------ |
| [in]   | mesh_provisioner_event_en | MESH Provisioner 事件，参见mesh_provisioner_event_en（枚举）定义 |
| [in]   | void *p_arg               | 事件相关数据，具体数据类型参见mesh_provisioner_event_en（枚举）定义 |

| struct mesh_provisioner_event_en（枚举）定义 |                                      |                                                  |
| -------------------------------------------- | ------------------------------------ | ------------------------------------------------ |
| BT_MESH_EVENT_RECV_UNPROV_DEV_ADV            | Provisioner 节点接收到未入网设备广播 | mesh_node_t* node，参见mesh_node_t（结构体）定义 |
| BT_MESH_EVENT_PROV_COMP                      | Provisioner节点入网设备成功          | mesh_node_t* node，参见mesh_node_t（结构体）定义 |
| BT_MESH_EVENT_FOUND_DEV_TIMEOUT              | Provisioner节点查找设备超时          | NULL                                             |
| BT_MESH_EVENT_PROV_FAILED                    | Provisioner节点入网设备失败          | uint8_t* reason,入网失败原因                     |
| BT_MESH_EVENT_OOB_INPUT_NUM                  | Provisioner节点输入OOB NUM提示       | uint8_t* size,输入数字个数                       |
| BT_MESH_EVENT_OOB_INPUT_STRING               | Provisioner节点输入OOB STR提示       | uint8_t* szie,输入字符个数                       |
| BT_MESH_EVENT_OOB_INPUT_STATIC_OOB           | Provisioner节点输入 STATIC OOB 提示  | NULL（static oob默认输入16个字符）               |

| struct mesh_node_t 结构体定义 |                        |
| ----------------------------- | ---------------------- |
| uint8_t  uuid[16]             | 节点UUID               |
| uint8_t  dev_addr[6]          | 节点MAC地址            |
| uint8_t  addr_type            | 节点MAC地址类型        |
| uint16_t prim_unicast         | 节点首要element 地址   |
| uint16_t oob_info             | 节点oob信息            |
| uint8_t  element_num          | 节点elem数量           |
| uint8_t  bearer               | 节点入网 bearer        |
| uint8_t flags                 | 节点key更新/iv更新标志 |
| uint32_t iv_index             | 节点iv 索引            |
| uint8_t* node_name            | 节点名称               |

- 返回值

  无

- 注意事项

  无

### ble_mesh_provisioner_enable

- 函数原型

```c
int ble_mesh_provisioner_enable()
```

- 功能描述

​     节点provisioner使能

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### ble_mesh_provisioner_disable

- 函数原型

```c
int ble_mesh_provisioner_disable()
```

- 功能描述

​       provisioner节点禁用

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无

### ble_mesh_provisioner_dev_filter

- 函数原型

```c
int ble_mesh_provisioner_dev_filter(uint8_t enable, uuid_filter_t *filter)
```

- 功能描述

​       provisioners设备上报过滤

- 参数描述

| IN/OUT | NAME                  | DESC                                                     |
| ------ | --------------------- | -------------------------------------------------------- |
| [in]   | enable                | 关闭 0/开启1                                             |
| [in]   | uuid_filter_t *filter | uuid_filter_t过滤规则结构体，参见uuid_filter_t结构体定义 |

| struct  uuid_filter_t(结构体)定义 |                    |
| --------------------------------- | ------------------ |
| uint8_t *uuid                     | 过滤uuid信息头指针 |
| uint8_t uuid_length               | 过滤uuid信息长度   |
| uint8_t filter_start              | uuid开始匹配位置   |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | 非0    | 失败 |

- 注意事项

  使用该接口，provisioner将从上报的UUID filter_start 处开始与过滤器输入的长度为uuid_length的uuid信息进行匹配，若匹配成功，则上报，否则不上报该信息。

  ### ble_mesh_provisioner_show_dev

- 函数原型

  ```c
  int ble_mesh_provisioner_show_dev(uint8_t enable, uint32_t timeout)
  ```

- 功能描述

  ​       设备上报使能

- 参数描述

  | IN/OUT | NAME             | DESC                                |
  | ------ | ---------------- | ----------------------------------- |
  | [in]   | uint8_t enable   | 设备上报使能，0（关闭）/1（开启）   |
  | [in]   | uint32_t timeout | 设备上报超时，单位s,为0时默认无超时 |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | >= 0   | 成功 |
  | < 0    | 失败 |

- 注意事项

  使用前需初始化和使能provisioner功能

  

### ble_mesh_provisioner_dev_add

- 函数原型

```c
int ble_mesh_provisioner_dev_add(mesh_node_t *node,uint8_t auto_add_appkey)
```

- 功能描述

​       添加待入网NODE 设备

- 参数描述

| IN/OUT | NAME               | DESC                                                         |
| ------ | ------------------ | ------------------------------------------------------------ |
| [in]   | mesh_node_t *node  | mesh_node_t *node入网节点信息，参见struct mesh_node_t *node定义 |
| [in]   | uint8_t add_appkey | 是否添加appkey，0（添加）/1（不添加）                        |

- 返回值

| 返回值 |      |
| ------ | ---- |
| >= 0   | 成功 |
| < 0    | 失败 |

- 注意事项

  使用前需初始化和使能provisioner功能

### ble_mesh_provisioner_get_add_appkey_flag

- 函数原型

  ```c
  int ble_mesh_provisioner_get_add_appkey_flag(u16_t unicast_addr)
  ```

- 功能描述

  ​       获取已入网节点是否需要添加appkey标志

- 参数描述

  | IN/OUT | NAME         | DESC                  |
  | ------ | ------------ | --------------------- |
  | [in]   | unicast_addr | node 节点unicast 地址 |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | >= 0   | 成功 |
  | < 0    | 失败 |

- 注意事项

  若使用MESH NODE 组件开发，则用户一般不需要使用该接口

### ble_mesh_provisioner_dev_del

- 函数原型

  ```c
  int ble_mesh_provisioner_dev_del(uint8_t addr[6], uint8_t addr_type, uint8_t uuid[16])
  ```

- 功能描述

  删除添加的待入网设备/正在入网设备节点

- 参数描述

  | IN/OUT | NAME              | DESC                                |
  | ------ | ----------------- | ----------------------------------- |
  | [in]   | uint8_t addr[6]   | 设备mac地址                         |
  | [in]   | uint8_t addr_type | 设备地址类型，0（public）/1(random) |
  | [in]   | uint8_t uuid[16]  | 设备uuid                            |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| < 0    | 失败 |

- 注意事项
  使用前需初始化和使能provisioner功能

### ble_mesh_provisioner_OOB_input_num

- 函数原型

  ```c
  int ble_mesh_provisioner_OOB_input_num(uint32_t num)
  ```

- 功能描述

  provisioner 输入OOB num

- 参数描述

  | IN/OUT | NAME         | DESC         |
  | ------ | ------------ | ------------ |
  | [in]   | uint32_t num | oob num 个数 |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  用户在接收到BT_MESH_EVENT_OOB_INPUT_NUM事件后，需调用该接口出入指定数量的num，其它时间调用该接口无效

### ble_mesh_provisioner_OOB_input_string

- 函数原型

  ```c
  int ble_mesh_provisioner_OOB_input_string(const char *str)
  ```

- 功能描述

  provisioner 输入OOB string

- 参数描述

  | IN/OUT | NAME            | DESC           |
  | ------ | --------------- | -------------- |
  | [in]   | const char *str | oob string字符 |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  用户在接收到BT_MESH_EVENT_OOB_INPUT_STRING事件后，需调用该接口出入指定数量的string，其它时间调用该接口无效

### ble_mesh_provisioner_static_OOB_set

- 函数原型

```c
    int ble_mesh_provisioner_static_OOB_set(const uint8_t *oob, uint16_t oob_size)
```

- 功能描述

  provisioner 输入static OOB 

- 参数描述

  | IN/OUT | NAME               | DESC     |
  | ------ | ------------------ | -------- |
  | [in]   | const uint8_t *oob | oob 指针 |
  | [in]   | uint16_t oob_size  | oob长度  |

- 返回值

  | 返回值 |      |
  | ------ | ---- |
  | 0      | 成功 |
  | < 0    | 失败 |

- 注意事项
  用户在接收到BT_MESH_EVENT_OOB_INPUT_STATIC_OOB事件后，需调用该接口出入指定数量的oob 字符，其它时间调用该接口无效

### ble_mesh_provisioner_get_node_info

- 函数原型

  ```c
  mesh_node_t * ble_mesh_provisioner_get_node_info(u16_t unicast_addr)
  ```

- 功能描述

  provisioner 获取本身NODE节点以及入网NODE节点信息

- 参数描述

  | IN/OUT | NAME               | DESC                 |
  | ------ | ------------------ | -------------------- |
  | [in]   | u16_t unicast_addr | NODE节点unicast_addr |

- 返回值

  | 返回值 |                                         |
  | ------ | --------------------------------------- |
  | 非NULL | 成功，返回参数参见mesh_node_t结构体定义 |
  | < 0    | 失败                                    |

- 注意事项
  用户输入的unicast_addr地址范围若是属于Provisioner本身地址，则返回Provisioner NODE节点信息，否则返回已入网设备节点地址信息

### ble_mesh_provisioner_get_provisioner_data

- 函数原型

  ```c
  const provisioner_comp *ble_mesh_provisioner_get_provisioner_data()
  ```

- 功能描述

  获取 provisioner comp

- 参数描述

  无

- 返回值

  | 返回值 |                                                   |
  | ------ | ------------------------------------------------- |
  | 非NULL | provisioner comp ，参见provisioner_comp结构体定义 |
  | NULL   | 失败                                              |

  | struct provisioner_comp(结构体)定义 |                                 |
  | ----------------------------------- | ------------------------------- |
  | const struct bt_mesh_provisioner *  | mesh provisioner 结构体         |
  | uint16_t unicast_addr_local         | mesh provisioner本地起始地址    |
  | uint16_t local_sub                  | mesh provisioner本地默认sub地址 |
  | uint16_t local_pub                  | mesh provisioner本地默认pub地址 |

- 注意事项
  若使用MESH NODE 组件开发，则用户一般不需要使用该接口

- 