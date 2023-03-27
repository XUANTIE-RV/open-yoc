# 概述
该经典蓝牙协议栈API主要包含，GAP模块，A2DP模块，AVRCP模块、HFP模块功能。
# BT API表格
## GAP接口
| 名称 | 参数 | 说明 |
| --- | --- | --- |
| bt_stack_register_callback | bt_stack_cb_t callback | GAP 事件回调 |
| bt_stack_init | void | 蓝牙协议栈初始化 |
| bt_stack_set_connectable | int enable | 设置设备可被连接属性 |
| bt_stack_set_discoverable | int enable | 设置设备可被连接属性 |
| bt_stack_set_name | const char* name | 设置设备名称 |
| bt_stack_set_eir | uint8_t fec_required,  bt_eir_data_t* eir_data,  size_t eri_num | 设置设备EIR |
| bt_stack_set_cod | bt_stack_cod_t cod | 设置设备COD |
| bt_stack_get_cod | bt_stack_cod_t *cod | 获取设备COD |
| bt_stack_remove_bond_device | bt_dev_addr_t *peer_addr | 解绑配对设备 |
| bt_stack_paired_dev_foreach | void (*func)(bt_dev_addr_t*addr, void*data), void *data | 遍历配对设备 |
| bt_stack_set_io_capability | bt_stack_io_cap_t io_cap | 设置IO能力 |
| bt_stack_pin_entry | bt_dev_addr_t*peer_addr, constchar*pin | 设备配对，输入pincode |
| bt_stack_passkey_entry | bt_dev_addr_t*peer_addr, uint32_tpasskey | 设备配对，输入passkey |
| bt_stack_passkey_confirm | bt_dev_addr_t*peer_addr | 设备配对，确认passkey |
| bt_stack_pairing_cancel | bt_dev_addr_t*peer_addr | 设备配对，取消配对 |

## A2DP接口
| 名称 | 参数 | 说明 |
| --- | --- | --- |
| bt_prf_a2dp_register_callback | bt_prf_a2dp_cb_t callback | A2DP 事件回调 |
| bt_prf_a2dp_sink_register_data_callback | bt_prf_a2dp_sink_data_cb_t callback | A2DP 数据回调 |
| bt_prf_a2dp_sink_init | void | A2DP SINK 模块初始化 |
| bt_prf_a2dp_sink_connect | bt_dev_addr_t *peer_addr | A2DP 连接 |
| bt_prf_a2dp_sink_disconnect | bt_dev_addr_t *peer_addr | A2DP 断连接 |

## AVRCP 接口
| 名称 | 参数 | 说明 |
| --- | --- | --- |
| bt_prf_avrcp_ct_register_callback | bt_prf_avrcp_ct_cb_t callback | AVRCP CT 事件回调 |
| bt_prf_avrcp_ct_init | void | AVRCP CT 模块初始化 |
| bt_prf_avrcp_ct_cmd_register_notification | bt_prf_avrcp_rn_event_ids_tevent_id | 发送注册通知命令 |
| bt_prf_avrcp_ct_cmd_get_rn_capabilities | void | CT 获取TG能力 |
| bt_prf_avrcp_ct_cmd_passthrough | bt_prf_avrcp_op_id_t   operation_i
bt_prf_avrcp_op_stat_t operation_state | 发送按键命令 |
| bt_prf_avrcp_ct_cmd_get_play_status | void | 发送获取TG播放器状态命令 |
| bt_prf_avrcp_ct_cmd_set_absolute_volume | uint8_t volume | 发送设置绝对音量命令 |
| bt_prf_avrcp_tg_register_callback | bt_prf_avrcp_tg_cb_tcallback | AVRCP TG 事件回调注册 |
| bt_prf_avrcp_tg_init | void | AVRCP TG 模块初始化 |
| bt_prf_avrcp_tg_set_rn_evt_cap | uint16_tcap_bit_mask | 设置TG 本地能力 |
| bt_prf_avrcp_tg_get_rn_evt_cap | void | 获取TG 本地能力 |
| bt_prf_avrcp_tg_send_rn_rsp | bt_prf_avrcp_rn_event_ids_t event_id, bt_prf_avrcp_ctype_response_t rsp,
bt_prf_avrcp_rn_param_t *param | TG 回复订阅通知 |
| bt_prf_avrcp_tg_notify_vol_changed | uint8_t volume | TG 回复音量变化事件通知，并修改系统音量 |

## HFP 接口
| 名称 | 参数 | 说明 |
| --- | --- | --- |
| bt_prf_hfp_hf_init | void | HFP HF 模块初始化 |
| bt_prf_hfp_hf_register_callback | bt_prf_hfp_hf_cb_t callback | HFP 事件回调 |
| bt_prf_hfp_hf_connect | bt_dev_addr_t *peer_addr | HFP 连接 |
| bt_prf_hfp_hf_disconnect | bt_dev_addr_t *peer_addr | HFP 断开连接 |
| bt_prf_hfp_hf_audio_connect | bt_dev_addr_t *peer_addr | HFP 音频连接 |
| bt_prf_hfp_hf_audio_disconnect | bt_dev_addr_t *peer_addr | HFP 音频断开连接 |
| bt_prf_hfp_hf_send_chld_cmd | bt_prf_hfp_hf_chld_type_t chld,
int idx | 挂起电话和三方通话（AT+CHLD） |
| bt_prf_hfp_hf_answer_call | void | 接听电话（ATA） |
| bt_prf_hfp_hf_reject_call | void | 拒接电话（AT+CHUP） |
| bt_prf_hfp_hf_dial | char *number | 拨打电话 |
| bt_prf_hfp_hf_vol_update | bt_prf_hfp_hf_vol_ctrl_target_t type, int volume | 设置AG端音量 |

# BT API 介绍
## GAP
### bt_stack_event_cb_t

- 函数原型

```c
typedef void (*bt_stack_event_cb_t)(bt_stack_cb_event_t event, bt_stack_cb_param_t *param);
```

- 功能描述

蓝牙协议栈事件回调函数声明

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_stack_cb_event_t event | 协议栈上报的事件，具体事件见下表 |
| [in] | bt_stack_cb_param_t *param | 协议栈上报事件参数 |

| bt_stack_cb_event_t event |  |
| --- | --- |
|     BT_STACK_AUTH_CMPL_EVT | 配对完成 |
|     BT_STACK_PIN_REQ_EVT | PINCODE 请求 |
|     BT_STACK_CFM_REQ_EVT | SSP 用户确认请求 |
|     BT_STACK_KEY_NOTIF_EVT | SSP passkey 通知 |
|     BT_STACK_KEY_REQ_EVT | SSP passkey 请求 |
|     BT_STACK_SET_EIR_DATA_EVT | EIR数据设置完成通知 |


- 返回值

无

-  注意事项
	无 
### bt_stack_register_callback

- 函数原型

```c
void bt_stack_register_callback(bt_stack_cb_t callback)
```

- 功能描述

蓝牙协议栈注册事件回调函数

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_stack_cb_t callback | 事件回调函数，bt_stack_event_cb_t 具体定义见下表 |

| bt_stack_cb_t 回调函数定义 |  |
| --- | --- |
| bt_stack_event_cb_t callback | 协议栈事件回调函数 |
| slist_t next | 链表下一个 |


- 返回值

无

-  注意事项
无 
### bt_stack_init

- 函数原型

```c
bt_stack_status_t bt_stack_init(void);
```

- 功能描述

蓝牙协议栈初始化

- 参数描述

无

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### 
### bt_stack_set_connectable

- 函数原型

```c
bt_stack_status_t bt_stack_set_connectable(int enable);
```

- 功能描述

设置经典蓝牙可连接模式

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | int enable | 是否可连接 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_set_discoverable

- 函数原型

```c
bt_stack_status_t bt_stack_set_discoverable(int enable);
```

- 功能描述

设置经典蓝牙可发现模式

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | int enable | 是否可发现 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_set_name

- 函数原型

```c
bt_stack_status_t bt_stack_set_name(const char *name);
```

- 功能描述

设置经典蓝牙设备名称

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | const char *name | 设备名称字符串 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_set_eir

- 函数原型

```c
bt_stack_status_t bt_stack_set_eir(uint8_t fec_required, bt_eir_data_t *eir_data, size_t eri_num);
```

- 功能描述

设置经典蓝牙扩展查询请求参数

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | uint8_t fec_required | 是否需要FEC，默认开启 |
| [in] | bt_eir_data_t *eir_data | eir 数据 |
| [in] | size_t eri_num | eir_data 数据长度 |

| bt_eir_data_t *eir_data |  |
| --- | --- |
|     uint8_t  type | eir 数据类型 |
|     uint8_t  len | eir 数据长度 |
|     uint8_t*data | eir 数据 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
协议栈模块初始化是，需要设置默认的EIR，设置设备名称时也需要更新EIR
### bt_stack_set_cod

- 函数原型

```c
bt_stack_status_t bt_stack_set_cod(bt_stack_cod_t cod);
```

- 功能描述

设置经典蓝牙设备类型

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_stack_cod_t cod | 设备类型 |

| bt_stack_eir_t *eir |  |
| --- | --- |
|    uint32_t      reserved_2: 2 | 保留 |
|    uint32_t      minor: 6 | 次要类型 |
|    uint32_t      major: 5 | 主要类型 |
|    uint32_t      service: 11 | 服务类型 |
|     uint32_t      reserved_8: 8 | 保留 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_get_cod

- 函数原型

```c
bt_stack_status_t bt_stack_get_cod(bt_stack_cod_t *cod);
```

- 功能描述

获取经典蓝牙设备类型

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [out] | bt_stack_cod_t *cod | class of device数据类型 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_dev_unpair

- 函数原型

```c
bt_stack_status_t bt_stack_dev_unpair(bt_dev_addr_t  *peer_addr);
```

- 功能描述

解绑蓝牙设备

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t  *peer_addr | 对端设备mac地址，如果为NULL，则解绑所有设备 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_paired_dev_foreach

- 函数原型

```c
bt_stack_status_t bt_stack_paired_dev_foreach(void (*func)(bt_dev_addr_t *addr, void *data), void *data);
```

- 功能描述

遍历绑定的对端设备

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | void (*func)(bt_dev_addr_t *addr, void *data) | 遍历回调函数 |
| [in] | void *data | 回调函数私有数据 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_set_io_capability

- 函数原型

```c
bt_stack_status_t bt_stack_set_io_capability(bt_stack_io_cap_t io_cap);
```

- 功能描述

设置经典蓝牙设备IO能力

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_stack_io_cap_t io_cap | IO能力，具体见下表 |

| bt_stack_io_cap_t io_cap |  |
| --- | --- |
| BT_STACK_IO_CAP_OUT | 只有显示 |
| BT_STACK_IO_CAP_IO | 显示是否 |
| BT_STACK_IO_CAP_IN | 只有键盘 |
| BT_STACK_IO_CAP_NONE | 没有输入输出，目前只需要支持该类型 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_pin_entry

- 函数原型

```c
bt_stack_status_t bt_stack_pin_entry(bt_dev_addr_t *peer_addr, const char *pin);
```

- 功能描述

设备配对过程中，输入pincode。上报 BT_STACK_PIN_REQ_EVT 事件时，需要调用该API。

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |
| [in] | const char *pin | 需要输入的pincode |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_passkey_entry

- 函数原型

```c
bt_stack_status_t bt_stack_passkey_entry(bt_dev_addr_t *peer_addr, uint32_t passkey);
```

- 功能描述

设备配对过程中，输入passkey code。上报 BT_STACK_KEY_REQ_EVT 事件时，需要调用该API。

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |
| [in] | uint32_t passkey | 需要输入的passkey |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 

### bt_stack_passkey_confirm

- 函数原型

```c
bt_stack_status_t bt_stack_passkey_confirm(bt_dev_addr_t *peer_addr);
```

- 功能描述

设备配对过程中，passkey 确认。上报 BT_STACK_CFM_REQ_EVT事件时，需要调用该API。

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_stack_pairing_cancel

- 函数原型

```c
bt_stack_status_t bt_stack_pairing_cancel(bt_dev_addr_t *peer_addr);
```

- 功能描述

设备配对过程中，取消配对。上报 BT_STACK_CFM_REQ_EVT，BT_STACK_KEY_REQ_EVT ，BT_STACK_PIN_REQ_EVT  事件后，可以调用该API。

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 

## A2DP
### bt_prf_a2dp_cb_t

- 函数原型

```c
typedef void (* bt_prf_a2dp_cb_t)(bt_prf_a2dp_cb_event_t event, bt_prf_a2dp_cb_param_t *param);
```

- 功能描述

A2DP模块回调函数申明

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_a2dp_cb_event_t event | 回调事件，具体事件见下表 |
| [in] | bt_prf_a2dp_cb_param_t *param | 回调参数 |

| bt_prf_a2dp_cb_event_t event |  |
| --- | --- |
| BT_PRF_A2DP_CONNECTION_STATE_EVT | 连接事件 |
| BT_PRF_A2DP_AUDIO_STATE_EVT | 音频流状态改变事件 |
| BT_PRF_A2DP_AUDIO_SBC_CFG_EVT | 音频流参数配置事件 |

- 返回值

无

-  注意事项
无 
### bt_prf_a2dp_register_callback

- 函数原型

```c
void bt_prf_a2dp_register_callback(bt_prf_a2dp_cb_t callback);
```

- 功能描述

A2DP模块注册事件回调函数

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_a2dp_cb_t callback | 事件回调函数 |


- 返回值

无

-  注意事项
 无 
### bt_prf_a2dp_sink_register_data_callback

- 函数原型

```c
void bt_prf_a2dp_sink_register_data_callback(bt_prf_a2dp_sink_data_cb_t callback);
```

- 功能描述

A2DP模块注册音频数据回调函数

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_a2dp_sink_data_cb_t callback | 音频数据回调函数 |


- 返回值

无

-  注意事项
目前音频需要协议栈内部处理，该函数暂时不需要实现
### bt_prf_a2dp_sink_init

- 函数原型

```c
bt_stack_status_t bt_prf_a2dp_sink_init(void);
```

- 功能描述

A2DP SINK模块初始化

- 参数描述

无

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_a2dp_sink_connect

- 函数原型

```c
bt_stack_status_t bt_prf_a2dp_sink_connect(bt_dev_addr_t *peer_addr);
```

- 功能描述

A2DP SINK 模块连接对端设备

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_a2dp_sink_disconnect

- 函数原型

```c
bt_stack_status_t bt_prf_a2dp_sink_disconnect(bt_dev_addr_t *peer_addr);
```

- 功能描述

A2DP SINK 模块断连接对端设备

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
## AVRCP
### bt_prf_avrcp_ct_cb_t

- 函数原型

```c
typedef void (* bt_prf_avrcp_ct_cb_t)(bt_prf_avrcp_ct_cb_event_t event, bt_prf_avrcp_ct_cb_param_t *param);
```

- 功能描述

ARVCP CT模块事件回调函数申明

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_avrcp_ct_cb_event_t event | 回调事件，具体事件见下表 |
| [in] | bt_prf_avrcp_ct_cb_param_t *param | 回调参数 |

| bt_prf_avrcp_ct_cb_event_t event |  |
| --- | --- |
| BT_PRF_AVRCP_CT_CONNECTION_STATE_EVT | 连接事件 |
| BT_PRF_AVRCP_CT_PASSTHROUGH_RSP_EVT | 透传按键事件回复 |
| BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT | 音频流参数配置事件 |
| BT_PRF_AVRCP_CT_CHANGE_NOTIFY_EVT | 对端设备状态改变通知 |
| BT_PRF_AVRCP_CT_SET_ABSOLUTE_VOLUME_RSP_EVT | 设置绝对音量回复 |

- 返回值

无

-  注意事项
无 
### bt_prf_avrcp_ct_register_callback

- 函数原型

```c
void bt_prf_avrcp_ct_register_callback(bt_prf_avrcp_ct_cb_t callback);
```

- 功能描述

ARVCP CT模块注册事件回调函数

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_avrcp_ct_cb_t callback | 事件回调函数，具体见bt_prf_avrcp_ct_cb_t |

- 返回值

无

-  注意事项
无 
### bt_prf_avrcp_ct_init

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_ct_init(void);
```

- 功能描述

ARVCP CT模块初始化

- 参数描述

无

- 返回值

无

-  注意事项
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |

### bt_prf_avrcp_ct_cmd_register_notification

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_ct_cmd_register_notification(bt_prf_avrcp_rn_event_ids_t event_id);
```

- 功能描述

ARVCP CT模块发送注册通知命令

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_avrcp_rn_event_ids_t event_id | 通知事件，具体事件如下表格 |

| bt_prf_avrcp_rn_event_ids_t event_id |  |
| --- | --- |
| BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE | 播放状态改变 |
| BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE | 音量变化 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |

-  注意事项

无
### bt_prf_avrcp_ct_cmd_passthrough

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_ct_cmd_passthrough(bt_prf_avrcp_op_id_t   operation_id,
                                                  bt_prf_avrcp_op_stat_t operation_state);
```

- 功能描述

ARVCP CT模块发送按键命令

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_avrcp_op_id_t   operation_id | 按键码，需要实现的按键码如下表格 |
| [in] | bt_prf_avrcp_op_stat_toperation_state | 按键状态，pressed ： 0 ；released ：1  |

| bt_prf_avrcp_ct_cmd_t key_code |  |
| --- | --- |
| BT_PRF_AVRCP_CT_PT_CMD_VOL_UP | 音量加 |
| BT_PRF_AVRCP_CT_PT_CMD_VOL_DOWN | 音量减 |
| BT_PRF_AVRCP_CT_PT_CMD_MUTE | 静音 |
| BT_PRF_AVRCP_CT_PT_CMD_PLAY | 开始播放 |
| BT_PRF_AVRCP_CT_PT_CMD_STOP | 通知播放 |
| BT_PRF_AVRCP_CT_PT_CMD_PAUSE | 暂停播放 |
| BT_PRF_AVRCP_CT_PT_CMD_FORWARD | 下一首 |
| BT_PRF_AVRCP_CT_PT_CMD_BACKWARD | 上一首 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项

无
### bt_prf_avrcp_ct_cmd_get_play_status

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_ct_cmd_get_play_status(void);
```

- 功能描述

ARVCP CT模块发送获取TG端播放状态命令

- 参数描述

无

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项

对端设备返回值，在 **BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT **事件中返回
### bt_prf_avrcp_ct_cmd_set_absolute_volume

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_ct_cmd_set_absolute_volume(uint8_t volume);
```

- 功能描述

ARVCP CT模块发送设置绝对音量命令

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | uint8_t volume | 音量值，范围为 0 - 127 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项

无
### bt_prf_avrcp_tg_register_callback

- 函数原型

```c
void bt_prf_avrcp_tg_register_callback(bt_prf_avrcp_tg_cb_t callback);
```

- 功能描述

ARVCP TG模块注册事件回调函数

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_avrcp_tg_cb_t callback | 事件回调函数，具体见bt_prf_avrcp_tg_cb_t |

- 返回值

无

-  注意事项
无 
### bt_prf_avrcp_tg_init

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_tg_init(void);
```

- 功能描述

ARVCP TG模块初始化

- 参数描述

无

- 返回值

无

-  注意事项
### bt_prf_avrcp_tg_set_rn_evt_cap

- 函数原型

```c
void bt_prf_avrcp_tg_set_rn_evt_cap(uint16_t cap_bit_mask);
```

- 功能描述

ARVCP TG设置本地设备能力

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | uint16_t cap_bit_mask | 设备能力，是通过bt_prf_avrcp_rn_event_ids_t 按位设置  |

- 返回值

无

-  注意事项

无
### bt_prf_avrcp_tg_get_rn_evt_cap

- 函数原型

```c
uint16_t bt_prf_avrcp_tg_get_rn_evt_cap(void);
```

- 功能描述

ARVCP TG获取本地设备能力

- 参数描述

无

- 返回值
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [out] | uint16_t cap_bit_mask | 设备能力，是通过bt_prf_avrcp_rn_event_ids_t 按位设置  |

-  注意事项

无
### bt_prf_avrcp_tg_send_rn_rsp

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_tg_send_rn_rsp(bt_prf_avrcp_rn_event_ids_t event_id, bt_prf_avrcp_ctype_response_t rsp,
                                              bt_prf_avrcp_rn_param_t *param);
```

- 功能描述

ARVCP TG 发送

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_avrcp_rn_event_ids_t event_id | 事件ID |
| [in] | bt_prf_avrcp_ctype_response_t rsp | 回复事件 BT_AVRCP_RESPONSE_INTERIM 或者
BT_AVRCP_RESPONSE_CHANGED_STABLE |
| [in] | bt_prf_avrcp_rn_param_t *param | 回复参数 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项

无
### bt_prf_avrcp_tg_notify_vol_changed

- 函数原型

```c
bt_stack_status_t bt_prf_avrcp_tg_notify_vol_changed(uint8_t volume);
```

- 功能描述

ARVCP TG通知音量变化，通知对端CT音量变化 BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE，并修改本地音量。

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | uint8_t volume | 音量 范围 0 ~ 127 |

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项

无

| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |

## HFP
### bt_prf_hfp_hf_cb_t
```c
typedef void (* bt_prf_hfp_hf_cb_t)(bt_prf_hfp_hf_cb_event_t event, bt_prf_hfp_hf_cb_param_t *param);
```

- 功能描述

HFP HF模块回调函数申明

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_hfp_hf_cb_event_t event | 回调事件，具体需要实现的回调见下表 |
| [in] | bt_prf_hfp_hf_cb_param_t *param | 回调参数 |

| bt_prf_hfp_hf_cb_event_t event |  |
| --- | --- |
| BT_PRF_HFP_HF_CONNECTION_STATE_EVT | 服务连接事件 |
| BT_PRF_HFP_HF_AUDIO_STATE_EVT | 音频连接事件 |
| BT_PRF_HFP_HF_CIND_CALL_EVT | 电话状态通知 |
| BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT | 电话拨打状态通知 |
| BT_PRF_HFP_HF_CIND_CALL_HELD_EVT | 电话挂起状态通知 |
| BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT | 网络状态是否可用通知 |
| BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT | AG设备信号强度 |
| BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT | AG设备漫游状态 |
| BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT | AG设备电池状态 |
| BT_PRF_HFP_HF_BTRH_EVT | 电话回应状态通知 |
| BT_PRF_HFP_HF_VOLUME_CONTROL_EVT | 音量控制 |
| BT_PRF_HFP_HF_AT_RESPONSE_EVT | AT命令回复通知 |
| BT_PRF_HFP_HF_BSIR_EVT | 设置带内铃声 |
| BT_PRF_HFP_HF_RING_IND_EVT | 铃声通知 |

- 返回值

无

-  注意事项
无 
### bt_prf_hfp_hf_register_callback

- 函数原型

```c
void bt_prf_hfp_hf_register_callback(bt_prf_hfp_hf_cb_t callback);
```

- 功能描述

HFP HF模块注册回调函数

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_hfp_hf_cb_t callback | 回调函数 |

- 返回值

无

-  注意事项
无 
### bt_prf_hfp_hf_init

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_init(void);
```

- 功能描述

HFP HF模块初始化

- 参数描述

无

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_connect

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_connect(bt_dev_addr_t *peer_addr);
```

- 功能描述

HFP HF 模块连接对端设备

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_disconnect

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_disconnect(bt_dev_addr_t *peer_addr);
```

- 功能描述

HFP HF 模块断连接对端设备

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_audio_connect

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_audio_connect(bt_dev_addr_t *peer_addr);
```

- 功能描述

HFP HF 模块连接对端设备音频链路

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_audio_disconnect

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_audio_disconnect(bt_dev_addr_t peer_addr);
```

- 功能描述

HFP HF 模块断连接对端设备音频链路

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_dev_addr_t *peer_addr | 对端设备mac地址 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_send_chld_cmd

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_send_chld_cmd(bt_prf_hfp_hf_chld_type_t chld, int idx);
```

- 功能描述

HFP HF 模块发送AT+CHLD 命令

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_hfp_hf_chld_type_t chld | AT+CHLD  参数 |
| [in] | int idx | 当chld 为 BT_PRF_HFP_HF_CHLD_TYPE_REL_X 或者 BT_PRF_HFP_HF_CHLD_TYPE_PRIV_X，需要输入 idx |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_answer_call

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_answer_call(void);
```

- 功能描述

HFP HF 模块接听电话，发送ATA命令

- 参数描述

无

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_reject_call

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_reject_call(void);
```

- 功能描述

HFP HF 模块拒绝电话，发送AT+CHUP命令

- 参数描述

无

- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_dial

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_dial(char *number);
```

- 功能描述

HFP HF 模块拨打电话

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | char *number | 电话号码字符串 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
### bt_prf_hfp_hf_vol_update

- 函数原型

```c
bt_stack_status_t bt_prf_hfp_hf_vol_update(bt_prf_hfp_hf_vol_ctrl_target_t type, int volume);
```

- 功能描述

HFP HF 模块设置AG设备音量

- 参数描述
| IN/OUT | NAME | DESC |
| --- | --- | --- |
| [in] | bt_prf_hfp_hf_vol_ctrl_target_t type | 设置音量目标，喇叭端，或者麦克风端 |
| [in] | int volume | 音量，范围为 0 -15 |


- 返回值
| 返回值 |  |
| --- | --- |
| BT_STACK_STATUS_SUCCESS | 成功 |
| other | 失败 |


-  注意事项
无 
