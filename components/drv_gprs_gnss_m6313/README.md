# 概述

中移动M6313驱动组件，包含gprs gnss两个驱动，实现了基于gprs的数据通道和gnss定位功能，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/m6313.h>
#include <yoc/netmgr.h>
#include <pin.h>

void board_yoc_init()
{
    aos_kv_setint("wifi_en", 0);
    aos_kv_setint("gprs_en", 1);
    aos_kv_setint("eth_en", 0);
    m6313_param_t sim_param;
    sim_param.baud           = 115200;
    sim_param.buf_size       = 4096;
    sim_param.enable_flowctl = 0;
    sim_param.device_name    = "uart2";
    sim_param.reset_pin      = GPRS_SIM800_PIN_STATUS;
    sim_param.state_pin      = GPRS_SIM800_PIN_PWR;

    gprs_m6313_register(NULL, &sim_param);
    gnss_m6313_register(NULL, &sim_param);
    app_netmgr_hdl = netmgr_dev_gprs_init();

    utask_t *task = utask_new("netmgr", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    if (app_netmgr_hdl) {
        netmgr_service_init(task);
        netmgr_start(app_netmgr_hdl);
    }
}
```
