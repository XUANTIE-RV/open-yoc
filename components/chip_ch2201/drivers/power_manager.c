/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     power_manager.c
 * @brief    source file for power manager.
 * @version  V1.0
 * @date     18. July 2018
 ******************************************************************************/
#include <stdint.h>
#include <soc.h>
#include <io.h>
#include <drv/common.h>
#ifdef CONFIG_TEE_CA
#include <drv/tee.h>
#endif
#include <power_manager.h>

int32_t drv_get_boot_type(void)
{
    if (*(volatile uint32_t *)CSKY_PMU_BASE & 0x20) {
        /* boot from low power mode */
        return 1;
    }

    /* normal boot */
    return 0;
}

int32_t drv_clock_manager_config(clock_manager_e device, uint8_t enable)
{
#ifdef CONFIG_TEE_CA
    uint32_t val, val1;
    csi_tee_read_reg((uint32_t)CLOCK_CONTROL_REGISTER1, &val1);
    csi_tee_read_reg((uint32_t)CLOCK_CONTROL_REGISTER0, &val);
#endif

    if (enable) {
        if (device > 31) {
#ifdef CONFIG_TEE_CA
            val1 |= BIT(device - 32);
#else
            set_bit(device - 32, (uint32_t *)CLOCK_CONTROL_REGISTER1);
#endif
        } else {
#ifdef CONFIG_TEE_CA
            val |= BIT(device);
#else
            set_bit(device, (uint32_t *)CLOCK_CONTROL_REGISTER0);
#endif
        }
    } else {
        if (device > 31) {
#ifdef CONFIG_TEE_CA
            val1 &= ~BIT(device - 32);
#else
            clear_bit(device - 32, (uint32_t *)CLOCK_CONTROL_REGISTER1);
#endif
        } else {
#ifdef CONFIG_TEE_CA
            val &= ~BIT(device);
#else
            clear_bit(device, (uint32_t *)CLOCK_CONTROL_REGISTER0);
#endif
        }
    }

#ifdef CONFIG_TEE_CA
    csi_tee_write_reg((uint32_t)CLOCK_CONTROL_REGISTER1, val1);
    csi_tee_write_reg((uint32_t)CLOCK_CONTROL_REGISTER0, val);
#endif

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

int32_t drv_soc_power_control(void *handle, csi_power_stat_e state, power_cb_t *cb)
{
    power_ctrl_t *ctrl = handle;

    switch (state) {
        case DRV_POWER_FULL:
            if (ctrl->power_status == DRV_POWER_SUSPEND && cb->wakeup) {
                cb->wakeup(handle);
            } else if (ctrl->power_status != DRV_POWER_SUSPEND && cb->manage_clock) {
                cb->manage_clock(handle, 1);
            }

            break;

        case DRV_POWER_OFF:
            if (cb->manage_clock) {
                cb->manage_clock(handle, 0);
            }

            break;

        case DRV_POWER_SUSPEND:
            if (ctrl->power_status != DRV_POWER_SUSPEND && cb->sleep) {
                cb->sleep(handle);
            }

            break;

        case DRV_POWER_LOW:
            return (DRV_ERROR_UNSUPPORTED);

        default:
            return (DRV_ERROR_PARAMETER);
    }

    ctrl->power_status = state;

    return 0;
}
