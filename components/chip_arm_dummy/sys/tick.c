/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tick.c
 * @brief    Source File for tick
 * @version  V1.1
 * @date     13. April 2020
 ******************************************************************************/

#include <csi_config.h>
#include <sys_clk.h>
#include <csi_core.h>
#include <drv/tick.h>
#include <drv/porting.h>
#include <drv/common.h>
#include <drv/irq.h>
#include <soc.h>

#define __WEAK         __attribute__((weak))

static csi_dev_t tick_dev;

static volatile uint32_t csi_tick = 0U;
static volatile uint32_t last_time_ms = 0U;
static volatile uint64_t last_time_us = 0U;

extern void csi_coret_clear_irq(void);

void csi_tick_increase(void)
{
    csi_tick++;
}

void tick_irq_handler(void *arg)
{
    csi_tick_increase();
    csi_coret_clear_irq();
#ifndef CONFIG_KERNEL_NONE
    extern void aos_sys_tick_handler(void);
    aos_sys_tick_handler();
#endif
}

csi_error_t csi_tick_init(void)
{
    csi_tick = 0U;
    tick_dev.irq_num = (uint8_t)CORET_IRQn;

    csi_irq_priority(CORET_IRQn, 0U);
    csi_irq_attach((uint32_t)tick_dev.irq_num, &tick_irq_handler, &tick_dev);
    csi_coret_config((soc_get_coretim_freq()/ CONFIG_SYSTICK_HZ), CORET_IRQn);
#ifndef __arm__
    csi_irq_enable((uint32_t)tick_dev.irq_num);
#endif

    return CSI_OK;
}

void csi_tick_uninit(void)
{
    csi_irq_disable((uint32_t)tick_dev.irq_num);
    csi_irq_detach((uint32_t)tick_dev.irq_num);
}

uint32_t csi_tick_get(void)
{
    return csi_tick;
}

uint32_t csi_tick_get_ms(void)
{
    uint32_t time;

    while (1) {
        time = (csi_tick * (1000U / CONFIG_SYSTICK_HZ)) + ((csi_coret_get_load() - csi_coret_get_value()) / (soc_get_coretim_freq() / 1000U));

        if (time >= last_time_ms) {
            break;
        }
    }

    last_time_ms = time;
    return time;
}

uint64_t csi_tick_get_us(void)
{
    uint32_t temp;
    uint64_t time;

    while (1) {
        /* the time of coretim pass */
        temp = csi_coret_get_load() - csi_coret_get_value();
        time = ((uint64_t)temp * 1000U) / ((uint64_t)soc_get_coretim_freq() / 1000U);
        /* the time of csi_tick */
        time += ((uint64_t)csi_tick * (1000000U / CONFIG_SYSTICK_HZ));

        if (time >= last_time_us) {
            break;
        }
    }

    last_time_us = time;
    return time;
}

static void _500usdelay(void)
{
    uint32_t load = csi_coret_get_load();
    uint32_t start = csi_coret_get_value();
    uint32_t cur;
    uint32_t cnt = (soc_get_coretim_freq() / 1000U / 2U);

    while (1) {
        cur = csi_coret_get_value();

        if (start > cur) {
            if ((start - cur) >= cnt) {
                break;
            }
        } else {
            if (((load - cur) + start) > cnt) {
                break;
            }
        }
    }
}

__WEAK void mdelay(uint32_t ms)
{
    while (ms) {
        ms--;
        _500usdelay();
        _500usdelay();
    }
}

static void _10udelay(void)
{
    uint32_t load  = csi_coret_get_load();
    uint32_t start = csi_coret_get_value();
    uint32_t cnt   = (soc_get_coretim_freq() / 1000U / 100U);

    while (1) {
        uint32_t cur = csi_coret_get_value();

        if (start > cur) {
            if ((start - cur) >= cnt) {
                break;
            }
        } else {
            if (((load - cur) + start) > cnt) {
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
