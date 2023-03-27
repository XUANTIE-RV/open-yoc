## Title:BLE UART API

# 1. 概述

BLE SDK提供的自定义UART透传组件，完成了GATT 自定义UART透传服务的注册和蓝牙协议栈的事件处理，应用程序只需分别调用UART透传Server组件API以及UART透传Client组件API即可实现数据的透传转发

# 2. 服务介绍

BLE UART 透传组件为自定义服务，开发者可通过该组件实现数据透传功能。该服务不依赖任何其他服务可独立存在，一个设备只支持一个实例

UART透传服务Primary Service定义

| 服务名称     | UUID                                                         |
| ------------ | ------------------------------------------------------------ |
| UART Service | 0x7e,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9, 0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x93 |

UART透传服务包含如下表所示特征：

| 编号 | 特征      | UUID                                                         | 说明     |
| ---- | --------- | ------------------------------------------------------------ | -------- |
| 1    | UART RX   | 0x7e,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9,0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x94 | UART接收 |
| 2    | UART   TX | 0x7e,0x31,0x35,0xd4,0x12,0xf3,0x11,0xe9,0xab,0x14,0xd6,0x63,0xbd,0x87,0x3d,0x95 | UAR发送  |

# 3.  交互流程

BLE UART Server组件与BLE UART Client组件交互处理流程如下图所示：

![](../../../../assets/zh-cn/bluetooth/at_ble/server_client.PNG)

- Server端应用程序初始化BLE UART Server组件

- Server组件完成GATT服务的注册、蓝牙协议栈事件回调函数的注册

- Client端应用程序初始化BLE UART Client组件

- Client组件完成蓝牙协议栈事件回调函数的注册

- Client端开启扫描

- Server端应用程序发起广播

- Client端扫描蓝牙设备，并发起连接建立请求

- 连接成功建立后，Client端完成Primary Service以及该服务包含的各个特征属性的发现，并使能Server端TX属性的Notify

- Server端应用程序调用ble_prf_uart_server_send发送数据到Client端，Client端应用程序通过uart_rx_data_cb回调函数接收数据

- Client端应用程序调用ble_prf_uart_client_send发送数据到Server端，Server端应用程序通过uart_rx_data_cb回调函数接收数据

  

# 4. 接口定义

该部分主要包含BLE UART server组件API以及BLE UART client组件API

## 4.1 BLE UART Server API

### 4.1.1 ble_prf_uart_server_init

- 函数原型

```c
uart_handle_t ble_prf_uart_server_init(ble_uart_server_t *service)
```

- 功能描述

​     完成BLE UART Server服务初始化

- 参数描述

| IN/OUT | NAME                       | DESC                                                         |
| ------ | -------------------------- | ------------------------------------------------------------ |
| [in]   | ble_uart_server_t *service | BLE UART Server初始化结构体；参见ble_uart_server_t结构体定义 |

- 返回值

| NAME          | DESC                                                     |
| ------------- | --------------------------------------------------------- |
| uart_handle_t | BLE UART Server组件注册句柄，可转为ble_uart_server_t*使用 |

- 注意事项
  无
| ble_uart_server_t（结构体）定义  |                                                           |
| -------------------------------- | --------------------------------------------------------- |
| int16_t conn_handle              | 蓝牙连接句柄，记录当前实例的蓝牙连接句柄                  |
| uint16_t uart_svc_handle         | BLE UART服务句柄，记录当前实例的UART服务句柄              |
| uart_rx_data_cb uart_recv        | BLE UART接收数据回调函数，参见uart_rx_data_cb回调函数定义 |
| ble_event_cb uart_event_callback | BLE 协议栈事件回调函数，参见ble_event_cb回调函数定义      |
| conn_param_t *conn_param         | 连接更新参数                                              |
| uint8_t conn_update_def_on       | 连接参数是否默认开启更新，0（关闭）/1（开启）             |
| uint8_t update_param_flag        | 连接参数更新标志，0(未更新)/1（已更新）                   |
| uint8_t mtu_exchanged            | mtu交换标志，0（未更新）/1（已更新）                      |
| uint16_t mtu                     | mtu大小                                                   |
| uart_server server_data          | uart server数据，参见uart_server结构体定义                |

####    uart_rx_data_cb回调定义

- 函数原型

```c
int (*uart_rx_data_cb)(const uint8_t *, int)
```

- 功能描述

  用于上报BLE UART组件接收到的数据

- 参数描述

| IN/OUT | NAME            | DESC     |
| ------ | --------------- | -------- |
| [in]   | const uint8_t * | 数据指针 |
| [in]   | int             | 数据长度 |

- 返回值

| 返回值 |          |
| ------ | -------- |
| int    | 上报结果 |

- 注意事项

  无

#### ble_event_cb 回调定义

  函数原型

  ```c
  int (*ble_event_cb)(ble_event_en, void *)
  ```

- 功能描述

    用于上报BLE 协议栈事件

- 参数描述

| IN/OUT | NAME         | DESC                                           |
| ------ | ------------ | ----------------------------------------------- |
| [in]   | ble_event_en | ble协议栈事件类型，参见ble_event_en（枚举）定义 |
| [in]   | void *       | ble协议栈事件数据                               |

- 返回值

| 返回值 |          |
| ------ | -------- |
| int    | 上报结果 |

- 注意事项

无

| uart_server（结构体）定义 |                                           |
| ------------------------- | ----------------------------------------- |
| uint8_t adv_def_on        | 默认开启广播标志，0（不开启）/1（开启）   |
| adv_param_t *advParam     | 广播参数，参见adv_param_t结构体定义       |
| ccc_value_en tx_ccc_value | ccc使能标志，参见ccc_value_en（枚举）定义 |

| ccc_value_en（枚举）定义 |              |
| ------------------------ | ------------ |
| CCC_VALUE_NONE     = 0   | 未使能       |
| CCC_VALUE_NOTIFY   = 1   | 使能Notify   |
| CCC_VALUE_INDICATE = 2   | 使能Indicate |

### 4.1.2 ble_prf_uart_server_send

- 函数原型

```c
int ble_prf_uart_server_send(uart_handle_t handle,const char *data,int length,bt_uart_send_cb *cb)
```

- 功能描述

     BLE UART Server组件发送数据

- 参数描述

| IN/OUT | NAME                 | DESC                                                |
| ------ | -------------------- | --------------------------------------------------- |
| [in]   | uart_handle_t handle | BLE UART Server组件注册句柄                         |
| [in]   | const char *data     | 发送数据指针                                        |
| [in]   | int length           | 发送数据长度                                        |
| [in]   | bt_uart_send_cb *cb  | 发送数据回调函数，参见bt_uart_send_cb（结构体）定义 |

- 返回值

| 返回值 |          |
| ------ | -------- |
| 0      | 发送成功 |
| <0     | 发送失败 |

- 注意事项

  无

| bt_uart_send_cb（结构体）定义         |                                             |
| ------------------------------------- | ------------------------------------------- |
| void (*start)(int err, void *cb_data) | 数据开始发送回调函数，参见start回调函数定义 |
| void (*end)(int err, void *cb_data)   | 数据发送完成回调函数，参见end回调函数定义   |


#### start回调定义

- 函数原型

  ```c
  void (*start)(int err, void *cb_data)
  ```

- 功能描述

  上层应用注册的数据开始发送回调函数

- 参数描述

| IN/OUT | NAME          | DESC                        |
| ------ | ------------- | --------------------------- |
| [in]   | int err       | 错误码，0为正确，其它为错误 |
| [in]   | void *cb_data | 回调数据，目前为NULL        |

- 返回值

  无

- 注意事项

  无
  
#### end回调定义

- 函数原型
  
    ```c
    void (*end)(int err, void *cb_data)
    ```
  
- 功能描述
  
    上层应用注册的数据发送结束回调函数
  
- 参数描述
  
| IN/OUT | NAME          | DESC                        |
| ------ | ------------- | --------------------------- |
| [in]   | int err       | 错误码，0为正确，其它为错误 |
| [in]   | void *cb_data | 回调数据，目前为NULL        |

- 返回值
  
    无
  
- 注意事项
  
    无

### 4.1.3 ble_prf_uart_server_disconn

- 函数原型

```c
int ble_prf_uart_server_disconn(uart_handle_t handle)
```

- 功能描述

  BLE UART Server组件断开BLE连接

- 参数描述

| IN/OUT | NAME                 | DESC                        |
| ------ | -------------------- | --------------------------- |
| [in]   | uart_handle_t handle | BLE UART Server组件注册句柄 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| <0     | 失败 |

- 注意事项

  无

### 4.1.4 ble_prf_uart_server_adv_control

- 函数原型

```c
int ble_prf_uart_server_adv_control(uint8_t adv_on, adv_param_t *adv_param)
```

- 功能描述

  BLE UART Server组件控制adv开关及设置adv参数

- 参数描述

| IN/OUT | NAME                   | DESC                               |
| ------ | ---------------------- | ---------------------------------- |
| [in]   | uart_handle_t handle   | BLE UART Server组件注册句柄        |
| [in]   | adv_param_t *adv_param | adv参数，参见adv_param_t结构体定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| <0     | 失败 |

- 注意事项

  无

### 4.1.5 ble_prf_uart_server_conn_param_update
- 函数原型

```c
int ble_prf_uart_server_conn_param_update(uart_handle_t handle, conn_param_t *param)
```

- 功能描述

  BLE UART Server组件更新连接参数

- 参数描述

| IN/OUT | NAME                 | DESC                                 |
| ------ | -------------------- | ------------------------------------ |
| [in]   | uart_handle_t handle | BLE UART Server组件注册句柄          |
| [in]   | conn_param_t *param  | conn参数，参见conn_param_t结构体定义 |

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| <0     | 失败 |

- 注意事项

  无


## 4.2 BLE UART Client API

### 4.2.1 ble_prf_uart_client_init

- 函数原型

```c
uart_handle_t ble_prf_uart_client_init(ble_uart_client_t *service)
```

- 功能描述

     完成BLE UART Client服务初始化

- 参数描述

| IN/OUT | NAME                       | DESC                                                         |
| ------ | -------------------------- | ------------------------------------------------------------ |
| [in]   | ble_uart_client_t *service | BLE UART Client初始化结构体；参见ble_uart_client_t结构体定义 |

- 返回值

| NAME          | DESC                                                    |
| ------------- | --------------------------------------------------------- |
| uart_handle_t | BLE UART Server组件注册句柄，可转为ble_uart_client_t*使用 |

- 注意事项
  无
| ble_uart_client_t（结构体）定义  |                                                           |
| -------------------------------- | --------------------------------------------------------- |
| int16_t conn_handle              | 蓝牙连接句柄，记录当前实例的蓝牙连接句柄                  |
| uart_rx_data_cb uart_recv        | BLE UART接收数据回调函数，参见uart_rx_data_cb回调函数定义 |
| ble_event_cb uart_event_callback | BLE 协议栈事件回调函数，参见ble_event_cb回调函数定义      |
| conn_param_t *conn_param         | 连接更新参数                                              |
| uint8_t conn_update_def_on       | 连接参数是否默认开启更新，0（关闭）/1（开启）             |
| uint8_t update_param_flag        | 连接参数更新标志，0(未更新)/1（已更新）                   |
| uint8_t mtu_exchanged            | mtu交换标志，0（未更新）/1（已更新）                      |
| uint16_t mtu                     | mtu大小                                                   |
| uart_client client_data          | uart client数据，参见uart_client结构体定义                |

| client_config（结构体）定义 |                                                     |
| --------------------------- | --------------------------------------------------- |
| uint8_t conn_def_on         | 默认连接开关，0（默认不发起连接）/1（默认发起连接） |
| uint8_t auto_conn_mac_size  | 自动连接匹配设备地址数量                            |
| dev_addr_t *auto_conn_mac   | 自动连接匹配设备地址，参见dev_addr_t（结构体）定义  |

### 4.2.2 ble_prf_uart_client_scan_start
- 函数原型

```c
int ble_prf_uart_client_scan_start()
```

- 功能描述

   UART Client设备开启扫描

- 参数描述

  无

- 返回值

| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项

  无
  
### 4.2.3 ble_prf_found_dev_get

- 函数原型
  
  ```c
  dev_addr_t *ble_prf_found_dev_get()
  ```
  
- 功能描述
  
     UART Client设备获取扫描到的设备
  
- 参数描述
  
    无
 - 返回值

| 返回值 |      |
| ------ | ---- |
| 非NULL | 成功 |
| NULL   | 失败 |

  - 注意事项
  
    每调用一次该接口，返回一个最近扫描到的设备
    
    ### 4.2.4 ble_prf_uart_client_conn
    
- 函数原型
  
    ```c
    int ble_prf_uart_client_conn(dev_addr_t *conn_mac, conn_param_t *conn_param)
    ```
    
- 功能描述
  
    UART Client连接设备
    
- 参数描述
  
| IN/OUT | NAME                     | DESC                                |
| ------ | ------------------------ | ---------------------------------------- |
| [in]   | dev_addr_t *conn_mac     | 连接设备地址，参见dev_addr_t结构体定义 |
| [in]   | conn_param_t *conn_param | 设备连接参数，参见conn_param_t结构体定义 |

- 返回值
  
| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项
  
  执行成功不代表连接成功，连接成功后通过ble_event_cb回调函数，上报EVENT_GAP_CONN_CHANGE事件
### 4.2.4 ble_prf_uart_client_disconn

- 函数原型
  
    ```c
    int ble_prf_uart_client_disconn(uart_handle_t handle)
    ```
    
- 功能描述
  
    UART Client断开连接设备
    
- 参数描述
  
| IN/OUT | NAME                 | DESC                        |
| ------ | -------------------- | --------------------------- |
| [in]   | uart_handle_t handle | BLE UART Client组件注册句柄 |

- 返回值
  
| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| 非0    | 失败 |

- 注意事项
  
  执行成功不代表断开成功，断开成功后通过ble_event_cb回调函数，上报EVENT_GAP_CONN_CHANGE事件
  
### 4.2.5 ble_prf_uart_client_send

- 函数原型
  
  ```c
  int ble_prf_uart_client_send(uart_handle_t handle,const char *data,int length,bt_uart_send_cb *cb)
  ```
  
- 功能描述
  
       BLE UART Client组件发送数据
  
- 参数描述
  
| IN/OUT | NAME                 | DESC                                                |
| ------ | -------------------- | --------------------------------------------------- |
| [in]   | uart_handle_t handle | BLE UART Server组件注册句柄                         |
| [in]   | const char *data     | 发送数据指针                                        |
| [in]   | int length           | 发送数据长度                                        |
| [in]   | bt_uart_send_cb *cb  | 发送数据回调函数，参见bt_uart_send_cb（结构体）定义 |

- 返回值
  
| 返回值 |          |
| ------ | -------- |
| 0      | 发送成功 |
| <0     | 发送失败 |

- 注意事项
  
    无
    
### 4.2.6 ble_prf_uart_client_conn_param_update
- 函数原型
  
    ```c
    int ble_prf_uart_client_conn_param_update(uart_handle_t handle, conn_param_t *param)
    ```
    
- 功能描述
  
      BLE UART Client组件更新连接参数
  
- 参数描述
  
| IN/OUT | NAME                 | DESC                                 |
| ------ | -------------------- | ------------------------------------ |
| [in]   | uart_handle_t handle | BLE UART Client组件注册句柄          |
| [in]   | conn_param_t *param  | conn参数，参见conn_param_t结构体定义 |

- 返回值
  
| 返回值 |      |
| ------ | ---- |
| 0      | 成功 |
| <0     | 失败 |

  

- 注意事项
  无
