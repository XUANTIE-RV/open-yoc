/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <string.h>

#include <pinmux.h>
#include <drv/gpio.h>
#include <yoc/lpm.h>

#define TAG "blpm"

extern void mdelay(uint32_t ms);
static void lpm_gpio_suspend(void)
{
    gpio_pin_handle_t pin_hdl;

    for(int i = PA0; i <= PD10; i++) {
        pin_hdl = csi_gpio_pin_initialize(i, NULL);
        csi_gpio_pin_config_mode(pin_hdl, GPIO_MODE_PUSH_PULL);
        csi_gpio_pin_config_direction(pin_hdl, GPIO_DIRECTION_OUTPUT);
        csi_gpio_pin_uninitialize(pin_hdl);
        mdelay(10);
    }
}

void board_enter_lpm(pm_policy_t policy)
{
    switch(policy) {
        case LPM_POLICY_LOW_POWER:
            break;

        case LPM_POLICY_DEEP_SLEEP:
            lpm_gpio_suspend();
            break;

        default: break;
    }
}

void board_leave_lpm(pm_policy_t policy)
{
}
