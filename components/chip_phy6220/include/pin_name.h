/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pin_name.h
 * @brief    header file for the pin_name
 * @version  V1.0
 * @date     23. August 2017
 ******************************************************************************/
#ifndef _PINNAMES_H
#define _PINNAMES_H

#include "gpio.h"
#ifdef __cplusplus
extern "C" {
#endif

#define PIN_GPIO_0                      P0//P14 
#define PIN_GPIO_14                     P14//P14
#define BOARD_GPIO_PIN_NAME             "P14"
#define GPIO_PIN_FUNC                   PIN_FUNC_GPIO

#define PIN_FUNC_GPIO  99
#define pin_func_e     gpio_fmux_e
#define pin_name_e     gpio_pin_e


typedef enum {
    PORTA = 0,

} port_name_e;


/*
typedef enum {
    PORTA = 0,
    PORTB = 1,
    PORTC = 2,
} port_name_e;

typedef enum {
    PA0 = 0,
    PA1,
    PA2,
    PA3,
    PA4,
    PA5,

    PB0,
    PB1,
    PB2,
    PB3,

    PA6,
    PA7,
    PA8,
    PA9,
    PA10,
    PA11,
    PA12,
    PA13,
    PA14,
    PA15,
    PA16,
    PA17,
    PA18,
    PA19,
    PA20,
    PA21,
    PA22,
    PA23,
    PA24,
    PA25,
    PA26,
    PA27,

    PC0,
    PC1
}
pin_name_e;
*/

#ifdef __cplusplus
}
#endif

#endif
