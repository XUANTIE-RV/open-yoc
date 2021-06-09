/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "rtc_test.h"

int test_rtc_interface(char *args)
{
    uint32_t rt = 0;
    csi_error_t rtc_ret;
    csi_rtc_time_t rtctime;
    csi_rtc_t *rtc_handler;

    rtc_ret = csi_rtc_init(NULL, 0);
    TEST_CASE_ASSERT(rtc_ret == CSI_ERROR, "csi_rtc_init test error,should return %d, but returned %d", CSI_ERROR, rtc_ret);

    rtc_ret = csi_rtc_init(rtc_handler, 0);
    TEST_CASE_ASSERT(rtc_ret == CSI_OK, "csi_rtc_init test error,should return %d, but returned %d", CSI_OK, rtc_ret);

    rtc_ret = csi_rtc_set_time(NULL, NULL);
    TEST_CASE_ASSERT(rtc_ret == CSI_ERROR, "csi_rtc_set_time test error,should return %d, but returned %d", CSI_ERROR, rtc_ret);

    rtc_ret = csi_rtc_set_time_no_wait(NULL, NULL);
    TEST_CASE_ASSERT(rtc_ret == CSI_ERROR, "csi_rtc_set_time_no_wait test error,should return %d, but returned %d", CSI_ERROR, rtc_ret);

    rtc_ret = csi_rtc_get_time(NULL, &rtctime);
    TEST_CASE_ASSERT(rtc_ret == CSI_ERROR, "csi_rtc_get_time test error,should return %d, but returned %d", CSI_ERROR, rtc_ret);

    rt = csi_rtc_get_alarm_remaining_time(NULL);
    TEST_CASE_ASSERT(rt == 0, "csi_rtc_get_alarm_remaining_time test error,should return %d, but returned %d", 0, rt);

    rtc_ret = csi_rtc_set_alarm(NULL, &rtctime, NULL, NULL);
    TEST_CASE_ASSERT(rtc_ret == CSI_ERROR, "csi_rtc_set_alarm test error,should return %d, but returned %d", CSI_ERROR, rtc_ret);

    rtc_ret = csi_rtc_cancel_alarm(NULL);
    TEST_CASE_ASSERT(rtc_ret == CSI_ERROR, "csi_rtc_cancel_alarm test error,should return %d, but returned %d", CSI_ERROR, rtc_ret);

    TEST_CASE_ASSERT(csi_rtc_is_running(NULL) == false, "csi_rtc_is_running test error,should return %d, but returned %d", false, true);

    csi_rtc_uninit(NULL);

    csi_rtc_uninit(rtc_handler);
    return 0;
}