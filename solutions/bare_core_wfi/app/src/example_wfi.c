/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <csi_core.h>
#include <drv/tick.h>
#include <drv/timer.h>
#include "soc.h"

#if CONFIG_CPU_E9XX
#define TIMER_IDX  (0)
#define TIMEOUT    (3000000)

static csi_timer_t g_tick_timer0;
static volatile int g_tick_cnt0 = 0;

static void _tick_event_cb0(csi_timer_t *timer, void *arg)
{
    printf("wake up by interrupt\r\n");
    g_tick_cnt0++;
}

int example_core_wfi()
{
    csi_error_t ret;

    /* coret in clint may be trigger wfi on irq disabled. disable it first */
    csi_tick_uninit();

    ret = csi_timer_init(&g_tick_timer0, TIMER_IDX);
    if (ret != CSI_OK)
        goto error;

    csi_timer_attach_callback(&g_tick_timer0, _tick_event_cb0, NULL);
    ret = csi_timer_start(&g_tick_timer0, TIMEOUT);
    if (ret != CSI_OK)
        goto error;

    printf("execute wfi and enter standby mode:\n");
    __WFI();
    while (!g_tick_cnt0);
    csi_timer_stop(&g_tick_timer0);
    csi_timer_uninit(&g_tick_timer0);

    printf("bare_core_wfi runs success!\n");
    return 0;
error:
    printf("bare_core_wfi runs failed\r\n");
    return -1;
}
#endif /* end exx */


