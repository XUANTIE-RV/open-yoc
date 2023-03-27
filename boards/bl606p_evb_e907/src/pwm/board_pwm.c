/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(CONFIG_BOARD_PWM) && CONFIG_BOARD_PWM
#include <stdlib.h>
#include <stdio.h>

#include <drv/pin.h>
#include <drv/gpio.h>

void board_pwm_init(void)
{
    csi_pin_set_mux(GPIO_PIN_17, GPIO17_PWM1_CH1P);
    csi_pin_set_mux(GPIO_PIN_18, GPIO18_PWM1_CH2P);
    csi_pin_set_mux(GPIO_PIN_19, GPIO19_PWM1_CH3P);
}
#endif

