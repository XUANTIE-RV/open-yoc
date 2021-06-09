/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pin.h
 * @brief    header File for pin definition
 * @version  V1.0
 * @date     15. May 2019
 ******************************************************************************/
#ifndef _PIN_H_
#define _PIN_H_

#include <stdint.h>
//#include <pin_name.h>
#include <pinmux.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_GETTIME_USE_TIMER_ID 0
#define CONSOLE_IDX 0

#define CONSOLE_TXD                 P9
#define CONSOLE_RXD                 P10
#define CONSOLE_TXD_FUNC            FMUX_UART0_TX
#define CONSOLE_RXD_FUNC            FMUX_UART0_RX

#define EXAMPLE_USART_IDX         0
#define EXAMPLE_PIN_USART_TX      P9
#define EXAMPLE_PIN_USART_RX      P10
#define EXAMPLE_PIN_USART_TX_FUNC UART_TX
#define EXAMPLE_PIN_USART_RX_FUNC UART_RX

#define EXAMPLE_SPI_SLAVE_IDX         1
#define EXAMPLE_PIN_SPI_SLAVE_MOSI    GPIO_P24
#define EXAMPLE_PIN_SPI_SLAVE_MISO    GPIO_P25
#define EXAMPLE_PIN_SPI_SLAVE_CS      GPIO_P31
#define EXAMPLE_PIN_SPI_SLAVE_SCK     GPIO_P32
#define EXAMPLE_PIN_SPI_SLAVE_MOSI_FUNC   SPI_1_TX
#define EXAMPLE_PIN_SPI_SLAVE_MISO_FUNC   SPI_1_RX
#define EXAMPLE_PIN_SPI_SLAVE_CS_FUNC     SPI_1_SSN
#define EXAMPLE_PIN_SPI_SLAVE_SCK_FUNC    SPI_1_SCK

/* tests definition */
#define TEST_GPIO_PIN            P14
#define TEST_BOARD_GPIO_PIN_NAME "P14"
#define TEST_GPIO_PIN_FUNC       PIN_FUNC_GPIO

#define TEST_SPI_IDX         0
#define TEST_PIN_SPI_MOSI    GPIO_P33
#define TEST_PIN_SPI_MISO    GPIO_P23
#define TEST_PIN_SPI_CS      GPIO_P14
#define TEST_PIN_SPI_SCK     GPIO_P15

#define TEST_PIN_SPI_MOSI_FUNC   SPI_0_TX
#define TEST_PIN_SPI_MISO_FUNC   SPI_0_RX
#define TEST_PIN_SPI_CS_FUNC     PIN_FUNC_GPIO
#define TEST_PIN_SPI_SCK_FUNC    SPI_0_SCK

#define TEST_IIC_IDX         0
#define TEST_PIN_IIC_SDA     GPIO_P14
#define TEST_PIN_IIC_SCL     GPIO_P15
#define TEST_PIN_IIC_SDA_FUNC    IIC0_SDA
#define TEST_PIN_IIC_SCL_FUNC    IIC0_SCL

#define TEST_PWM_IDX        0
#define TEST_PWM_CH         0       /*range 0~5 PWM_CH0~PWM_CH5*/
#define TEST_PWM_CH_PIN     GPIO_P14
#define TEST_PWM_CH_FUNC    PWM0

/* hal examples */
#define EXAMPLE_UART1_PORT_NUM  1
#define EXAMPLE_PIN_USART1_TX      P26
#define EXAMPLE_PIN_USART1_RX      P25
#define EXAMPLE_PIN_USART1_TX_FUNC      FMUX_UART1_TX
#define EXAMPLE_PIN_USART1_RX_FUNC      FMUX_UART1_RX

#define EXAMPLE_PIN_GPIO_INPUT      P15
#define EXAMPLE_PIN_GPIO_OUTPUT     P23
#define EXAMPLE_PIN_GPIO_FUNC  PIN_FUNC_GPIO

#define EXAMPLE_PIN_ADC_CH3P_P20        7
#define EXAMPLE_PIN_ADC_CH3P_P20_FUNC        FMUX_ADCC

#define EXAMPLE_I2C0_PORT_NUM   0
#define EXAMPLE_I2C1_PORT_NUM   1
#define EXAMPLE_PIN_IIC_SCL     P31
#define EXAMPLE_PIN_IIC_SDA     P32

#define EXAMPLE_PIN_IIC0_SDA_FUNC    FMUX_IIC0_SDA
#define EXAMPLE_PIN_IIC0_SCL_FUNC    FMUX_IIC0_SCL

#define EXAMPLE_PIN_IIC1_SDA_FUNC    FMUX_IIC1_SDA
#define EXAMPLE_PIN_IIC1_SCL_FUNC    FMUX_IIC1_SCL

#define EXAMPLE_PWM1_PORT_NUM   1
#define EXAMPLE_PIN_PWM         GPIO_P23
#define EXAMPLE_PIN_PWM_FUNC    FMUX_PWM1

#define EXAMPLE_SPI1_PORT_NUM   1
#define EXAMPLE_PIN_SPI_MISO    P31
#define EXAMPLE_PIN_SPI_MOSI    P32
#define EXAMPLE_PIN_SPI_CS      P34
#define EXAMPLE_PIN_SPI_SCK     P33

#define EXAMPLE_PIN_SPI_MISO_FUNC    FMUX_SPI_1_RX
#define EXAMPLE_PIN_SPI_MOSI_FUNC    FMUX_SPI_1_TX
#define EXAMPLE_PIN_SPI_CS_FUNC      FMUX_SPI_1_SSN
#define EXAMPLE_PIN_SPI_SCK_FUNC     FMUX_SPI_1_SCK

#define EXAMPLE_RTC1_PORT_NUM       0
#define EXAMPLE_WDG1_PORT_NUM       0
#define EXAMPLE_TIMER1_PORT_NUM     1

#ifdef __cplusplus
}
#endif

#endif /* _PIN_H_ */
