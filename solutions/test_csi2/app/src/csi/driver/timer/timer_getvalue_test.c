/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "timer_test.h"

static volatile uint32_t getvalue_timeout_flag = 0;

static void timer_event_cb_reload_fun(csi_timer_t *timer_handle, void *arg)
{
    if (!getvalue_timeout_flag) {
        getvalue_timeout_flag = 1;
    }
}


int test_timer_getLoadvalue(char *args)
{
    csi_error_t ret;
    csi_timer_t timer_handle;
    test_timer_args_t r_timer_args;
    uint32_t get_data[2];
    int32_t freq;
    uint32_t load_time;

    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    r_timer_args.timer_idx = get_data[0];
    r_timer_args.timeout = get_data[1];
    TEST_CASE_TIPS("test timer idx is %d, timeout %d us", r_timer_args.timer_idx, r_timer_args.timeout);


    ret = csi_timer_init(&timer_handle, r_timer_args.timer_idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer init error");

    ret = csi_timer_start(&timer_handle, r_timer_args.timeout);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer start error");

    freq = soc_get_timer_freq(r_timer_args.timer_idx);
    load_time = csi_timer_get_load_value(&timer_handle);
    TEST_CASE_ASSERT((uint64_t)load_time * 1000000U == (uint64_t)r_timer_args.timeout * freq, "load time timer get is not equal to timeout set");

    csi_timer_stop(&timer_handle);
    csi_timer_uninit(&timer_handle);

    return 0;
}


int test_timer_getRemainingvalue(char *args)
{
    csi_error_t ret;
    csi_timer_t timer_handle;
    test_timer_args_t r_timer_args;
    uint32_t get_data[2];
    int32_t freq;
    uint32_t remaining_value_last;
    uint32_t remaining_value_cur;


    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    r_timer_args.timer_idx = get_data[0];
    r_timer_args.timeout = get_data[1];
    TEST_CASE_TIPS("test timer idx is %d, timeout %d us", r_timer_args.timer_idx, r_timer_args.timeout);


    ret = csi_timer_init(&timer_handle, r_timer_args.timer_idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer init error");

    getvalue_timeout_flag = 0;

    ret = csi_timer_attach_callback(&timer_handle, timer_event_cb_reload_fun, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer attach callback error");

    ret = csi_timer_start(&timer_handle, r_timer_args.timeout);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer start error");

    freq = soc_get_timer_freq(r_timer_args.timer_idx);
    remaining_value_last = csi_timer_get_remaining_value(&timer_handle);

    tst_mdelay(1);

    while (!getvalue_timeout_flag) {
        remaining_value_cur = csi_timer_get_remaining_value(&timer_handle);

        TEST_CASE_ASSERT((remaining_value_cur < remaining_value_last) && ((uint64_t)remaining_value_cur * 1000000U < (uint64_t)r_timer_args.timeout * freq),
                         "timer get remaining value error");
        tst_mdelay(1);
    }

    csi_timer_stop(&timer_handle);
    csi_timer_detach_callback(&timer_handle);
    csi_timer_uninit(&timer_handle);

    getvalue_timeout_flag = 0;

    return 0;
}