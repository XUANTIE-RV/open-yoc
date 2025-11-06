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

#ifdef RT_USING_SMP
typedef union {
    unsigned long slock;
    struct __arch_tickets {
        unsigned short owner;
        unsigned short next;
    } tickets;
} rt_hw_spinlock_t;
#endif
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

/* 33 general registers */
#define CTX_GENERAL_REG_NR  33

#if defined(__riscv_flen) && defined(ENABLE_FPU)
/* 32 fpu register */
#define CTX_FPU_REG_NR      32
/* 1 fpu csr register */
#define CTX_FPU_CSR_REG_NR  1
#else
#define CTX_FPU_REG_NR      0
#define CTX_FPU_CSR_REG_NR  0
#endif

#if defined(__riscv_vector) && defined(ENABLE_VECTOR)
/* 32 vector registers */
#define CTX_VECTOR_REG_NR       32
/* 5 vector csr registers */
#define CTX_VECTOR_CSR_REG_NR   5
#else
#define CTX_VECTOR_REG_NR       0
#define CTX_VECTOR_CSR_REG_NR   0
#endif

#ifdef CONFIG_VIC_TSPDR
#define VIC_TSPDR CONFIG_VIC_TSPDR
#else
#define VIC_TSPDR 0xE4000000
#endif

#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
#define RISCV_VIC_TSPDR (VIC_TSPDR + 0xC000)       /*soft irq register*/
#else
#define RISCV_VIC_TSPDR (VIC_TSPDR)                /*soft irq register*/
#endif

#ifndef __ASSEMBLY__
#include <rtdef.h>

int rt_hw_cpu_id(void);

#endif

#endif
#ifdef RISCV_U_MODE
#define RISCV_USER_ENTRY 0xFFFFFFE000000000ULL
#endif
