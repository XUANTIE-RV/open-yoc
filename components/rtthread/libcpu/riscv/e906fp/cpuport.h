/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-10-03     Bernard      The first version
 */
#ifndef CPUPORT_H__
#define CPUPORT_H__

#include <rtconfig.h>

/* bytes of register width  */
#if __riscv_xlen == 64
#define STORE                   sd
#define LOAD                    ld
#define REGBYTES                8
#else
#define STORE                   sw
#define LOAD                    lw
#define REGBYTES                4
#endif

/* 33 general register (x1~x31, mepc, mstatus)*/
#define CTX_GENERAL_REG_NR  33

#if defined(__riscv_flen) && defined(ENABLE_FPU)
/* 32 fpu register */
#define CTX_FPU_REG_NR  32
#else
#define CTX_FPU_REG_NR  0
#endif

/* all context registers */
#define CTX_REG_NR  (CTX_GENERAL_REG_NR + CTX_FPU_REG_NR)

#ifndef __ASSEMBLY__
#include <rtdef.h>

int rt_hw_cpu_id(void);

#endif

#endif
#ifdef RISCV_U_MODE
#define RISCV_USER_ENTRY 0xFFFFFFE000000000ULL
#endif
