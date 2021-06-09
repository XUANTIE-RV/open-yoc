/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <wdt_test.h>

int test_wdt_startStop(char *args)
{
    csi_wdt_t hd;
    csi_error_t ret_sta;

    int ret;
    test_wdt_args_t td;
    uint32_t get_data[1];

    ret = args_parsing(args, get_data, 1);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    td.dev_idx = (uint8_t)get_data[0];

    TEST_CASE_TIPS("test WDT idx is %d", td.dev_idx);

    ret_sta = csi_wdt_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_set_timeout(&hd, 100);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d set timeout error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

///<循环调用csi_wdt_start和csi_wdt_stop接口，是对WDT进行多次启停测试
    for (int i = 0; i < 10; i++) {
        tst_mdelay(50);
        ret_sta = csi_wdt_start(&hd);
        TEST_CASE_ASSERT(ret_sta == CSI_OK, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
    }

    for (int i = 0; i < 10; i++) {
        csi_wdt_stop(&hd);
        tst_mdelay(200);
        ret_sta = csi_wdt_start(&hd);
        TEST_CASE_ASSERT(ret_sta == CSI_OK, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
        tst_mdelay(50);
    }

///<调用两次csi_wdt_stop接口，是对WDT进行在不调用csi_wdt_start接口情况下，多次停止测试
    csi_wdt_stop(&hd);
    csi_wdt_stop(&hd);

    csi_wdt_uninit(&hd);


    ret_sta = csi_wdt_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_set_timeout(&hd, 100);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d set timeout error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_attach_callback(&hd, NULL, NULL);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d attach callback error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

///<循环调用csi_wdt_start和csi_wdt_stop接口，是对WDT进行多次启停测试
    for (int i = 0; i < 10; i++) {
        tst_mdelay(50);
        ret_sta = csi_wdt_start(&hd);
        TEST_CASE_ASSERT(ret_sta == CSI_OK, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
    }

    for (int i = 0; i < 10; i++) {
        csi_wdt_stop(&hd);
        tst_mdelay(200);
        ret_sta = csi_wdt_start(&hd);
        TEST_CASE_ASSERT(ret_sta == CSI_OK, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
        tst_mdelay(50);
    }

    csi_wdt_stop(&hd);
    csi_wdt_stop(&hd);///<调用两次csi_wdt_stop接口，是对WDT进行在不调用csi_wdt_start接口情况下，多次停止测试

    csi_wdt_detach_callback(&hd);

    csi_wdt_uninit(&hd);

    return 0;
}
