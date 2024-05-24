/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>

__attribute__((weak)) int g_fr_next_sleep_ticks = -1;
/* used by chip component(without aos-osal) */
int freertos_intrpt_enter(void)
{
    return 0;
}

int freertos_intrpt_exit(void)
{
    portYIELD_FROM_ISR(pdTRUE);
    return 0;
}


