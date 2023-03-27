/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_ADC_NUM) && BOARD_ADC_NUM > 0
#include <stdlib.h>
#include <stdio.h>

#include <drv/pin.h>
#include <drv/gpio.h>

void board_adc_init(void)
{
    //csi_pin_set_mux(GPIO_PIN_16, GPIO16_ADC_CH8);
}
#endif
