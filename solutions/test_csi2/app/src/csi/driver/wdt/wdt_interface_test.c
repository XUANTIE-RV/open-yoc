/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <wdt_test.h>

int test_wdt_interface(char *args)
{
    csi_error_t ret_sta;
    uint32_t ret_num;
    bool ret_bool;

    ret_sta = csi_wdt_init(NULL, 0);
    TEST_CASE_ASSERT(ret_sta == CSI_ERROR, "csi_wdt_init interface test error,should return CSI_ERROR,but returned %d.", ret_sta);

    csi_wdt_uninit(NULL);

    ret_sta = csi_wdt_set_timeout(NULL, 10);
    TEST_CASE_ASSERT(ret_sta == CSI_ERROR, "csi_wdt_set_timeout interface test error,should return CSI_ERROR,but returned %d.", ret_sta);

    ret_sta = csi_wdt_start(NULL);
    TEST_CASE_ASSERT(ret_sta == CSI_ERROR, "csi_wdt_start interface test error,should return CSI_ERROR,but returned %d.", ret_sta);

    csi_wdt_stop(NULL);

    ret_sta = csi_wdt_feed(NULL);
    TEST_CASE_ASSERT(ret_sta == CSI_ERROR, "csi_wdt_feed interface test error,should return CSI_ERROR,but returned %d.", ret_sta);

    ret_num = csi_wdt_get_remaining_time(NULL);
    TEST_CASE_ASSERT(ret_num != 0, "csi_wdt_get_remaining_time interface test error,shouldn't return 0,but returned %d.", ret_num);

    ret_bool = csi_wdt_is_running(NULL);
    TEST_CASE_ASSERT(ret_bool == 1, "csi_wdt_is_running interface test error,should return True,but returned %d.", ret_bool);

    ret_sta = csi_wdt_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret_sta == CSI_ERROR, "csi_wdt_attach_callback interface test error,should return CSI_ERROR,but returned %d.", ret_sta);

    csi_wdt_detach_callback(NULL);

    return 0;
}
