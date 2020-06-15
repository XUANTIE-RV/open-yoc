/*
 * Copyright (C) 2020 FishSemi Limited
 */

/******************************************************************************
 * @file     pinmux.h
 * @brief    Header file for the pinmux
 * @version  V1.0
 * @date     14. May 2020
 ******************************************************************************/
#ifndef U1_PINMUX_H
#define U1_PINMUX_H

#include <stdint.h>
#include "pin_name.h"

void ioreuse_initial(void);
int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);
void drv_pinmux_reset(void);

#define U1_PINMUX_BASE_ADDR 0xb0050000

#endif /* U1_PINMUX_H */

