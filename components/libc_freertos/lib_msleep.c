/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>

__attribute__((weak)) uint32_t msleep(uint32_t ms)
{
    if (ms) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    }

    return 0;
}

