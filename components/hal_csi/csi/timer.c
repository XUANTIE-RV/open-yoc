/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include "soc.h"
#include "aos/hal/timer.h"
#include "drv/timer.h"

typedef void *timer_handler_t;

typedef struct {
    timer_handler_t  csi_tim_handle;
    timer_dev_t     *hal_tim_handle;
} timer_handlers_t;

static timer_handlers_t  timer_handlers[CONFIG_TIMER_NUM] = {NULL};

typedef struct {
    int32_t idx;
    timer_event_e event;
} timer_callback_t;

static void hal_timer_cb_func(int32_t idx, timer_event_e event)
{
    timer_callback_t hal_tim_cb;

    hal_tim_cb.idx = idx;
    hal_tim_cb.event = event;

    if (TIMER_EVENT_TIMEOUT == event) {
        timer_handlers[idx].hal_tim_handle->config.cb(&hal_tim_cb);
    }
}

int32_t hal_timer_init(timer_dev_t *tim)
{
    if (!tim) {
        return -1;
    }

    timer_handler_t tim_handle = NULL;
    tim_handle = csi_timer_initialize(tim->port, hal_timer_cb_func);

    if (!tim_handle) {
        return -1;
    }

    timer_handlers[tim->port].csi_tim_handle = tim_handle;
    timer_handlers[tim->port].hal_tim_handle = tim;
    csi_timer_set_timeout(tim_handle, tim->config.period);

    switch (tim->config.reload_mode) {
        case (TIMER_RELOAD_AUTO):
            csi_timer_config(tim_handle, TIMER_MODE_RELOAD);
            break;

        case (TIMER_RELOAD_MANU):
            csi_timer_config(tim_handle, TIMER_MODE_FREE_RUNNING);
            break;

        default:
            csi_timer_config(tim_handle, TIMER_MODE_RELOAD);
            break;
    }

    return 0;
}

/**
 * start a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_start(timer_dev_t *tim)
{
    if (tim == NULL) {
        return -1;
    }

    if (!timer_handlers[tim->port].csi_tim_handle) {
        return -1;
    }

    return csi_timer_start(timer_handlers[tim->port].csi_tim_handle);
}

/**
 * stop a hardware timer
 *
 * @param[in]  tim  timer device
 *
 * @return  none
 */
void hal_timer_stop(timer_dev_t *tim)
{
    if (tim == NULL) {
        return;
    }

    if (!timer_handlers[tim->port].csi_tim_handle) {
        return;
    }

    csi_timer_stop(timer_handlers[tim->port].csi_tim_handle);
}

/**
 * change the config of a hardware timer
 *
 * @param[in]  tim   timer device
 * @param[in]  para  timer config
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_para_chg(timer_dev_t *tim, timer_config_t para)
{
    if (tim == NULL) {
        return -1;
    }

    if (!timer_handlers[tim->port].csi_tim_handle) {
        return -1;
    }

    csi_timer_set_timeout(timer_handlers[tim->port].csi_tim_handle, para.period);

    switch (para.reload_mode) {
        case (TIMER_RELOAD_AUTO):
            csi_timer_config(timer_handlers[tim->port].csi_tim_handle, TIMER_MODE_RELOAD);
            break;

        case (TIMER_RELOAD_MANU):
            csi_timer_config(timer_handlers[tim->port].csi_tim_handle, TIMER_MODE_FREE_RUNNING);
            break;

        default:
            csi_timer_config(timer_handlers[tim->port].csi_tim_handle, TIMER_MODE_RELOAD);
            break;
    }

    return csi_timer_start(timer_handlers[tim->port].csi_tim_handle);
}

/**
 * De-initialises an TIMER interface, Turns off an TIMER hardware interface
 *
 * @param[in]  tim  timer device
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_timer_finalize(timer_dev_t *tim)
{
    if (tim == NULL) {
        return -1;
    }

    if (!timer_handlers[tim->port].csi_tim_handle) {
        return -1;
    }

    return csi_timer_uninitialize(timer_handlers[tim->port].csi_tim_handle);
}

