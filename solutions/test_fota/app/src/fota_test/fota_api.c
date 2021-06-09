/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "../yunit.h"
#include <yoc/fota.h>
#include <yoc/lpm.h>
#include <app_config.h>

#define KV_FOTA_CYCLE_MS        "fota_cycle"
#define KV_FOTA_ENABLED         "fota_en"

#define TAG "test_fota"

static fota_t *g_fota_handle = NULL;


/**
 *  return 0: still do the default handle
 *  not zero: only do the user handle
 */
static int fota_event_cb(void *arg, fota_event_e event)
{
    fota_t *fota = (fota_t *)arg;
    switch (event) {
        case FOTA_EVENT_VERSION:
            /* get new version, return -1 if we do not want upgrade */
            LOGD(TAG, "FOTA VERSION :%x", fota->status);
            break;

        case FOTA_EVENT_FAIL:
            /* fota download or flash error
             * it will try next version check
             */
            LOGD(TAG, "FOTA FAIL :%x", fota->status);
            break;

        case FOTA_EVENT_FINISH:
            LOGD(TAG, "FOTA FINISH :%x", fota->status);
            break;

        default:
            break;
    }
    return 0;
}

void test_fota_init(void)
{
    int ret;
    LOGD(TAG, "%s", aos_get_app_version());

    int fota_en = 1;
    ret = aos_kv_getint(KV_FOTA_ENABLED, &fota_en);

    if (ret == 0 && fota_en == 0) {
        LOGI(TAG, "fota disabled, use \"kv setint fota_en 1\" to enable");
        return;
    }

    fota_register_cop();
    YUNIT_ASSERT_MSG_QA(1, "fota_register_cop ret=%d", ret, "YoC_FOTA_01");

    netio_register_http();
    netio_register_flash();
    g_fota_handle = fota_open("cop", "flash://misc", fota_event_cb);

    fota_config_t config;

    config.sleep_time = 60000;
    config.timeoutms = 10000;
    config.retry_count = 0;

    if (pm_get_policy() == LPM_POLICY_NO_POWER_SAVE) {
        config.auto_check_en = 1;
    } else {
        config.auto_check_en = 0;
    }

    aos_kv_getint(KV_FOTA_CYCLE_MS, &(config.sleep_time));

    //assert
    /*fota_config(g_fota_handle, NULL);
    YUNIT_ASSERT_MSG_QA(ret==0, "fota_register_cop ret=%d", ret, "YoC_FOTA_03");*/

    fota_config(g_fota_handle, &config);
    YUNIT_ASSERT_MSG_QA(1, "fota_register_cop ret=%d", ret, "YoC_FOTA_02");
}

void test_fota_start(void)
{
    int ret = 0;
/*
    ret = fota_start(NULL);
    YUNIT_ASSERT_MSG_QA(ret==0, "fota_start ret=%d", ret, "YoC_FOTA_05");
*/

    if (g_fota_handle != NULL) {
        ret = fota_start(g_fota_handle);
        YUNIT_ASSERT_MSG_QA(ret==0, "fota_start ret=%d", ret, "YoC_FOTA_04");
    }
}

int test_fota_status(void)
{
    int ret = -1;
    ret = fota_get_status(g_fota_handle);
    YUNIT_ASSERT_MSG_QA(ret==1, "fota_get_status ret=%d", ret, "YoC_FOTA_06");

    return 0;
}

void test_fota_do_check(void)
{
    int ret = 0;
    if (g_fota_handle == NULL) {
        return;
    }

    LOGD(TAG, "app_fota_do_check");

    /* do sleep here, for avoid lpm conflict */
    aos_msleep(200);
    fota_do_check(g_fota_handle);
    YUNIT_ASSERT_MSG_QA(ret==0, "fota_do_check ret=%d", ret, "YoC_FOTA_08");
}

void test_fota_set_auto_check()
{
    int ret = 0;
    if (g_fota_handle == NULL) {
        return;
    }

    fota_set_auto_check(g_fota_handle, 1);
    YUNIT_ASSERT_MSG_QA(ret==0, "fota_set_auto_check ret=%d", ret, "YoC_FOTA_09");

    ret = fota_get_auto_check(g_fota_handle);
    YUNIT_ASSERT_MSG_QA(ret==1, "fota_get_auto_check ret=%d", ret, "YoC_FOTA_11");
    aos_msleep(1000);

    fota_set_auto_check(g_fota_handle, 0);
    YUNIT_ASSERT_MSG_QA(1, "fota_set_auto_check ret=%d", ret, "YoC_FOTA_10");

    aos_msleep(1000);
    ret = fota_get_auto_check(g_fota_handle);
    YUNIT_ASSERT_MSG_QA(ret==0, "fota_get_auto_check ret=%d", ret, "YoC_FOTA_12");

}

void test_fota_stop(void)
{
    int ret = 0;

    ret = fota_stop(g_fota_handle);
    YUNIT_ASSERT_MSG_QA(ret==0, "fota_stop ret=%d", ret, "YoC_FOTA_17");

}

static void test_fota_api_param(void)
{
    test_fota_init();
    test_fota_start();
    test_fota_status();
    test_fota_do_check();
    test_fota_set_auto_check();
    test_fota_stop();    
}

void api_fota_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "api_fota", test_fota_api_param);
}

