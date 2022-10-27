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


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PA0 = 0,
    PA1,
    PA2,
    PA3,
    PA4,
    PA5,
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

    PB0,
    PB1,
    PB2,
    PB3,
    PB4,
    PB5,
    PB6,
    PB7,
    PB8,
    PB9,
    PB10,
    PB11,
    PB12,
    PB13,
    PB14,
    PB15,
    PB16,
    PB17,
    PB18,
    PB19,
    PB20,
    PB21,
    PB22,
    PB23,
    PB24,
    PB25,
    PB26,
    PB27,
    PB28,
    PB29,
    PB30,
    PB31
}
pin_name_e;

typedef enum {
    PA0_SPI_CS     = 0,
    PA0_IIS_DO     = 1,
    PA0_IIS_MCLK   = 2,
    PA0_PWM        = 3,

    PA1_IIS_WS     = 0,
    PA1_IIC_SCL    = 1,
    PA1_PWM        = 2,
    PA1_ADC        = 5,

    PA4_IIS_CK     = 0,
    PA4_IIC_SDA    = 1,
    PA4_PWM        = 2,
    PA4_ADC        = 5,

    PA7_SPI_DO     = 0,
    PA7_IIS_DI     = 1,
    PA7_IIS_EXTCLK = 2,
    PA7_PWM        = 3,

    PB0_UART3_TX   = 0,
    PB0_SPI_DI     = 1,
    PB0_PWM        = 2,
    PB0_PSRAM_CK   = 3,

    PB1_UART3_RX   = 0,
    PB1_SPI_CK     = 1,
    PB1_PWM        = 2,
    PB1_PSRAM_CS   = 3,

    PB2_UART2_TX   = 0,
    PB2_SPI_CK     = 1,
    PB2_PWM        = 2,
    PB2_PSRAM_DAT0 = 3,

    PB3_UART2_RX   = 0,
    PB3_SPI_DI     = 1,
    PB3_PWM        = 2,
    PB3_PSRAM_DAT1 = 3,

    PB4_UART2_RTS  = 0,
    PB4_UART4_TX   = 1,
    PB4_SPI_CS     = 2,
    PB4_PSRAM_DAT2 = 3,

    PB5_UART2_CTS  = 0,
    PB5_UART4_RX   = 1,
    PB5_SPI_DO     = 2,
    PB5_PSRAM_DAT3 = 3,

    PB6_UART1_TX   = 0,

    PB7_UART1_RX   = 0,

    PB8_IIS_CK     = 0,

    PB9_IIS_WS     = 0,

    PB10_IIS_DI    = 0,

    PB11_IIS_DO    = 0,

    PB19_UART0_TX  = 0,
    PB19_UART1_RTS = 1,
    PB19_IIC_SDA   = 2,
    PB19_PWM       = 3,

    PB20_UART0_RX  = 0,
    PB20_UART1_CTS = 1,
    PB20_IIC_SCL   = 2,
    PB20_PWM       = 3,

    PIN_FUNC_GPIO  = 5
} pin_func_e;

typedef enum {
    PORTA = 0,
    PORTB = 1,
} port_name_e;

#ifdef __cplusplus
}
#endif

#endif
