# 概述

ESP8266无线网卡驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/esp8266.h>
#include <yoc/netmgr.h>
#include <pin.h>

void board_yoc_init()
{
    /* kv config check */
    aos_kv_setint("wifi_en", 1);
    aos_kv_setint("gprs_en", 0);
    aos_kv_setint("eth_en", 0);

    esp_wifi_param_t esp_param;

    esp_param.device_name    = "uart2";
    esp_param.baud           = 115200;
    esp_param.buf_size       = 4096;
    esp_param.enable_flowctl = 0;
    esp_param.reset_pin      = WIFI_ESP8266_RESET;
    esp_param.smartcfg_pin   = WIFI_ESP8266_SMARTCFG;

    wifi_esp8266_register(NULL, &esp_param);
    netmgr_hdl_t app_netmgr_hdl = netmgr_dev_wifi_init();
    if (app_netmgr_hdl) {
        netmgr_service_init(NULL);
        netmgr_start(app_netmgr_hdl);
    }
}
```
