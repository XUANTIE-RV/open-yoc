# 概述

u1驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/u1.h>
#include <yoc/netmgr.h>

void board_yoc_init()
{
    u1_nbiot_param_t nbiot_param;
    nbiot_param.device_name = "uart7";
    nbiot_u1_register(NULL, &nbiot_param);
    netmgr_hdl_t app_netmgr_hdl = netmgr_dev_gprs_init();
    if (app_netmgr_hdl) {
        netmgr_service_init(NULL);
        netmgr_start(app_netmgr_hdl);
    }
}
```
