/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(CONFIG_BOARD_DISPLAY) && CONFIG_BOARD_DISPLAY > 0

#include <stdio.h>
#include <drv/pin.h>
#include <st7701s_devops.h>
#include <gsl1680_devops.h>

#include "platform.h"

void board_display_init(void)
{
    st7701s_gpio_pin _disp_gpio_pin = {0};

    /* Panel PWM_8 */
    _disp_gpio_pin.blank_pwm_bank = LCD_PWM_PIN_GRP;
    _disp_gpio_pin.blank_pwm_chn = LCD_PWM_PIN_CHN;

#if !defined(CONFIG_DISPLAY_TOUCHPAD_DISABLED)
    gsl1680_gpio_pin _touch_gpio_pin = {0};
    /* Touch TP_INT:XGPIOA_19 TP_RST:XGPIOA_18 */
    _touch_gpio_pin.iic_id = TOUCH_I2C_ID;
    _touch_gpio_pin.reset_gpio_polarity = 1;
    _touch_gpio_pin.reset_gpio_group = TOUCH_RST_PIN_GRP;
    _touch_gpio_pin.reset_gpio_chn = TOUCH_RST_PIN_CHN;
    _touch_gpio_pin.interrupt_gpio_group = TOUCH_INTR_PIN_GRP;
    _touch_gpio_pin.interrupt_gpio_chn = TOUCH_INTR_PIN_CHN;
    _touch_gpio_pin.rotation_angle = 0;
    _touch_gpio_pin.mirror_type = 1; /* x mirror */

    drv_gsl1680_devops_register(&_touch_gpio_pin);
#endif

    drv_st7701s_display_register(&_disp_gpio_pin);

#if !defined(CONFIG_DISPLAY_TOUCHPAD_DISABLED)
    /* 恢复Jtag功能,上电前必须拔掉Jtag，触屏才能使用,同时插上Jtag也可以烧写调试 */
    PLATFORM_JtagPinmux();
#endif
}
#endif
