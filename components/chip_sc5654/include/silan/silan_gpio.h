/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     silan_gpio.h
 * @brief    header file for GPIO Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef _SILAN_GPIO_H_
#define _SILAN_GPIO_H_

#include <soc.h>
#include <addrspace.h>
#include <drv/gpio.h>

#define GPIO1            0
#define GPIO2            1

#define GPIO1_MAX_NUM    22
#define GPIO2_MAX_NUM    10

#define IO_MAX_NUM       41

#define IO_INPUT         0
#define IO_OUTPUT        1

#define GPIO_PIN(n)         (1 << n)
 
#define GPIO_DATA           0X000
#define GPIO_DIR            0X400
#define GPIO_IS             0X404
#define GPIO_IBE            0X408
#define GPIO_IEV            0x40c
#define GPIO_IE             0X410
#define GPIO_RIS            0x414
#define GPIO_MIS            0x418
#define GPIO_IC             0X41C
#define GPIO_AFSEL          0X420
#define GPIO_FILSEL         0X424
#define GPIO_FILDIV         0X428

#define GPIO_HARDWARE_CONTROL    1u
#define GPIO_SOFTWARE_CONTROL    0u

#define GPIO_FIL_DIV(x)     log2(x)

#define SILAN_GPIO_BASE(n)        (SILAN_GPIO1_BASE + n*0x10000)

typedef struct {
} dw_gpio_reg_t;

typedef struct {
} dw_gpio_control_reg_t;

#endif /* _SILAN_GPIO_H_ */
