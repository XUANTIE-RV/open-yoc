# 概述

rtl8723ds无线网卡驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/rtl8723ds.h>
#include <yoc/netmgr.h>
#include <pin.h>

static netmgr_hdl_t _network_init()
{

    /* kv config check */
    aos_kv_setint("wifi_en", 1);

    netmgr_hdl_t netmgr_hdl = netmgr_dev_wifi_init();

    netmgr_service_init(NULL);
    if (netmgr_hdl)
        netmgr_start(netmgr_hdl);

    return netmgr_hdl;
}

void board_yoc_init()
{
    /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };

    wifi_rtl8723ds_register(&pin);
    netmgr_get_dev(_network_init());
}
```
