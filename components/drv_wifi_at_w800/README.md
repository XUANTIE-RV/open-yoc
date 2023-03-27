## 概述

W800芯片是一款串口转无线模芯片，内部自带固件，用户操作简单，无需编写时序信号等。

**特性**：

* 802.11 b/g/n
* 内置低功耗32位CPU:可以兼作应用处理器
* 内置10 bit高精度ADC
* 内置TCP/IP协议栈
* 内置TR开关、balun、LNA、功率放大器和匹配网络
* 802.11b模式下+20 dBm的输出功率
* 待机状态消耗功率小于1.0 mW (DTIM3)
* 工作温度范围：-40°C - 125°C

## 配置

无。

## 接口说明

#### NET模块结构体

首先，先了解一下NETHAL相关的一个重要结构体`net_ops_t`。WiFi特有相关的操作和接口都封装在`net_ops_t`这个结构体中，相关定义在文件:`net_impl.h`。

```c
typedef struct net_ops {
    int (*get_mac_addr)(rvm_dev_t *dev, uint8_t *mac);
    int (*set_mac_addr)(rvm_dev_t *dev, const uint8_t *mac);

    int (*get_dns_server)(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);
    int (*set_dns_server)(rvm_dev_t *dev, ip_addr_t ipaddr[], uint32_t num);

    int (*set_hostname)(rvm_dev_t *dev, const char *name);
    const char* (*get_hostname)(rvm_dev_t *dev);

    int (*set_link_up)(rvm_dev_t *dev);
    int (*set_link_down)(rvm_dev_t *dev);

    int (*start_dhcp)(rvm_dev_t *dev);
    int (*stop_dhcp)(rvm_dev_t *dev);
    int (*set_ipaddr)(rvm_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask, const ip_addr_t *gw);
    int (*get_ipaddr)(rvm_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw);
    int (*ping)(rvm_dev_t *dev, int type, char *remote_ip);

    int (*subscribe)(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param);
    int (*unsubscribe)(rvm_dev_t *dev, uint32_t event, event_callback_t cb, void *param);
} net_ops_t;
```

#### NET接口说明
| 成员 |  说明 |
| :--- |  :--- |
| get_mac_addr、set_mac_addr | 设置（获取）mac地址 |
| get_dns_server、set_dns_server | 设置（获取）dns地址 |
| set_hostname、get_hostname | 设置（获取）主机名 |
| start_dhcp、stop_dhcp | 启动（停止）dhcp |
| set_ipaddr、get_ipaddr | 设置（获取）ip相关信息，ipaddr、netmask及gw |
| ping | 网络测试（Packet Internet Groper） |
| subscribe、 unsubscribe | 订阅及取消订阅相关事件 |

#### SAL模块结构体

首先，先了解一下sal相关的一个重要结构体`sal_op_t`。sal特有相关的操作和接口都封装在`sal_op_t`这个结构体中，相关定义在文件:sal.h。

```c
typedef struct sal_op_s {
    char *version; /* Reserved for furture use. */

    /**
     * Module low level init so that it's ready to setup socket connection.
     *
     * @return  0 - success, -1 - failure
     */
    int (*init)(void);

    /**
     * Start a socket connection via module.
     *
     * @param[in]  c - connect parameters which are used to setup
     *                 the socket connection.
     *
     * @return  0 - success, -1 - failure
     */
    int (*start)(sal_conn_t *c);

    /**
     * Send data via module.
     * This function does not return until all data sent.
     *
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  data - pointer to data to send.
     * @param[in]  len - length of the data.
     * @param[in]  remote_ip - remote ip address (optional).
     * @param[in]  remote_port - remote port number (optional).
     * @param[in]  timeout - packet send timeout (ms)
     * @return  0 - success, -1 - failure
     */
    int (*send)(int fd, uint8_t *data, uint32_t len,
                char remote_ip[16], int32_t remote_port, int32_t timeout);

    int (*recv)(int fd, uint8_t *data, uint32_t len,
                char remote_ip[16], int32_t remote_port);

    /**
     * Get IP information of the corresponding domain.
     * Currently only one IP string is returned (even when the domain
     * coresponses to mutliple IPs). Note: only IPv4 is supported.
     *
     * @param[in]   domain - the domain string.
     * @param[out]  ip - the place to hold the dot-formatted ip string.
     *
     * @return  0 - success, -1 - failure
     */
    int (*domain_to_ip)(char *domain, char ip[16]);

    /**
     * Close the socket connection.
     *
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  remote_port - remote port number (optional).
     *
     * @return  0 - success, -1 - failure
     */
    int (*close)(int fd, int32_t remote_port);

    /**
     * Destroy SAL or exit low level state if necessary.
     *
     * @return  0 - success, -1 - failure
     */
    int (*deinit)(void);

    /**
     * Register network connection data input function
     * Input data from module.
     * This callback should be called when the data is received from the module
     * It should tell the sal where the data comes from.
     * @param[in]  fd - the file descripter to operate on.
     * @param[in]  data - the received data.
     * @param[in]  len - expected length of the data when IN,
     *                    and real read len when OUT.
     * @param[in]  addr - remote ip address. Caller manages the
                                memory (optional).
     * @param[in]  port - remote port number (optional).
     *
     * @return  0 - success, -1 - failure
     */
    int (*register_netconn_data_input_cb)(netconn_data_input_cb_t cb);

    int (*register_netconn_close_cb)(netconn_close_cb_t cb);

#ifdef SAL_SERVER
    /**
    * Register remote client status function
    * Input data from module.
    * This callback should be called when the data is received from the module
    * It should tell the sal where the data comes from.
    * @param[in]  fd - the file descripter to operate on.
    * @param[in]  status - remote client status
    * @param[in]  addr - remote ip address. Caller manages the
                              memory (optional).
    * @param[in]  port - remote port number (optional).
    *
    * @return  0 - success, -1 - failure
    */
    int (*register_netconn_client_status_notify)(netconn_client_status_notify_t cb);
#endif
} sal_op_t
```

#### SAL接口说明

| 成员 |  说明 |
| :--- |  :--- |
| version | 版本，将来使用 |
| init  | 初始化函数 |
| start | 启动函数 |
| send | 数据发送函数 |
| recv | 数据接收函数 |
| domain_to_ip| 获取域对应的IP地址 |
| close  | 连接关闭函数 |
| deinit | 模块注销函数 |
| register_netconn_data_input_cb | 注册网络数据输入回调函数 |
| register_netconn_close_cb | 注册网络连接关闭回调函数 |
| register_netconn_client_status_notify | 注册客户端状态通知回调函数 |

## 示例

### 驱动注册

主要流程为**注册网卡驱动**----->**启动netmgr微服务**（netmgr详细说明 [网络管理器](https://yoc.docs.t-head.cn/yocbook/Chapter4-%E6%A0%B8%E5%BF%83%E6%A8%A1%E5%9D%97/%E7%BD%91%E7%BB%9C%E8%BF%9E%E6%8E%A5/%E7%BD%91%E7%BB%9C%E7%AE%A1%E7%90%86%E5%99%A8.html) ）

```c
#include <devices/w800.h>
#include <yoc/netmgr.h>
#include <pin.h>

void board_yoc_init()
{
    w800_wifi_param_t esp_param;

    wifi_w800_register(NULL, &esp_param);
    netmgr_hdl_t app_netmgr_hdl = netmgr_dev_wifi_init();
    if (app_netmgr_hdl) {
        netmgr_service_init(NULL);
        netmgr_start(app_netmgr_hdl);
    }
}
```

### console网络测试

- 在src/init/cli_cmd.c中的console 命令行增加网络相关命令，方便测试网卡功能

```C
void board_cli_init(utask_t *task)
{
    ...
    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();  
    ...
}
```

- **ifconfig** 

使用ifconfig命令连接网络`ifconfig ap ssid psk`，将`ssid`和`psk`更新为路由器的账号和密码

- **ping**

确认连接网络后，使用`ping`命令测试网络是否连接外网，

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。