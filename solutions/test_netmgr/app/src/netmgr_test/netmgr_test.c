/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */


#include "../yunit.h"
#include <app_config.h>

static int init(void)
{
    return 0;
}
static int cleanup(void)
{
    return 0;
}
static void setup(void)
{
}
static void teardown(void)
{
}

extern void api_netmgr_test_entry(yunit_test_suite_t *suite);

/**
 * 用户修改register函数
 */
void test_wifi_register()
{
    aos_kv_setint("wifi_en", 1);
    aos_kv_setint("gprs_en", 0);
    aos_kv_setint("eth_en", 0);

    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
	.power = WLAN_POWER_PIN,
    };
    wifi_rtl8723ds_register(&pin);
}

void api_netmgr_test(void)
{
	yunit_test_suite_t *suite;
	suite = yunit_add_test_suite("netmgrapi", init, cleanup, setup, teardown);

	api_netmgr_test_entry(suite);
}
