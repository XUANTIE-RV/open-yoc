/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "timer_test.h"


static volatile uint32_t timeout_flag = 0;
static volatile uint64_t callback_time = 0;


static void timer_event_cb_reload_fun(csi_timer_t *timer_handle, void *arg)
{
    if (!timeout_flag) {
        timeout_flag = 1;
        callback_time = csi_tick_get_us();
    }
}


int test_timer_precision(char *args)
{
    csi_error_t ret;
    csi_timer_t timer_handle;
    test_timer_args_t r_timer_args;
    uint32_t get_data[2];
    uint64_t real_time;
    uint32_t diff_time;
    uint64_t start_time = 0;

    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    r_timer_args.timer_idx = get_data[0];
    r_timer_args.timeout = get_data[1];
    TEST_CASE_TIPS("test timer idx is %d, timeout %d us", r_timer_args.timer_idx, r_timer_args.timeout);


    ret = csi_timer_init(&timer_handle, r_timer_args.timer_idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer init error");

    timeout_flag = 0;

    ret = csi_timer_attach_callback(&timer_handle, timer_event_cb_reload_fun, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer attach callback error");

    ret = csi_timer_start(&timer_handle, r_timer_args.timeout);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer start error");

    start_time = csi_tick_get_us();

    while (!timeout_flag) {
        tst_mdelay(1);
    }

    real_time = callback_time - start_time;
    TEST_CASE_TIPS("real_time:%lld us", real_time);

    if (real_time > r_timer_args.timeout) {
        diff_time = real_time -  r_timer_args.timeout;
    } else {
        diff_time = r_timer_args.timeout - real_time;
    }

    TEST_CASE_TIPS("diff_time:%d us", diff_time);
    TEST_CASE_ASSERT(diff_time < r_timer_args.timeout * 0.1, "the precision of timer is over 10%");

    csi_timer_stop(&timer_handle);
    csi_timer_detach_callback(&timer_handle);
    csi_timer_uninit(&timer_handle);

    timeout_flag = 0;

    return 0;
}