/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <csi_core.h>
#include <drv/timer.h>
#include "soc.h"

#if CONFIG_CPU_E9XX
/* connected TIMER4 to nmi-exception on soc bit of smartl, just for nmi test */
#define TIMER4     4
#define TIMEOUT    3000000

static csi_timer_t g_tick_timer4;
static volatile int g_nmi_occur = 0;
extern void (*trap_c_callback)(void);

static void my_trap_c(void)
{
    printf("entry exception callback: my_trap_c\n");
    printf("second: access illegal address 0xf1234568 again, trigger lockup! and wait for nmi!\n");
    __LDRT((unsigned long *)0xf1234568);
}

/* overide nmi-exception handler */
void handle_nmi_exception(void)
{
    printf("nmi has happened\r\n");
    g_nmi_occur++;
    csi_timer_stop(&g_tick_timer4);
    csi_timer_uninit(&g_tick_timer4);
    printf("bare_core_lockup runs success!\n");
}

int example_core_lockup()
{
    csi_error_t ret;

    trap_c_callback = my_trap_c;
    ret = csi_timer_init(&g_tick_timer4, TIMER4);
    if (ret != CSI_OK)
        goto error;

    /* trigger nmi-exception(NO. 24) after 3s */
    ret = csi_timer_start(&g_tick_timer4, TIMEOUT);
    if (ret != CSI_OK)
        goto error;

    __disable_irq();

    printf("first: access illegal address 0xf1234568, trigger exception!\n");
    __LDRT((unsigned long *)0xf1234568);

    printf("bellow code will not execute normally\r\n");
    while (!g_nmi_occur);
    csi_timer_stop(&g_tick_timer4);
    csi_timer_uninit(&g_tick_timer4);

    printf("bare_core_lockup runs success!\n");
    return 0;
error:
    printf("bare_core_lockup runs failed\r\n");
    return -1;
}
#endif /* end exx */

