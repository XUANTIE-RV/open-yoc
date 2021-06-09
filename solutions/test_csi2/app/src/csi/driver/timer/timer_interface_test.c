/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "timer_test.h"


int test_timer_interface(char *args)
{
    csi_error_t ret;
    uint32_t load_time = 0;
    uint32_t remaining_time = 0;
    bool is_running = false;

    ret = csi_timer_init(NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_timer_init interface test error,should return CSI_ERROR,but returned %d.", ret);

    ret  = csi_timer_attach_callback(NULL, NULL, NULL);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_timer_attach_callback interface test error,should return CSI_ERROR,but returned %d.", ret);

    ret = csi_timer_start(NULL, 0);
    TEST_CASE_ASSERT(ret == CSI_ERROR, "csi_timer_start interface test error,should return CSI_ERROR,but returned %d.", ret);

    load_time = csi_timer_get_load_value(NULL);
    TEST_CASE_ASSERT(load_time == 0, "csi_timer_get_load_value interface test error,should return 0,but returned %d.", load_time);

    remaining_time = csi_timer_get_remaining_value(NULL);
    TEST_CASE_ASSERT(remaining_time == 0, "csi_timer_get_remaining_value interface test error,should return 0,but returned %d.", remaining_time);

    is_running = csi_timer_is_running(NULL);
    TEST_CASE_ASSERT(is_running == 0, "csi_timer_is_running interface test error,should return 0,but returned %d.", is_running);

    csi_timer_stop(NULL);

    csi_timer_detach_callback(NULL);

    csi_timer_uninit(NULL);

    return 0;
}
