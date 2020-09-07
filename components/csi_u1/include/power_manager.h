/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     power_manager.h
 * @brief    Header file for the power_manager
 * @version  V1.0
 * @date     23. August 2017
 ******************************************************************************/
#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

#include <stdint.h>
#include <drv/common.h>

typedef struct {
    void (*wakeup)(void *handle);
    void (*sleep)(void *handle);
    void (*manage_clock)(void *handle, uint8_t enable);
} power_cb_t;

int32_t drv_soc_power_control(void *handle, csi_power_stat_e state, power_cb_t *cb);
int32_t drv_clock_manager_config(clock_manager_e device, uint8_t enable);
void registers_save(uint32_t *mem, uint32_t *addr, int size);
void registers_restore(uint32_t *addr, uint32_t *mem, int size);

//#define CLOCK_CONTROL_REGISTER0      (CSKY_CLKGEN_BASE + 0xc)
//#define CLOCK_CONTROL_REGISTER1      (CSKY_CLKGEN_BASE + 0x4c)

#endif /* _POWER_MANAGER_H_ */

