/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>

static rt_timer_t timer1;
static rt_timer_t timer2;
static int cnt = 0;

static void timeout1(void *parameter)
{
    rt_kprintf("periodic timer is timeout %d\n", cnt);

    if (cnt++ >= 9) {
        rt_timer_stop(timer1);
        rt_timer_delete(timer1);
        rt_kprintf("periodic timer was stopped! \n");
    }
}

static void timeout2(void *parameter)
{
    rt_timer_delete(timer2);
    rt_kprintf("one shot timer is timeout\n");
}

int timer_example_main(void)
{
    /* create timer #1, periodic mode */
    timer1 = rt_timer_create("timer1", timeout1,
                             RT_NULL, 10,
                             RT_TIMER_FLAG_PERIODIC);

    if (timer1 != RT_NULL)
        rt_timer_start(timer1);

    /* create timer #2, one-shot mode */
    timer2 = rt_timer_create("timer2", timeout2,
                             RT_NULL,  30,
                             RT_TIMER_FLAG_ONE_SHOT);
    if (timer2 != RT_NULL)
        rt_timer_start(timer2);

    return 0;
}