# 概述

YOC平台提供了丰富的网络连接组件和能力，主要包括:

- 轻量级TCP/IP网络协议栈(LWIP)
- 套接字适配层(SAL)
- 连接协议(wifi、ethernet、 GPRS、NB-IoT等)
- 网络管理器(netmgr)

为支持不同的网络连接芯片类型和多元的应用场景，YOC平台提供一套完善的网络框架。该框架主要分为四层架构，分别为网络应用层、网络接口层、网络芯片层和外设接口层。


网络框架主要分为两个通道，分别如下：

- 数据通道：负责网络数据的发送和接收，可通过调用SAL或LWIP接口来实现
- 管理通道：即Netmgr

# Netmgr

网络管理器负责所有网络的联网管理功能，具体如下：

- 链路层的开启/初始化/配置/复位等
- 链路层设备信息的获取
- 链路层事件的处理和转发(linkup,linkdown)
- 网络IP的配置（DHCP或者静态IP）

Netmgr根据联网管理的需要，调用到对应设备的网络层和链路层接口函数。YOC将不同类型的网络连接外设都看作是具有网络层功能和链路层功能的外设，这样就可以统一所有的网络设备。

Netmgr各层次功能如下：

- HAL层：是驱动适配层的函数指针封装。
- 驱动适配层：将网络层接口和链路层接口(xxx_impl.h)做了对接。
- 设备实现层：实现了网络连接设备相关的一些操作

# 接口设计

## 主要类型定义

```c
typedef enum {
    ETH_MAC_SPEED_10M,
    ETH_MAC_SPEED_100M,
    ETH_MAC_SPEED_1000M,
} eth_mac_speed_e;

typedef enum {
    ETH_MAC_DUPLEX_HALF,
    ETH_MAC_DUPLEX_FULL,
} eth_mac_duplex_e;

typedef void *netmgr_hdl_t;

typedef struct netmgr_dev_s {
    slist_t     next;

    char name[16];
    uint8_t id;
    uint8_t enable;
    union {
        wifi_setting_t wifi_config;
        eth_setting_t eth_config;
        gprs_setting_t gprs_config;
    } config;

    uint8_t dhcp_en;
    uint8_t mac_en;
    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    int (*provision)(struct netmgr_dev_s *node);
    int (*unprovision)(struct netmgr_dev_s *node);
    int (*info)(struct netmgr_dev_s *node);
    int (*reset)(struct netmgr_dev_s *node);

    dev_t *dev;

    /* status */
    int is_gotip;
} netmgr_dev_t;
```

## netmgr_dev_wifi_init

```c
netmgr_hdl_t netmgr_dev_wifi_init();
```

wifi设备初始化

- 参数:无
- 返回值:
  - 非空: 成功
  - 空: 失败

## netmgr_service_init

```c
void netmgr_service_init(utask_t *task)
```

网络管理服务初始化

- 参数:
  - task:  微任务句柄
- 返回值:无

## netmgr_wifi_deinit

```c
void netmgr_wifi_deinit(netmgr_hdl_t hdl);
```

wifi设备反初始化

- 参数:
  - hdl:  wifi句柄
- 返回值:无

## netmgr_get_handle

```c
netmgr_hdl_t netmgr_get_handle(const char *name);
```

根据网络设备名称获取网络管理器句柄

- 参数:
  - name:  网络设备名称，如wifi
- 返回值:
  - 非空: 成功
  - 空: 失败

## netmgr_config_wifi

```c
int netmgr_config_wifi(netmgr_hdl_t hdl, char *ssid, uint8_t ssid_length, char *psk, uint8_t psk_length);
```

连接指定ssid名称的wifi

- 参数:
  - hdl: wifi句柄
  - ssid：无线网络名称
  - ssid_length：无线网络名称长度
  - psk：无线网络密码
  - psk_length：无线网络密码长度
- 返回值:
  - 0: 成功
  - -1: 失败

## netmgr_start

```c
int netmgr_start(netmgr_hdl_t hdl);
```

使能指定网络设备开始正常工作(如对于wifi，从kv分区读取无线网络ssid名称和密码，并连接该ssid)

- 参数:
  - hdl: 网络管理器句柄
- 返回值:
  - 0: 成功
  - -1: 失败

## netmgr_reset

```c
int netmgr_reset(netmgr_hdl_t hdl, int sec);
```

复位重连指定网络设备

- 参数:
  - hdl：网络管理器句柄
  - sec：延时重置时间(当小于等于0时，立即reset)
- 返回值:
  - 0: 成功
  - -1: 失败

## netmgr_stop

```c
int netmgr_stop(netmgr_hdl_t hdl);
```

停止指定网络设备运行

- 参数:
  - hdl: 网络管理器句柄
- 返回值:
  - 0: 成功
  - -1: 失败

## netmgr_is_gotip

```c
int netmgr_is_gotip(netmgr_hdl_t hdl);
```

指定网络设备是否获取到ip

- 参数:
  - hdl: 网络管理器句柄
- 返回值:
  - 0: 未获取
  - 1: 已获取

# 使用示例

## rtl8723ds无线网卡连接

示例代码如下：

```c
#define PIN_WI_EN     PA16
#define PIN_POWER_EN  0xFFFFFFFF

/* network event callback */
void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    char url[128];
    if (event_id == EVENT_NETMGR_GOT_IP) { // 网络连接成功
        LOGD(TAG, "Net up");
    } else if (event_id == EVENT_NETMGR_NET_DISCON) {
        if ((int)param == NET_DISCON_REASON_DHCP_ERROR) {
            LOGD(TAG, "Net down"); 
            netmgr_reset(netmgr_get_handle("wifi"), 30); // 本次网络连接失败，30s后自动重连
        }
    }
}

static netmgr_hdl_t network_init()
{
    /* kv config check */
    aos_kv_setstring("wifi_ssid", "your ssid"); // 配置无线网络的名称和密码，并使能wifi
    aos_kv_setstring"wifi_psk", "your psk");
    aos_kv_setint("wifi_en", 1);

    netmgr_hdl_t netmgr_hdl = netmgr_dev_wifi_init(); // 初始化wifi网卡

    netmgr_service_init(NULL); // 初始化网络管理服务
    if (netmgr_hdl)
        netmgr_start(netmgr_hdl); // 启动wifi管理器

    return netmgr_hdl;
}

void main()
{
    board_yoc_init();

    /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = PIN_WI_EN,
        .power = PIN_POWER_EN,
    };

    wifi_rtl8723ds_register(&pin);// 配置rtl8723ds网卡相应的管脚
    network_init();
    /* 系统事件订阅 */
    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL); // 订阅网络连接成功事件
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL); // 订阅网络连接失败事件
}

```
