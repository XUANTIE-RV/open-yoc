/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <wdt_test.h>

int test_wdt_getInfo(char *args)
{
    csi_wdt_t hd;
    csi_error_t ret_sta;
    bool ret_bool;

    int ret;
    test_wdt_args_t td;
    uint32_t get_data[1];

    ret = args_parsing(args, get_data, 1);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");


    td.dev_idx = (uint8_t)get_data[0];

    TEST_CASE_TIPS("test WDT idx is %d", td.dev_idx);


    ret_sta = csi_wdt_init(&hd, td.dev_idx);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d init error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_sta = csi_wdt_set_timeout(&hd, 2000);
    TEST_CASE_ASSERT_QUIT(ret_sta == CSI_OK, "wdt %d set timeout error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);

    ret_bool = csi_wdt_is_running(&hd);
    TEST_CASE_ASSERT(ret_bool == false, "wdt %d get status error,should return FALSE,but returned TRUE.", td.dev_idx);

    ret_sta = csi_wdt_start(&hd);

    if (ret_sta != 0) {
        csi_wdt_uninit(&hd);
        TEST_CASE_ASSERT_QUIT(1 == 0, "wdt %d start error,should return CSI_OK,but returned %d.", td.dev_idx, ret_sta);
    }

    ret_bool = csi_wdt_is_running(&hd);
    TEST_CASE_ASSERT(ret_bool == true, "wdt %d get status error,should return TRUE,but returned FALSE.", td.dev_idx);

    csi_wdt_stop(&hd);

    ret_bool = csi_wdt_is_running(&hd);
    TEST_CASE_ASSERT(ret_bool == false, "wdt %d get status error,should return FALSE,but returned TRUE.", td.dev_idx);

    csi_wdt_uninit(&hd);

    return 0;
}
