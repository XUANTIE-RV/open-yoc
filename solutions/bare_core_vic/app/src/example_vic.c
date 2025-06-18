 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <csi_core.h>
#include <drv/timer.h>
#include "soc.h"
#include <board.h>

#define TIMER0         EXAMPLE_TIMER_IDX
#define TIMER0_IRQn    TIM0_IRQn

#define TIMER1         (EXAMPLE_TIMER_IDX + 1)
#define TIMER1_IRQn    TIM1_IRQn

#define HI_PRIO   3
#define LO_PRIO   2

#define EXAMPLE_LOOP_CNT    1
#define TIMEOUT    1000000

static csi_timer_t g_tick_timer0;
static csi_timer_t g_tick_timer1;

#if CONFIG_SUPPORT_IRQ_NESTED
static volatile int endless_loop_cnt = 0;
#endif
static volatile int g_tick_cnt0 = 0;
static volatile int g_tick_cnt1 = 0;

extern void mdelay(uint32_t ms);
static void _tick_event_cb0(csi_timer_t *timer, void *arg)
{
    int idx = timer->dev.idx;

#if CONFIG_SUPPORT_IRQ_NESTED
    /*  for irq nested on exx */
    mdelay(100);
    printf("I am timer %d's callback in an endless loop.\n", idx);

    while (1) {
        if (endless_loop_cnt >= 1) {
            break;
        }
    }
#else
    printf("I am timer %d's callback.\n", idx);
#endif
    g_tick_cnt0++;
}

static void _tick_event_cb1(csi_timer_t *timer, void *arg)
{
    int idx = timer->dev.idx;

#if CONFIG_SUPPORT_IRQ_NESTED
    if (!endless_loop_cnt) {
        endless_loop_cnt++;
        printf("I am timer %d's with higher interrupt priority than timer %d.\n", idx, TIMER0);
    }
#else
    printf("I am timer %d's callback.\n", idx);
#endif
    g_tick_cnt1++;
}

int example_core_vic()
{
    csi_error_t ret;

    ret = csi_timer_init(&g_tick_timer0, TIMER0);
    if (ret == CSI_OK) {
        ret = csi_timer_attach_callback(&g_tick_timer0, _tick_event_cb0, NULL);
    }
    if (ret != CSI_OK)
        goto error;

    printf("set timer %d 's interrrupt priority with %d (lower priority).\n", TIMER0, LO_PRIO);
    // csi_timer_init will get irq_num
    csi_vic_set_prio(g_tick_timer0.dev.irq_num, LO_PRIO);

    ret = csi_timer_init(&g_tick_timer1, TIMER1);
    if (ret == CSI_OK) {
        ret = csi_timer_attach_callback(&g_tick_timer1, _tick_event_cb1, NULL);
    }
    if (ret != CSI_OK)
        goto error;

    printf("initialize timer %d, set it's interrupt priority : %d (higher priority).\n", TIMER1, HI_PRIO);
    // csi_timer_init will get irq_num
    csi_vic_set_prio(g_tick_timer1.dev.irq_num, HI_PRIO);

#if CONFIG_SUPPORT_IRQ_NESTED
    printf("start reload timer %d with lower interrupt priority, and it will enter an endless loop callback\n", TIMER0);
#endif
    ret = csi_timer_start(&g_tick_timer0, TIMEOUT);
    if (ret != CSI_OK)
        goto error;

    printf("start reload mode timer %d with higher interrupt priority.\n", TIMER1);
    ret = csi_timer_start(&g_tick_timer1, TIMEOUT);
    if (ret != CSI_OK)
        goto error;

    while (!(g_tick_cnt0 > 0 && g_tick_cnt1 > 0));
    csi_timer_stop(&g_tick_timer0);
    csi_timer_stop(&g_tick_timer1);
    csi_timer_uninit(&g_tick_timer0);
    csi_timer_uninit(&g_tick_timer1);

    printf("bare_core_vic runs success!\n");
    return 0;
error:
    printf("bare_core_vic runs failed\r\n");
    return -1;
}

