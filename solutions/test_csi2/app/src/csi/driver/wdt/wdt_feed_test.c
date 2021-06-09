/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <wdt_test.h>

static void wdt_callback(csi_wdt_t *wdt,  void *arg)
{
    *(uint8_t *)arg = 0;
}

int test_wdt_queryFeed(char *args)
{
    csi_wdt_t hd;
    csi_error_t ret_sta;
    uint32_t volatile ret_num;

    int ret;
    test_wdt_args_t td;
    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)get_data[0];
    td.timeout_ms = (uint32_t)get_data[1];
    td.wait_timeout_ms = (uint32_t)get_data[1];

    ret_sta = csi_wdt_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_set_timeout(&hd, td.timeout_ms);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d set timeout error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_start(&hd);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    for (uint32_t i = 0; i < 100; i++) {
        do {
            ret_num = csi_wdt_get_remaining_time(&hd);
        } while (ret_num > 10);

        ret_sta = csi_wdt_feed(&hd);

        if (ret_sta != 0) {
            TEST_CASE_ASSERT(1 == 0, "wdt %d feed error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
            break;
        }
    }

    csi_wdt_stop(&hd);

    tst_mdelay(td.wait_timeout_ms);

    csi_wdt_uninit(&hd);

    return 0;
}

int test_wdt_interruptFeed(char *args)
{
    csi_wdt_t hd;
    csi_error_t ret_sta;
    volatile uint8_t lock = 0;

    int ret;
    test_wdt_args_t td;
    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)get_data[0];
    td.timeout_ms = (uint32_t)get_data[1];
    td.wait_timeout_ms = (uint32_t)get_data[2];

    ret_sta = csi_wdt_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_set_timeout(&hd, td.timeout_ms);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d set timeout error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_attach_callback(&hd, wdt_callback, (void *)&lock);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d attach callback error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    lock = 1;
    ret_sta = csi_wdt_start(&hd);

    if (ret_sta != 0) {
        csi_wdt_detach_callback(&hd);
        csi_wdt_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
    }

    for (uint32_t i = 0; i < 100; i++) {
        while (lock) ;

        ret_sta = csi_wdt_feed(&hd);

        if (ret_sta != 0) {
            TEST_CASE_ASSERT(1 == 0, "wdt %d feed error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
            break;
        }
    }

    csi_wdt_stop(&hd);

    csi_wdt_detach_callback(&hd);

    tst_mdelay(td.wait_timeout_ms);

    csi_wdt_uninit(&hd);

    return 0;
}

