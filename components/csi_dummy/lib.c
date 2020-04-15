/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     lib.c
 * @brief    source file for the lib
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#include <csi_config.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "csi_core.h"
#include "soc.h"
#include <sys_freq.h>

extern uint32_t csi_coret_get_load(void);
extern uint32_t csi_coret_get_value(void);

extern int32_t drv_get_cpu_id(void);
static void _500udelay(void)
{
    uint32_t load = csi_coret_get_load();
    uint32_t start = csi_coret_get_value();
    uint32_t cur;
    uint32_t cnt = (drv_get_cpu_freq(drv_get_cpu_id()) / 1000 / 2);

    while (1) {
        cur = csi_coret_get_value();

        if (start > cur) {
            if (start - cur >= cnt) {
                return;
            }
        } else {
            if (load - cur + start > cnt) {
                return;
            }
        }
    }
}

void mdelay(uint32_t ms)
{
    while (ms--) {
        _500udelay();
        _500udelay();
    }
}


static void _10udelay(void)
{
    uint32_t load = csi_coret_get_load();
    uint32_t start = csi_coret_get_value();
    uint32_t cnt = (drv_get_sys_freq() / 1000 / 100);

    while (1) {
        uint32_t cur = csi_coret_get_value();

        if (start > cur) {
            if (start - cur >= cnt) {
                return;
            }
        } else {
            if (load - cur + start > cnt) {
                return;
            }
        }
    }
}

void udelay(uint32_t us)
{
    us = (us / 10);

    if (us == 0) {
        return;
    }

    while (us--) {
        _10udelay();
    }
}
