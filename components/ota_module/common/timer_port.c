/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <aos/aos.h>
#include "timer_port.h"
#include "ulog/ulog.h"

#define TAG "OTA_TIMER"

int ota_timer_init(ota_timer_t *timer, ota_timer_handler_t handle, void *args)
{
    int ret = 0;
    ret     = aos_timer_new_ext(&timer->timer.aos_timer, handle, args, 1000, 0, 0);
    if (ret) {
        LOGE(TAG, "Aos timer create failed %d", ret);
    }
    return ret;
}

int ota_timer_start(ota_timer_t *timer, uint32_t timeout)
{
    int ret = 0;
    ret     = aos_timer_change_once(&timer->timer.aos_timer, timeout);
    if (ret) {
        LOGE(TAG, "Aos timer change failed %d", ret);
        return ret;
    }

    return aos_timer_start(&timer->timer.aos_timer);
}

int ota_timer_stop(ota_timer_t *timer)
{
    int ret = 0;
    ret     = aos_timer_stop(&timer->timer.aos_timer);
    if (ret) {
        LOGE(TAG, "Aos timer stop failed %d", ret);
    }
    return ret;
}

void ota_timer_free(ota_timer_t *timer)
{
    aos_timer_stop(&timer->timer.aos_timer);
    aos_timer_free(&timer->timer.aos_timer);
}
