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

/* overide nmi-exception handler */
void handle_nmi_exception(void)
{
    printf("nmi has happened\r\n");
    g_nmi_occur++;
}

int example_core_nmi()
{
    csi_error_t ret;

    ret = csi_timer_init(&g_tick_timer4, TIMER4);
    if (ret != CSI_OK)
        goto error;

    /* trigger nmi-exception(NO. 24) after 3s */
    ret = csi_timer_start(&g_tick_timer4, TIMEOUT);
    if (ret != CSI_OK)
        goto error;

    __disable_irq();
    while (!g_nmi_occur);
    csi_timer_stop(&g_tick_timer4);
    csi_timer_uninit(&g_tick_timer4);

    printf("bare_core_nmi runs success!\n");
    return 0;
error:
    printf("bare_core_nmi runs failed\r\n");
    return -1;
}
#endif /* end exx */

