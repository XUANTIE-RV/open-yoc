/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pinmux.h
 * @brief    Header file for the pinmux
 * @version  V1.0
 * @date     23. August 2017
 ******************************************************************************/
#ifndef CH2201_PINMUX_H
#define CH2201_PINMUX_H

#include <stdint.h>
#include "pin_name.h"

void ioreuse_initial(void);
int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);
void drv_pinmux_reset(void);

#define GPIO_SET_BIT16 0x00010000
#define GPIO_SET_BIT17 0x00020000


/******************************************************************************
 * CH2201 gpio control and gpio reuse function
 * selecting regester adddress
 ******************************************************************************/

#define CH2201_GIPO0_PORTCTL_REG 0x50006008
#define CH2201_GIPO1_PORTCTL_REG 0x50009008
#define CH2201_IOMUX0L_REG       0x50006100
#define CH2201_IOMUX0H_REG       0x50006104
#define CH2201_IOMUX1L_REG       0x50006108
#define CH2201_IOPU0_REG         0x50006124
#define CH2201_IOPU1_REG         0x50006128
#define CH2201_IOPD0_REG         0x5000612C
#define CH2201_IOPD1_REG         0x50006130
#define CH2201_IOOD0_REG         0x5000611C
#define CH2201_IOOD1_REG         0x50006120


/*************basic gpio reuse v1.0********************************************
 * UART1(PA16,PA17) for bootrom
 * UART1(PA20,PA21) for console
 ******************************************************************************/
#define GPIO0_REUSE_DIS               (GPIO_SET_BIT16 | GPIO_SET_BIT17)

#endif /* HOBBIT_PINMUX_H */

