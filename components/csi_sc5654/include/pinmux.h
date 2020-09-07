/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pinmux.h
 * @brief    Header file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _SILAN_PINMUX_H_
#define _SILAN_PINMUX_H_

#include <stdint.h>
#include "pin_name.h"

enum{
    I2S_IN1=0,
    I2S_IN2,
    I2S_IN3,
    I2S_IN_51,
    I2S_OUT1,
    I2S_OUT2,
    I2S_OUT3,
    I2S_OUT_51,
    SDMMC,
    UART1,
    UART2,
    UART3,
    UART4,
    IIC1,
    IIC2,
    SPI1,
    SPI2,
    SPDIFIN1,
    SPDIFIN2,
    SPDIFIN3,
    SPDIFIN4,
    SPDIFOUT1,
    SPDIFOUT2,
    SPDIFOUT3,
    SPDIFOUT4
};
enum{
    OFF=0,
    ON
};

typedef union io_attr {
    /** raw data */
    char d8;
    /** bits def */
    struct {
        unsigned func:3;
        unsigned ds:2;
#define IO_ATTR_DS_2mA     0
#define IO_ATTR_DS_4mA     1
#define IO_ATTR_DS_8mA     2
#define IO_ATTR_DS_24mA    3

        unsigned ren:1;
#define IO_ATTR_PULL_EN    0
#define IO_ATTR_PULL_DIS   1

        unsigned ie:1;
#define IO_ATTR_INPUT_EN   1
#define IO_ATTR_INPUT_DIS  0

        unsigned sr:1;
#define IO_ATTR_SR_LS      0
#define IO_ATTR_SR_HS      1

    } __attribute__((packed)) b;
} __attribute__((packed)) io_attr_t;

#define  SILAN_IOMUX_BIAS_ADDR (64)

int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func);

#endif /* _SILAN_PINMUX_H_ */

