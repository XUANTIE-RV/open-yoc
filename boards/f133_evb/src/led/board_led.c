/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_LED_NUM) && BOARD_LED_NUM > 0
#include <stdlib.h>
#include <stdio.h>
#include <drv/pin.h>

void board_led_init(void)
{
    csi_pin_set_mux(BOARD_LED0_PWM_PIN, PE16_PWM7);
}
#endif
