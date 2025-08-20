/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-30     lizhirui     first version
 * 2021-11-18     JasonHu      add fpu member
 */

#ifndef __STACK_H__
#define __STACK_H__

#include <rtthread.h>
#include "cpuport.h"

struct rt_hw_stack_frame
{
    rt_ubase_t ra;         /* x1  - ra     - return address for jumps            1*/
    rt_ubase_t x2;         /* x2  - x0     - sp                                  2*/
    rt_ubase_t gp;         /* x3  - gp     - global pointer                      3*/
    rt_ubase_t tp;         /* x4  - tp     - thread pointer                      4*/
    rt_ubase_t t0;         /* x5  - t0     - temporary register 0                5*/
    rt_ubase_t t1;         /* x6  - t1     - temporary register 1                6*/
    rt_ubase_t t2;         /* x7  - t2     - temporary register 2                7*/
    rt_ubase_t s0_fp;      /* x8  - s0/fp  - saved register 0 or frame pointer   8*/
    rt_ubase_t s1;         /* x9  - s1     - saved register 1                    9*/
    rt_ubase_t a0;         /* x10 - a0     - return value or function argument 0 10*/
    rt_ubase_t a1;         /* x11 - a1     - return value or function argument 1 11*/
    rt_ubase_t a2;         /* x12 - a2     - function argument 2                12*/
    rt_ubase_t a3;         /* x13 - a3     - function argument 3                13*/
    rt_ubase_t a4;         /* x14 - a4     - function argument 4                14*/
    rt_ubase_t a5;         /* x15 - a5     - function argument 5                15*/
    rt_ubase_t epc;        /* epc - epc    - program counter                    16*/
    rt_ubase_t mstatus;    /*              - supervisor status register         17*/
};

#endif
