# 概述

有线网卡驱动, 基于csi eth接口，对接了netmgr，lwip

## 代码示例

驱动注册
```C
drv_eth_register();
```

程序初始化  
```C
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>

#include <uservice/eventid.h>
#include <uservice/event.h>


void net_init(void)
{
    drv_eth_register();

    app_netmgr_hdl = netmgr_dev_eth_init();

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

eth0	Link encap:eth  HWaddr f2:42:9f:a5:0a:72
    	inet addr:10.10.10.68
	GWaddr:10.10.10.1
	Mask:255.255.255.0
	DNS SERVER 0: 10.10.10.1
```