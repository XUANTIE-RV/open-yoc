/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     power_manager.c
 * @brief    source file for power manager.
 * @version  V1.0
 * @date     7. April 2020
 ******************************************************************************/
#include <stdint.h>
#include <soc.h>
#include <drv/io.h>
#include <drv/common.h>
#include <power_manager.h>

uint32_t pmu_clock_gate0, pmu_clock_gate1, pmu_clock_gate2;

int32_t drv_get_boot_type(void)
{
    int32_t ret = 0;
    if (*(volatile uint32_t *)WJ_PMU_BASE & 0x20U) {
        /* boot from low power mode */
        ret = 1;
    }

    /* normal boot */
    return ret;
}

int32_t drv_clock_manager_config(clk_manager_type_t device, uint8_t enable)
{
    if (enable) {
        if (device < 14) {
            putreg32(pmu_clock_gate0 |= (1 << device), (uint32_t *)CLOCK_CONTROL_REGISTER0);
        } else if (device < 25) {
            putreg32(pmu_clock_gate1 |= (1 << (device - 14)), (uint32_t *)CLOCK_CONTROL_REGISTER1);
        } else if (device < 27) {
            putreg32(pmu_clock_gate2 |= (1 << (device - 25)), (uint32_t *)CLOCK_CONTROL_REGISTER2);
        }
    } else {
        if (device < 14) {
            putreg32(pmu_clock_gate0 &= ~(1 << device), (uint32_t *)CLOCK_CONTROL_REGISTER0);
        } else if (device < 25) {
            putreg32(pmu_clock_gate1 &= ~(1 << (device - 14)), (uint32_t *)CLOCK_CONTROL_REGISTER1);
        } else if (device < 27) {
            putreg32(pmu_clock_gate2 &= ~(1 << (device - 25)), (uint32_t *)CLOCK_CONTROL_REGISTER2);
        }
    }

    return 0;
}

void registers_save(uint32_t *mem, uint32_t *addr, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        mem[i] = *((volatile uint32_t *)addr + i);
    }
}

void registers_restore(uint32_t *addr, uint32_t *mem, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        *((volatile uint32_t *)addr + i) = mem[i];
    }

}

typedef struct {
    uint8_t power_status;
} power_ctrl_t;

