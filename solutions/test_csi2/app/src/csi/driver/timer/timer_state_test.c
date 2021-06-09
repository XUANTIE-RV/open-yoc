/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "timer_test.h"

static volatile uint32_t state_timeout_flag = 0;


static void timer_event_cb_reload_fun(csi_timer_t *timer_handle, void *arg)
{
    if (!state_timeout_flag) {
        state_timeout_flag = 1;
    }
}


int test_timer_state(char *args)
{
    csi_error_t ret;
    csi_timer_t timer_handle;
    test_timer_args_t r_timer_args;
    uint32_t delay_time;
    bool state;
    uint32_t get_data[2];

    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    r_timer_args.timer_idx = get_data[0];
    r_timer_args.timeout = get_data[1];
    TEST_CASE_TIPS("test timer idx is %d, timeout %d us", r_timer_args.timer_idx, r_timer_args.timeout);

    ret = csi_timer_init(&timer_handle, r_timer_args.timer_idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer init error");

    state = csi_timer_is_running(&timer_handle);
    TEST_CASE_ASSERT(state == false, "timer gets wrong state,it should be stopped");

    state_timeout_flag = 0;

    ret = csi_timer_attach_callback(&timer_handle, timer_event_cb_reload_fun, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer attach callback error");

    ret = csi_timer_start(&timer_handle, r_timer_args.timeout);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer start error");
    state = csi_timer_is_running(&timer_handle);
    TEST_CASE_ASSERT(state == true, "timer gets wrong state,it should be running");

    delay_time = r_timer_args.timeout / 1000 / 2;
    tst_mdelay(delay_time);
    TEST_CASE_ASSERT(state_timeout_flag == 0, "timer callback called error");

    csi_timer_stop(&timer_handle);
    state = csi_timer_is_running(&timer_handle);
    TEST_CASE_ASSERT(state == false, "timer gets wrong state,it should be stopped");

    ret = csi_timer_start(&timer_handle, r_timer_args.timeout);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer start error");
    state = csi_timer_is_running(&timer_handle);
    TEST_CASE_ASSERT(state == true, "timer gets wrong state,it should be running");

    delay_time = r_timer_args.timeout / 1000 + 1;
    tst_mdelay(delay_time);
    TEST_CASE_ASSERT(state_timeout_flag == 1, "timer callback called error");

    csi_timer_stop(&timer_handle);
    state = csi_timer_is_running(&timer_handle);
    TEST_CASE_ASSERT(state == false, "timer gets wrong state,it should be stopped");

    csi_timer_detach_callback(&timer_handle);
    csi_timer_uninit(&timer_handle);

    state_timeout_flag = 0;

    return 0;
}