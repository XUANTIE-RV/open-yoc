# 概述

基于CherryUSB协议栈的host侧驱动对接实现，对接了usb serial class，rndis class。

## usb serial class

该驱动对接了uart devices, 是 uart_impl.h 的一个实现

## RNDIS Class

该驱动对接了移远EN200A 4G模块，对接了RNDIS Class到 lwip协议栈和 netmgr模块，实现了gprs_impl.h。


## 代码示例

驱动注册
```C
void usb_serial_uart_register(uint8_t idx);
void gprs_ec200a_register();
```

程序初始化  
```C
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>

#include <uservice/eventid.h>
#include <uservice/event.h>

#include "usbh_core.h"
#include "usbh_rndis.h"
#include "usbh_serial.h"

#include "drv_usbh_class.h"

void net_init(void)
{
    USBH_REGISTER_RNDIS_CLASS();
    USBH_REGISTER_USB_SERIAL_CLASS();
    usbh_initialize();

    drv_ec200a_serial_register(0);

    drv_ec200a_rndis_register();

    app_netmgr_hdl = netmgr_dev_gprs_init();

    if (app_netmgr_hdl) {
        utask_t *task = utask_new("netmgr", 10 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        netmgr_service_init(task);
        netmgr_start(app_netmgr_hdl);
    }

    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);

    event_subscribe(EVENT_NTP_RETRY_TIMER, user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_NTP_SUCCESS, user_local_event_cb, NULL);
}
```

## 调试

通过注册cli命令可以查看网卡信息，和网络联通测试
```c
    cli_reg_cmd_ping();
    cli_reg_cmd_ifconfig();
```

```
(cli-uart)# ping www.baidu.com
	ping www.baidu.com(36.152.44.95)
	from 36.152.44.95: icmp_seq=1 time=40 ms
	from 36.152.44.95: icmp_seq=2 time=24 ms
	from 36.152.44.95: icmp_seq=3 time=28 ms

(cli-uart)# ifconfig

gprs0	Link encap:GPRS
    	inet addr:192.168.43.200
    	Signal Quality(31): -52 dBm
    	Sim insert is OK
    	Sim ccid:898600E61xxxxxxxxxx
```

查看usb设备

```
(cli-uart)# lsusb -t
/: Hub 01, ports=1, is roothub
    |__Port 1,Port addr:0x02, ID 058f:6254, If 0,ClassDriver=hub
/: Hub 02, ports=4, mounted on Hub 01:Port 1
    |__Port 2,Port addr:0x03, ID 05e3:0610, If 0,ClassDriver=hub
/: Hub 03, ports=4, mounted on Hub 02:Port 2
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 0,ClassDriver=rndis
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 1,ClassDriver=usb_serial
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 2,ClassDriver=usb_serial
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 3,ClassDriver=usb_serial
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 4,ClassDriver=usb_serial
    |__Port 2,Port addr:0x04, ID 0bda:8153, If 0,ClassDriver=usb_serial
```