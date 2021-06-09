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
#include "io_config.h"
#include "lsgpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DUMMY_PIN,
}
pin_name_e;

typedef enum {
    DUMMY_FUNC,
} pin_func_e;

typedef enum {
    PORTA = 0,
    PORTB = 1,
    PORTC = 2,
} port_name_e;

#define PIN_FUNC_GPIO 0xff

#ifdef __cplusplus
}
#endif

#endif
