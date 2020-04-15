# 概述

m88wi6700s无线网卡驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/m88wi6700s.h>
#include <yoc/netmgr.h>

void board_yoc_init()
{
    /* kv config check */
    aos_kv_setint("wifi_en", 1);
    aos_kv_setint("gprs_en", 0);
    aos_kv_setint("eth_en", 0);

    wifi_m88wi6700s_register();
    netmgr_hdl_t app_netmgr_hdl = netmgr_dev_wifi_init();
    if (app_netmgr_hdl) {
        netmgr_service_init(NULL);
        netmgr_start(app_netmgr_hdl);
    }
}
```
