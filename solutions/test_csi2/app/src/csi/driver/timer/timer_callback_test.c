/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "timer_test.h"

#define CALLBACK_PARA 2
static void timer_event_cb(csi_timer_t *timer, void *arg)
{

    TEST_CASE_ASSERT(*((uint8_t *) arg) == CALLBACK_PARA, "timer callback error");
    *((uint8_t *) arg) = 0;

}

int test_timer_callback(char *args)
{
    csi_error_t ret;
    csi_timer_t timer_handle;
    test_timer_args_t r_timer_args;
    uint32_t get_data[2];
    uint32_t delay_time;
    uint8_t test_par = CALLBACK_PARA;


    ret = args_parsing(args, get_data, 2);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");

    r_timer_args.timer_idx = get_data[0];
    r_timer_args.timeout = get_data[1];
    TEST_CASE_TIPS("test timer idx is %d, timeout %d us", r_timer_args.timer_idx, r_timer_args.timeout);


    ret = csi_timer_init(&timer_handle, r_timer_args.timer_idx);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer init error");

    ret = csi_timer_attach_callback(&timer_handle, timer_event_cb, (void *)&test_par);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer attach callback error");

    ret = csi_timer_start(&timer_handle, r_timer_args.timeout);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer start error");

    delay_time = r_timer_args.timeout / 1000 + 1;
    tst_mdelay(delay_time);
    TEST_CASE_ASSERT(test_par == 0, "timer attach callback parameter error");

    test_par = CALLBACK_PARA;
    csi_timer_detach_callback(&timer_handle);

    delay_time = r_timer_args.timeout / 1000 + 1;
    tst_mdelay(delay_time);
    TEST_CASE_ASSERT(test_par == 2, "timer detach callback parameter error");

    csi_timer_stop(&timer_handle);
    csi_timer_uninit(&timer_handle);

    return 0;
}