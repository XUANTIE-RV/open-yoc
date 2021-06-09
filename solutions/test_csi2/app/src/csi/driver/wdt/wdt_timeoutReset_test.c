/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <wdt_test.h>

int test_wdt_timeoutReset(char *args)
{
    csi_wdt_t hd;
    csi_error_t ret_sta;

    int ret;
    test_wdt_args_t td;
    uint32_t get_data[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)get_data[0];
    td.timeout_ms = (uint32_t)get_data[1];
    td.wait_timeout_ms = (uint32_t)get_data[2];

    TEST_CASE_TIPS("test WDT idx is %d", td.dev_idx);
    TEST_CASE_TIPS("config timeout is %d ms", td.timeout_ms);

    ret_sta = csi_wdt_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_set_timeout(&hd, td.timeout_ms);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d set timeout error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_start(&hd);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    tst_mdelay(td.wait_timeout_ms);
    TEST_CASE_ASSERT(1 == 0, "wdt %d timeout reset error", td.dev_idx);

    csi_wdt_stop(&hd);
    csi_wdt_uninit(&hd);

    return 0;
}
