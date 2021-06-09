## 概述

在AliOS Things移植过程中，如果需要支持外接Wifi/BLE等模，且TCP/IP协议栈运行模组侧；则需要SAL和底层模组控制模块进行对接。SAL功能对上层提供标准socket接口，使上层应用不感知TCP/IP协议栈运行在MCU侧还是通讯模组侧。

## 组件安装

```bash
yoc init
yoc install sal
```

## 配置
无。

## 接口列表
| 函数 | 说明 |
| :--- | :--- |
| sal_module_register | 模块注册 |
| sal_module_init | 模块初始化 |
| sal_module_start | 模块建立socket连接 |
| sal_module_send | 模块发送数据 |
| sal_module_domain_to_ip | 获取当前域的IP地址 |
| sal_module_close | 模块关闭socket连接 |
| sal_module_deinit | 模块注销 |
| sal_module_register_netconn_data_input_cb | 模块注册网络连接输入回调函数 |
| sal_module_register_netconn_close_cb | 模块注册网络连接关闭回调函数 |
| sal_module_register_client_status_notify_cb | 模块注册客户端状态通知回调函数 |

## 接口详细说明

### sal_module_register
`int sal_module_register(sal_op_t *module);`

- 功能描述:
   - 模块向SAL注册。
   模块结构如下。
#### sal_op_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| version | char * | 版本，将来使用 |
| init | int (*init)(void) | 初始化函数 |
| start | int (*start)(sal_conn_t *c) | 启动函数 |
| send | int (*send)(int fd, uint8_t *data, uint32_t len, char remote_ip[16], int32_t remote_port, int32_t timeout) | 数据发送函数 |
| recv | int (*recv)(int fd, uint8_t *data, uint32_t len, char remote_ip[16], int32_t remote_port) | 数据接收函数 |
| domain_to_ip | int (*domain_to_ip)(char *domain, char ip[16]) | 获取域对应的IP地址 |
| close | int (*close)(int fd, int32_t remote_port) | 连接关闭函数 |
| deinit | int (*deinit)(void) | 模块注销函数 |
| register_netconn_data_input_cb | int (*register_netconn_data_input_cb)(netconn_data_input_cb_t cb) | 注册网络数据输入回调函数 |
| register_netconn_close_cb | int (*register_netconn_close_cb)(netconn_close_cb_t cb) | 注册网络连接关闭回调函数 |
| register_netconn_client_status_notify | int (*register_netconn_client_status_notify)(netconn_client_status_notify_t cb) | 注册客户端状态通知回调函数 |   

- 参数:
   - `module`: 模块。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### sal_module_init
`int sal_module_init(void);`

- 功能描述:
   - 模块初始化。

- 参数:
   - 无。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### sal_module_start
`int sal_module_start(sal_conn_t *conn);`

- 功能描述:
   - 模块建立socket连接。
#### sal_conn_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| fd | int | 文件描述符，用于socket连接 |
| type | CONN_TYPE | 建立连接的类型 |
| addr | char * | 对端ip或者域名，例如“192.168.1.1”或者“www.taobao.com” |
| r_port | int32_t | 对端的端口 |  
| l_port | int32_t | 本地端口 |   
| tcp_keep_alive | uint32_t | TCP保活值，若不用设置为0 |    

#### CONN_TYPE
| 枚举 | 说明 |
| :--- | :--- | 
| TCP_SERVER | TCP服务器 |
| TCP_CLIENT | TCP客户端 |
| SSL_CLIENT | SSL客户端 |
| UDP_BROADCAST | UDP广播 |  
| UDP_UNICAST | UDP单播 |   

- 参数:
   - `conn`: 连接参数，用于建立socket连接。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### sal_module_send
`int sal_module_send(int fd, uint8_t *data, uint32_t len, char remote_ip[16],
                    int32_t remote_port, int32_t timeout);`
		    
- 功能描述:
   - 模块发送数据。

- 参数:
   - `fd`: 连接对应SAL socket层的句柄。
   - `data`: 要发送的数据。
   - `len`: 要发送的数据长度。
   - `remote_ip`: 对端IP（可选）。
   - `remote_port`: 对端端口（可选）。   
   - `timeout`: 超时时间。      
   
- 返回值:
   - 0: 成功。
   - -1: 失败。		    

### sal_module_domain_to_ip
`int sal_module_domain_to_ip(char *domain, char ip[16]);`

- 功能描述:
   - 获取对应域名IP地址。注意：1、即使该域名对应多个IP，也只会返回一个IP地址；2、目前该接口只需要支持IPv4。

- 参数:
   - `domain`: 域。
   - `ip`: 返回的IP地址。点分格式的IP字符串，目前只支持IPv4，例如:192.168.111.111。

- 返回值:
   - 0: 成功。
   - -1: 失败。	

### sal_module_close
`int sal_module_close(int fd, int32_t remote_port);`

- 功能描述:
   - 模块关闭socket连接。

- 参数:
   - `fd`: 连接对应SAL socket层的句柄。
   - `remote_port`: 对端端口（可选）。

- 返回值:
   - 0: 成功。
   - -1: 失败。	
   
### sal_module_deinit
`int sal_module_deinit(void);`

- 功能描述:
   - 模块注销。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。	

### sal_module_register_netconn_data_input_cb
`int sal_module_register_netconn_data_input_cb(netconn_data_input_cb_t cb);`

- 功能描述:
   - 模块注册网络连接输入回调函数。底层模组控制模块在收到数据后，调用该接口上送到SAL中，SAL会在其中对每个句柄的数据进行管理。
   netconn_data_input_cb_t定义如下：
`typedef int (*netconn_data_input_cb_t)(int fd, void *data, size_t len, char remote_ip[16], uint16_t remote_port);`

- 参数:
   - `cb`: 回调函数。  

- 返回值:
   - 0: 成功。
   - -1: 失败。	
   
### sal_module_register_netconn_close_cb
`int sal_module_register_netconn_close_cb(netconn_close_cb_t cb);`

- 功能描述:
   - 模块注册网络连接关闭回调函数。当网络连接关闭时调用该回调函数。
   netconn_close_cb_t定义如下：
`typedef int (*netconn_close_cb_t)(int fd);`   

- 参数:
   - `cb`: 回调函数。  

- 返回值:
   - 0: 成功。
   - -1: 失败。	

### sal_module_register_client_status_notify_cb
`int sal_module_register_client_status_notify_cb(netconn_client_status_notify_t cb);`

- 功能描述:
   - 模块注册客户端状态通知回调函数。
   netconn_client_status_notify_t定义如下：
`typedef int (*netconn_client_status_notify_t)(int fd, client_status_t status, char remote_ip[16], uint16_t remote_port);`   

- 参数:
   - `cb`: 回调函数。  

- 返回值:
   - 0: 成功。
   - -1: 失败。	
   
   
## 示例

在完成SAL接口对接实现后，定义一个 `sal_op_t`结构体，将各个接口和回调的实现地址赋值给结构体中对应的域。例如：

```
sal_op_t sal_op = {
    .version = "1.0.0",
    .init = sal_wifi_init,
    .start = sal_wifi_start,
    .send = sal_wifi_send,
    .domain_to_ip = sal_wifi_domain_to_ip,
    .close = sal_wifi_close,
    .deinit = sal_wifi_deinit,
    .register_netconn_data_input_cb = sal_wifi_packet_input_cb_register,
};
```

底层模组控制模块需要实现一个函数调用`sal_module_register`对SAL进行模块注册。例如：

```
int mk3060_sal_init(void)
{
    return sal_module_register(&sal_op);
}
```

该模块注册函数需要在[device.c](https://github.com/alibaba/AliOS-Things/blob/master/device/sal/sal_device.c)的`sal_device_init`中进行调用。例如：

```
int sal_device_init()
{
    int ret = 0;

#ifdef DEV_SAL_MK3060
    ret = mk3060_sal_init();
#endif
    if (ret){
        LOGE(TAG, "device init fail ret is %d\n", ret);
    }
    
    return ret;
}
```
   
## 诊断错误码
无。

## 运行资源
无。

## 依赖资源
  - csi
  - aos

## 组件参考
无。   

