/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <aos/cli.h>
#include <ulog/ulog.h>
#include <wifi_provisioning.h>
#include <softap_prov.h>
#ifdef CONFIG_CSI_V2
#include <drv/gpio_pin.h>
#include <drv/pin.h>
#else
#include <pin_name.h>
#endif
#define TAG "wifi_provising"
//#define WLAN_ENABLE_PIN PC8
//#define WLAN_POWER_PIN 0xFFFFFFFF

extern void app_network_init(void);


static void wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    if (event == WIFI_PROV_EVENT_TIMEOUT) {
        LOGD(TAG, "wifi pair timeout...");
    } else {
        LOGD(TAG, "wifi pair got passwd...");
    }
}

static void wifi_provising_register(void)
{
    int ret;
    ret = wifi_prov_softap_register("YOC");
    // ret = wifi_prov_softap_register(NULL);
    // ret = wifi_prov_softap_register("s!o@n$g% t^e&s*t(");
    if(ret == 0)
    {
        LOGD(TAG, "register success!");
    }
    else
    {
        LOGE(TAG, "register failed!");
    }
    
}

static int  wifi_provising_get_method_id()
{
    int ret;
    ret = wifi_prov_get_method_id("softap");
    if(ret == 0)
    {
        LOGE(TAG, "get_method_id failed!");
    }
    else
    {
        LOGD(TAG, "get_method_id success!");
        LOGI(TAG, "method_id is %d", ret);
    }

    return ret;
    
}


static void wifi_provising_start()
{
    int ret;
    int method_id;
    method_id = wifi_provising_get_method_id();
    ret = wifi_prov_start(method_id, wifi_pair_callback, 120); 
    if(ret == 0)
    {
        LOGD(TAG, "start success!");
    }
    else
    {
        LOGE(TAG, "start failed!");
    }
    
}


static void wifi_provising_stop()
{
    wifi_prov_stop();
    LOGD(TAG, "stop ok!");
}


static void function_wifi_provising_test(char *wbuf, int wbuf_len, int argc, char **argv)
{
    
    if(argc == 2)
    {
        if(strcmp(argv[1], "register") == 0)
        {
            wifi_provising_register();
        }
        else if(strcmp(argv[1], "method_id") == 0)
        {
            wifi_provising_get_method_id();
        }
        else if(strcmp(argv[1], "start") == 0)
        {
            wifi_provising_start();
        }
        else if(strcmp(argv[1], "stop") == 0)
        {
            wifi_provising_stop();
        }
    }

}


void cli_reg_cmd_wifi_provising_test(void)
{
    static struct cli_command cmd_info = {"wifi_provising", "wifi_provising test commands", function_wifi_provising_test};

    aos_cli_register_command(&cmd_info);
}
