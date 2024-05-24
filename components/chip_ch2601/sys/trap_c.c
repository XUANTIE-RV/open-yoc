/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     trap_c.c
 * @brief    source file for the trap process
 * @version  V1.0
 * @date     12. December 2017
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <csi_config.h>
#include <csi_core.h>
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
#include <debug/dbg.h>
#else
#define printk printf
#endif

void (*trap_c_callback)(void);

void trap_c(uint32_t *regs)
{
    int i;
    uint32_t vec = 0;

    vec = __get_MCAUSE() & 0x3FF;

    printk("CPU Exception: NO.%d", vec);
    printk("\n");

    for (i = 0; i < 31; i++) {
        printk("x%d: %08x\t", i + 1, regs[i]);

        if ((i % 4) == 3) {
            printk("\n");
        }
    }

    printk("\n");
    printk("mepc   : %08x\n", regs[31]);
    printk("mstatus: %08x\n", regs[32]);

    if (trap_c_callback) {
        trap_c_callback();
    }

    while (1);
}

