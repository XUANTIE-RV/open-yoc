/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include <pin_name.h>

#define CONSOLE_IDX  1
#define CONSOLE_UART  2


#if 1//def I2C
#define I2C_SCL   PC12   //管脚3 GPIO1_21
#define I2C_SDA   PC13   //管脚4 GPIO1_22
#else
#define PWM5   PC12
#define PWM6   PC13
#endif


#define BOOTSTRAP      PD4//SPI2_CS1/O3_I2S_SDO/DSP_JTRST/PWM3/TCK  //管脚22 GPIO2_4
#define SPI2_CLK       PD2//SSP_CLK/SPI2_CLK/TXD3/O3_I2S_SCLK/DSP_JTDO/RISC_SWCLK/PWM1 //管脚23 GPIO2_2
#define SPI2_CS        PD3//SSP_FSS/SPI2_CS0/RXD3/O3_I2S_WS/DSP_JTMS/RISC_SWD/PWM2 //管脚24 GPIO2_3
#define SPI2_DI        PD1//SSP_MISO/SPI2_MISO/RXD2/O3_I2S_MCLK/DSP_JTDI/MCU_SWD/PWM0 //管脚25 GPIO2_1
#define SPI2_DO        PD0//SSP_MOSI/SPI2_MOSI/TXD2/DSP_JTCK/MCU_SWCLK/PWM7 //管脚26 GPIO2_0
#define PA1            PA1//ADIN1/PWM1/TCK //管脚32 GPIO2_12
#define PA2            PA2//ADIN2/PWM2 //管脚33 GPIO2_13
#define PA3            PA3//ADIN3/PWM3 //管脚34 GPIO2_14
#define UART_TXD2      PA4//ADIN4/PWM4/TXD2 //管脚35 GPIO2_15
#define UART_RXD2      PA5//ADIN5/PWM5/RXD2 //管脚36 GPIO2_16
#define ADIN6          PA6//ADIN6/PWM6/TCK //管脚37 GPIO2_17
#define UART_HS_RXD1   PB1//HS_RXD1/PWM1 //管脚38 GPIO1_1
#define UART_HS_TXD1   PB0//HS_TXD1/PWM0 //管脚39 GPIO1_0


#define TEST_GPIO_PIN    BOOTSTRAP
#define TEST_GPIO_PIN_FUNC   0

#define TEST_PWM_IDX      1
#define TEST_PWM_CH       1
#define TEST_PWM_CH1      PA1
#define TEST_PWM_CH1_FUNC IO_FUNC_PWM





/* 可用GPIO列表 */
#define USER_GPIO_LIST_STR \
"ID  Name\n \
 1  PA1\n \
 2  PA2\n \
 3  PA3\n "

#endif /* __BOARD_CONFIG_H__ */

