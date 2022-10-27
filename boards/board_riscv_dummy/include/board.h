/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include <soc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_UART_IDX                (0)
#define CONFIG_CLI_USART_BAUD           (115200)
#define CONFIG_CONSOLE_UART_BUFSIZE     (128)

#define CONSOLE_TXD                0// PA10
#define CONSOLE_RXD                0// PA11
#define CONSOLE_TXD_FUNC           0// PA10_UART0_TX
#define CONSOLE_RXD_FUNC           0// PA11_UART0_RX

// i2c
#define EXAMPLE_IIC_IDX            0// 1
#define EXAMPLE_PIN_IIC_SDA        0// PC1
#define EXAMPLE_PIN_IIC_SCL        0// PC0
#define EXAMPLE_PIN_IIC_SDA_FUNC   0// PC1_I2C1_SDA
#define EXAMPLE_PIN_IIC_SCL_FUNC   0// PC0_I2C1_SCL

// adc
#define EXAMPLE_ADC_CH0            0// PA8
#define EXAMPLE_ADC_CH0_FUNC       0// PA8_ADC_A0
#define EXAMPLE_ADC_CH12           0// PA26
#define EXAMPLE_ADC_CH12_FUNC      0// PA26_ADC_A12

/**
 * @brief  init the board for default: pin mux, etc.
 * re-implement if need.
 * @return
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */

