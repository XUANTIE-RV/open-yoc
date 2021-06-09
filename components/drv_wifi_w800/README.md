# 概述

W800 WiFi驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
wifi_mode_e app_network_init(void)
{
    int wifi_en;
    int ret;
    ret = aos_kv_getint("wifi_en", &wifi_en);
    if (ret < 0) {
        /* 若没有设置KV,默认使能 */
        wifi_en = 1;
    }

    if (wifi_en == 0) {
        return MODE_WIFI_CLOSE;
    }

    wifi_w800_register(NULL);

#if defined(CONFIG_YOC_SOFTAP_PROV) && CONFIG_YOC_SOFTAP_PROV
    wifi_prov_softap_register("YoC");
#endif
#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
    wifi_prov_sl_register();
#endif

    return app_net_init();
}
```