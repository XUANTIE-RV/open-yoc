/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <tick_test.h>

int test_tick_func(char *args)
{
    uint32_t ret = 0;
    uint32_t tick1 = 0, tick2 = 0, time_ms = 0;
    uint64_t time_us = 0;

    tst_mdelay(1000);
    TEST_CASE_TIPS("Time stamp1,tick begins to init");

    ret = csi_tick_init();

    TEST_CASE_ASSERT(ret == CSI_OK, "tick init error");

    tick1 = csi_tick_get();
    csi_tick_increase();
    tick2 = csi_tick_get();
    TEST_CASE_ASSERT(tick1 < tick2, "tick2 should be bigger than tick1");

    tst_mdelay(1000);
    time_ms = csi_tick_get_ms();
    TEST_CASE_TIPS("Time stamp2,time_ms is %u", time_ms);
    TEST_CASE_ASSERT(time_ms != 0, "get tick(ms) error");
    time_us = csi_tick_get_us();
    TEST_CASE_ASSERT(time_us != 0, "get tick(us) error");
    TEST_CASE_ASSERT((time_ms * 1000) < time_us, "get tick error");

    csi_tick_uninit();

    return 0;
}
