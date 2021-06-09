/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "../yunit.h"
#include <softap_prov.h>
#include <wifi_provisioning.h>
#include <devices/wifi.h>
#include <ulog/ulog.h>

#define TAG "wifi_prov_test"

static void test_wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    if (event == WIFI_PROV_EVENT_TIMEOUT) {
        LOGD(TAG, "wifi pair timeout...");
    } else {
        LOGD(TAG, "wifi pair got passwd...");
    }
}

static void test_wifi_prov_softap_register(void)
{
    int ret;

    ret = wifi_prov_softap_register("YoC");
    YUNIT_ASSERT_MSG_QA(ret == 0, "softap_register result is %d", ret, "YoC_WIFI_PROVISING_01");

    ret = wifi_prov_softap_register(NULL);
    YUNIT_ASSERT_MSG_QA(ret == 0, "softap_register result is %d", ret, "YoC_WIFI_PROVISING_02");

    ret = wifi_prov_softap_register("s!o@n$g% t^e&s*t(");
    YUNIT_ASSERT_MSG_QA(ret == 0, "softap_register result is %d", ret, "YoC_WIFI_PROVISING_03");

    ret = wifi_prov_softap_register("宋圣洁测试");
    YUNIT_ASSERT_MSG_QA(ret == 0, "softap_register result is %d", ret, "YoC_WIFI_PROVISING_04");
}

static void test_wifi_prov_method_register(void)
{
    wifi_prov_t softap_prov = {
    .name        = "softap",
    .start       = NULL,
    .stop        = NULL,
    };

    int ret;
    ret = wifi_prov_method_register(&softap_prov);
    YUNIT_ASSERT_MSG_QA(ret == -1, "method_register result is %d", ret, "YoC_WIFI_PROVISING_05");
}

static void test_wifi_prov_start(void)
{
    int ret;

    ret = wifi_prov_start(1, test_wifi_pair_callback, 120);
    YUNIT_ASSERT_MSG_QA(ret == 0, "prov_start result is %d", ret, "YoC_WIFI_PROVISING_06");
    // fatal because get into assert
    // wifi_prov_stop();

    // fatal because get into assert
    // ret = wifi_prov_start(1, NULL, 120);
    // YUNIT_ASSERT_MSG_QA(ret == -1, "prov_start result is %d", ret, "YoC_WIFI_PROVISING_07");
    // LOGD(TAG, "YoC_WIFI_PROVISING_07 success!");
    // fatal because get into assert
    // wifi_prov_stop();

    // fatal because get into assert
    // ret = wifi_prov_start(1, test_wifi_pair_callback, -1);
    // YUNIT_ASSERT_MSG_QA(ret == -1, "prov_start result is %d", ret, "YoC_WIFI_PROVISING_08");
    // LOGD(TAG, "YoC_WIFI_PROVISING_08 success!");
    // fatal because get into assert
    // wifi_prov_stop();

}

// get_method_id depends on register
static void test_wifi_prov_get_method_id(void)
{
    int ret;

    wifi_prov_softap_register("YoC");
    ret = wifi_prov_get_method_id("YoC");
    YUNIT_ASSERT_MSG_QA(ret == 0, "get_method_id result is %d", ret, "YoC_WIFI_PROVISING_09");
    // wifi_prov_stop();

    // fatal because of get into assert
    // ret = wifi_prov_get_method_id(NULL);
    // YUNIT_ASSERT_MSG_QA(ret == 1, "get_method_id result is %d", ret, "YoC_WIFI_PROVISING_010");
    // wifi_prov_stop();

    // fatal because of get into assert
    // ret = wifi_prov_get_method_id("s!o@n$g% t^e&s*t(");
    // YUNIT_ASSERT_MSG_QA(ret == 1, "get_method_id result is %d", ret, "YoC_WIFI_PROVISING_11");
    // wifi_prov_stop();

    // fatal because of get into assert
    // ret = wifi_prov_get_method_id("宋圣洁测试");
    // YUNIT_ASSERT_MSG_QA(ret == 1, "get_method_id result is %d", ret, "YoC_WIFI_PROVISING_12");
    // wifi_prov_stop();
}


static void test_wifi_provising_api_param(void)
{
    test_wifi_prov_softap_register();
    test_wifi_prov_method_register();
    test_wifi_prov_get_method_id();
    test_wifi_prov_start();
    
}


void api_wifi_provising_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "wifi_provising.softap_reg", test_wifi_prov_softap_register);
    yunit_add_test_case(suite, "wifi_provising.method_reg", test_wifi_prov_method_register);
    yunit_add_test_case(suite, "wifi_provising.get_method", test_wifi_prov_get_method_id);
    yunit_add_test_case(suite, "wifi_provising.prov_start", test_wifi_prov_start);
}
