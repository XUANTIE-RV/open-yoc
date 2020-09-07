/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __PIN_H__
#define __PIN_H__

#include <stdint.h>
#include "pinmux.h"

#define CONSOLE_UART_IDX  0

#define CONSOLE_TXD                 PA10
#define CONSOLE_RXD                 PA11
#define CONSOLE_TXD_FUNC            PA10_UART0_TX
#define CONSOLE_RXD_FUNC            PA11_UART0_RX

// i2c
#define EXAMPLE_IIC_IDX             1
#define EXAMPLE_PIN_IIC_SDA         PC1
#define EXAMPLE_PIN_IIC_SCL         PC0
#define EXAMPLE_PIN_IIC_SDA_FUNC    PC1_I2C1_SDA
#define EXAMPLE_PIN_IIC_SCL_FUNC    PC0_I2C1_SCL

// adc
#define EXAMPLE_ADC_CH0             PA8
#define EXAMPLE_ADC_CH0_FUNC        PA8_ADC_A0
#define EXAMPLE_ADC_CH12            PA26
#define EXAMPLE_ADC_CH12_FUNC       PA26_ADC_A12


#endif
