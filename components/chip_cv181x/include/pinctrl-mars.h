/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 *
 * File Name: pinctrl-mars.h
 * Description:
 */

#ifndef __PINCTRL_MARS_H__
#define __PINCTRL_MARS_H__

#include "mars_reg_fmux_gpio.h"
#include "soc.h"
#include <top_reg.h>
#include <mmio.h>

#define RTC_IO_BASE 0x05027000
#define CTRL_SELPHY_PWR_ON 0x88
#define RTC_IO_REG_CNT 20

#define CVITEK_PINMUX_REG_LAST (FMUX_GPIO_FUNCSEL_PAD_AUD_AOUTR + 4)


#define PINMUX_MASK(PIN_NAME) FMUX_GPIO_FUNCSEL_##PIN_NAME##_MASK
#define PINMUX_OFFSET(PIN_NAME) FMUX_GPIO_FUNCSEL_##PIN_NAME##_OFFSET
#define PINMUX_VALUE(PIN_NAME, FUNC_NAME) PIN_NAME##__##FUNC_NAME
#define PINMUX_CONFIG(PIN_NAME, FUNC_NAME) \
		mmio_clrsetbits_32(PINMUX_BASE + FMUX_GPIO_FUNCSEL_##PIN_NAME, \
		PINMUX_MASK(PIN_NAME) << PINMUX_OFFSET(PIN_NAME), \
		PINMUX_VALUE(PIN_NAME, FUNC_NAME))

#endif /* __PINCTRL_MARS_H__ */

