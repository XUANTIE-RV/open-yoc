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

void (*trap_c_callback)(void);

void trap_c(uint32_t *regs)
{
    int i;
    uint32_t vec = (__get_PSR() & PSR_VEC_Msk) >> PSR_VEC_Pos;

    printf("CPU Exception : NO.%u", vec);
    printf("\n");

    for (i = 0; i < 32; i++) {
        printf("r%d: %08x\t", i, regs[i]);

        if ((i % 4) == 3) {
            printf("\n");
        }
    }

    for (i = 0; i < 16; i++) {
        printf("vr%d: %08x\t", i, regs[i + 32]);

        if ((i % 4) == 3) {
            printf("\n");
        }
    }

    printf("\n");
    printf("epsr: %8x\n", regs[48]);
    printf("epc : %8x\n", regs[49]);

    if (trap_c_callback) {
        trap_c_callback();
    }

    while (1);
}

