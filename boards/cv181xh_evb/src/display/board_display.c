/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(CONFIG_BOARD_DISPLAY) && CONFIG_BOARD_DISPLAY > 0

#include <stdio.h>
#include <drv/pin.h>
#include <st7701s_devops.h>
#include <gsl1680_devops.h>

void board_display_init(void)
{
    st7701s_gpio_pin _disp_gpio_pin = {0};
    gsl1680_gpio_pin _touch_gpio_pin = {0};
#if (CONFIG_BOARD_RUIXIANG_BOARD == 1)
//panel
    _disp_gpio_pin.blank_pwm_bank = 3;
    _disp_gpio_pin.blank_pwm_chn = 0,
//touch
    _touch_gpio_pin.iic_id = 1;
    _touch_gpio_pin.reset_gpio_polarity = 1;
    _touch_gpio_pin.reset_gpio_group = 1;
    _touch_gpio_pin.reset_gpio_chn = 18;
    _touch_gpio_pin.interrupt_gpio_group = 1;
    _touch_gpio_pin.interrupt_gpio_chn = 19;
#else
//panel
    _disp_gpio_pin.blank_pwm_bank = 2,
    _disp_gpio_pin.blank_pwm_chn = 0,
//touch
    _touch_gpio_pin.iic_id = 3;
    _touch_gpio_pin.reset_gpio_polarity = 0;
    _touch_gpio_pin.reset_gpio_group = 0;
    _touch_gpio_pin.reset_gpio_chn = 1;
    _touch_gpio_pin.interrupt_gpio_group = 0;
    _touch_gpio_pin.interrupt_gpio_chn = 4;
#endif
    drv_gsl1680_devops_register(&_touch_gpio_pin);
    drv_st7701s_display_register(&_disp_gpio_pin);
}
#endif
