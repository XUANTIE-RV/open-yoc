/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     tick.c
 * @brief    Source File for tick
 * @version  V1.0
 * @date     7. April 2020
 ******************************************************************************/

#include <soc.h>
#include <csi_config.h>
#include <sys_clk.h>
#include <board_config.h>
#include <drv/common.h>
#include <drv/irq.h>
#include <drv/tick.h>
#include <drv/porting.h>
#include <drv/timer.h>

#ifndef CONFIG_TICK_TIMER_IDX
#define CONFIG_TICK_TIMER_IDX   0U
#endif

extern void krhino_tick_proc(void);
extern void xPortSysTickHandler(void);
extern void OSTimeTick(void);

static csi_timer_t tick_timer;

static volatile uint32_t csi_tick = 0U;
static volatile uint32_t last_time_ms = 0U;
static volatile uint64_t last_time_us = 0U;

void csi_tick_increase(void)
{
    csi_tick++;
}

static void tick_event_cb(csi_timer_t *timer_handle, void *arg)
{
    csi_tick_increase();
#if defined(CONFIG_KERNEL_RHINO)
    krhino_tick_proc();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif
}

csi_error_t csi_tick_init(void)
{
    csi_error_t ret;

    csi_tick = 0U;
    ret = csi_timer_init(&tick_timer, CONFIG_TICK_TIMER_IDX);

    if (ret == CSI_OK) {
        ret = csi_timer_attach_callback(&tick_timer, tick_event_cb, NULL);

        if (ret == CSI_OK) {
            ret = csi_timer_start(&tick_timer, (1000000U / CONFIG_SYSTICK_HZ));
        }
    }

    return ret;
}

void csi_tick_uninit(void)
{
    csi_timer_stop(&tick_timer);
    csi_timer_uninit(&tick_timer);
}

uint32_t csi_tick_get(void)
{
    return csi_tick;
}

uint32_t csi_tick_get_ms(void)
{
    uint32_t time = last_time_ms, freq;
    freq = csi_timer_get_load_value(&tick_timer) * CONFIG_SYSTICK_HZ;

    while (freq) {
        time = (csi_tick * (1000U / CONFIG_SYSTICK_HZ)) + ((csi_timer_get_load_value(&tick_timer) - csi_timer_get_remaining_value(&tick_timer)) / (freq / 1000U));

        if (time >= last_time_ms) {
            break;
        }
    }

    last_time_ms = time;
    return time;
}

uint64_t csi_tick_get_us(void)
{
    uint64_t time, freq;
    uint32_t temp;
    freq = soc_get_timer_freq(CONFIG_TICK_TIMER_IDX);

    while (1) {
        /* the time of coretim pass */
        temp = csi_timer_get_load_value(&tick_timer) - csi_timer_get_remaining_value(&tick_timer);
        time = ((uint64_t)temp * 1000U) / (freq / 1000U);
        /* the time of csi_tick */
        time += ((uint64_t)csi_tick * (1000000U / CONFIG_SYSTICK_HZ));

        if (time >= last_time_us) {
            break;
        }
    }

    last_time_us = time;
    return time;
}

static void _mdelay(void)
{
    uint32_t load = csi_timer_get_load_value(&tick_timer);
    uint32_t start_r = csi_timer_get_remaining_value(&tick_timer);
    uint32_t cur_r;
    uint32_t cnt   = (soc_get_timer_freq(CONFIG_TICK_TIMER_IDX) / 1000U);

    while (1) {
        cur_r = csi_timer_get_remaining_value(&tick_timer);

        if (start_r > cur_r) {
            if ((start_r - cur_r) >= cnt) {
                break;
            }
        } else {
            if (((load - cur_r) + start_r) >= cnt) {
                break;
            }
        }
    }
}

#if defined(CONFIG_KERNEL_RHINO)
#include <k_api.h>
#define RHINO_OS_MS_PERIOD_TICK      (1000 / RHINO_CONFIG_TICKS_PER_SECOND)
#elif defined(CONFIG_KERNEL_FREERTOS)
#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#endif

__WEAK void mdelay(uint32_t ms)
{
#if defined(CONFIG_KERNEL_RHINO)
    extern tick_t     g_tick_count;

    if (g_tick_count > 0) {
        extern kstat_t krhino_task_sleep(tick_t dly);
        uint32_t ms_get = ms;

        if ((ms < RHINO_OS_MS_PERIOD_TICK) && (ms != 0)) {
            ms_get = RHINO_OS_MS_PERIOD_TICK;
        }

        uint64_t ticks = (uint64_t)ms_get / RHINO_OS_MS_PERIOD_TICK;
        krhino_task_sleep(ticks);
    } else {
        while (ms) {
            ms--;
            _mdelay();
        }
    }

#elif defined(CONFIG_KERNEL_FREERTOS)
    extern UBaseType_t uxTaskGetNumberOfTasks(void);
    uint32_t task_num = (uint32_t)uxTaskGetNumberOfTasks();

    if (task_num > 0) {
        extern void vTaskDelay(const TickType_t xTicksToDelay);
#if ( INCLUDE_vTaskDelay == 1 )
        uint32_t ms_get = ms;

        if ((ms != 0) && (ms < portTICK_PERIOD_MS)) {
            ms_get = portTICK_PERIOD_MS;
        }

        uint64_t ticks = (uint64_t)ms_get / portTICK_PERIOD_MS;
        vTaskDelay(ticks);
    } else {
        while (ms) {
            ms--;
            _mdelay();
        }
    }

#endif
#else

    while (ms) {
        ms--;
        _mdelay();
    }

#endif

}


static void _10udelay(void)
{
    uint32_t load = csi_timer_get_load_value(&tick_timer);
    uint32_t start_r = csi_timer_get_remaining_value(&tick_timer);
    uint32_t cur_r;
    uint32_t cnt   = (soc_get_timer_freq(CONFIG_TICK_TIMER_IDX) / 100000U);

    while (1) {
        cur_r = csi_timer_get_remaining_value(&tick_timer);

        if (start_r > cur_r) {
            if ((start_r - cur_r) >= cnt) {
                break;
            }
        } else {
            if (((load - cur_r) + start_r) >= cnt) {
                break;
            }
        }
    }
}

/**
 * Ps: At least delay over 10us
*/
void udelay(uint32_t us)
{
    us /= 10U;

    while (us) {
        us--;
        _10udelay();
    }
}

