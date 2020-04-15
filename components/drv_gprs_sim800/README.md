# 概述

sim800驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/sim800.h>
#include <yoc/netmgr.h>
#include <pin.h>

void board_yoc_init()
{
    /* kv config check */
    aos_kv_setint("gprs_en", 1);
    aos_kv_setint("wifi_en", 0);
    aos_kv_setint("eth_en", 0);

    sim_gprs_param_t sim_param;
    sim_param.baud           = 115200;
    sim_param.buf_size       = 4096;
    sim_param.enable_flowctl = 0;
    sim_param.device_name    = "uart1";
    sim_param.reset_pin      = GPRS_SIM800_PIN_PWR;
    sim_param.state_pin      = GPRS_SIM800_PIN_STATUS;

    gprs_sim800_register(NULL, &sim_param);
    netmgr_hdl_t app_netmgr_hdl = netmgr_dev_gprs_init();
    if (app_netmgr_hdl) {
        netmgr_service_init(NULL);
        netmgr_start(app_netmgr_hdl);
    }
}
```
