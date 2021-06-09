/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <intnest_test.h>

static csi_timer_t timer0, timer1, timer2;
static uint8_t a = 0;
static uint8_t b = 0;

static void timer0_cb(csi_timer_t *timer, void *arg)
{

    (void)arg;
    csi_timer_stop(&timer0);
    a |= 0x01;

    tst_mdelay(800);

    a &= ~0x01;
}

static void timer1_cb(csi_timer_t *timer, void *arg)
{
    (void)arg;
    csi_timer_stop(&timer1);
    a |= 0x02;

    if (a & 0x01) {
        b |= 0x01;
    }

    tst_mdelay(600);

    a &= ~0x02;
}


static void timer2_cb(csi_timer_t *timer, void *arg)
{
    (void)arg;
    csi_timer_stop(&timer2);
    a |= 0x04;

    if ((a & 0x03) == 0x03) {
        b |= 0x02;
    }

    tst_mdelay(400);

    a &= ~0x04;
}


int test_intnest_timer(char *args)
{
    csi_error_t ret;
    uint32_t get_data[3];
    uint32_t prio[3];

    ret = args_parsing(args, get_data, 3);
    TEST_CASE_ASSERT_QUIT(ret == 0, "num_of_parameter error");
    memcpy(prio, get_data, sizeof(get_data));
    TEST_CASE_TIPS("timer0 prio = %d, timer1 prio = %d, timer2 prio = %d ", prio[0], prio[1], prio[2]);

    csi_vic_set_prio(prio[0], 3);
    csi_vic_set_prio(prio[1], 2);
    csi_vic_set_prio(prio[2], 1);

    ret = csi_timer_init(&timer0, 0);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer0 init error");
    ret = csi_timer_attach_callback(&timer0, timer0_cb, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer0 attach callback error");
    ret = csi_timer_start(&timer0, 100000);
    TEST_CASE_ASSERT(ret == CSI_OK, "start timer0 error");

    ret = csi_timer_init(&timer1, 1);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer1 init error");
    ret = csi_timer_attach_callback(&timer1, timer1_cb, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer1 attach callback error");
    ret = csi_timer_start(&timer1, 150000);
    TEST_CASE_ASSERT(ret == CSI_OK, "start timer1 error");

    ret = csi_timer_init(&timer2, 2);
    TEST_CASE_ASSERT_QUIT(ret == CSI_OK, "timer2 init error");
    ret = csi_timer_attach_callback(&timer2, timer2_cb, NULL);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer2 attach callback error");
    ret = csi_timer_start(&timer2, 200000);
    TEST_CASE_ASSERT(ret == CSI_OK, "start timer2 error");


    tst_mdelay(2000);
    TEST_CASE_ASSERT(0x03 == b, "timer0 interrupt,timer1 interrupt and timer2 interrupt doesn't work");

    csi_vic_set_prio(prio[0], 2);
    csi_vic_set_prio(prio[1], 3);

    ret = csi_timer_start(&timer0, 100000);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer0 start error");
    ret = csi_timer_start(&timer1, 150000);
    TEST_CASE_ASSERT(ret == CSI_OK, "timer1 start error");

    b = 0;
    tst_mdelay(2000);
    TEST_CASE_ASSERT(0x00 == b, "timer0,timer1 or timer2 interrupt work");

    csi_timer_stop(&timer0);
    csi_timer_detach_callback(&timer0);
    csi_timer_uninit(&timer0);

    csi_timer_stop(&timer1);
    csi_timer_detach_callback(&timer1);
    csi_timer_uninit(&timer1);

    csi_timer_stop(&timer2);
    csi_timer_detach_callback(&timer2);
    csi_timer_uninit(&timer2);

    return 0;
}
