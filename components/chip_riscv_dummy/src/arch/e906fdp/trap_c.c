/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <csi_core.h>
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
#include <debug/dbg.h>
#else
#define printk printf
#endif

void (*trap_c_callback)(void);

void trap_c(uintptr_t *regs)
{
    int i;
    unsigned long vec = 0;

    vec = __get_MCAUSE() & 0x3FF;

    printk("CPU Exception: NO.0x%lx", vec);
    printk("\n");

    for (i = 0; i < 31; i++) {
        printk("x%d: %p\t", i + 1, (void *)regs[i]);

        if ((i % 4) == 3) {
            printk("\n");
        }
    }

    printk("\n");
    printk("mepc   : %p\n", (void *)regs[31]);
    printk("mstatus: %p\n", (void *)regs[32]);

    if (trap_c_callback) {
        trap_c_callback();
    }

    while (1);
}

__attribute__((weak)) void exceptionHandler(void *context)
{
    trap_c((uintptr_t *)context);
}

