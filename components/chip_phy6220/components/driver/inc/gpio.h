/**************************************************************************************************

  Phyplus Microelectronics Limited confidential and proprietary.
  All rights reserved.

  IMPORTANT: All rights of this software belong to Phyplus Microelectronics
  Limited ("Phyplus"). Your use of this Software is limited to those
  specific rights granted under  the terms of the business contract, the
  confidential agreement, the non-disclosure agreement and any other forms
  of agreements as a customer or a partner of Phyplus. You may not use this
  Software unless you agree to abide by the terms of these agreements.
  You acknowledge that the Software may not be modified, copied,
  distributed or disclosed unless embedded on a Phyplus Bluetooth Low Energy
  (BLE) integrated circuit, either as a product or is integrated into your
  products.  Other than for the aforementioned purposes, you may not use,
  reproduce, copy, prepare derivative works of, modify, distribute, perform,
  display or sell this Software and/or its documentation for any purposes.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  PHYPLUS OR ITS SUBSIDIARIES BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

**************************************************************************************************/

/*******************************************************************************
* @file     gpio.h
* @brief    Contains all functions support for gpio and iomux driver
* @version  0.0
* @date     19. Oct. 2017
* @author   qing.han
*
* Copyright(C) 2016, PhyPlus Semiconductor
* All rights reserved.
*
*******************************************************************************/
#ifndef __GPIO_H__
#define __GPIO_H__

#include <types.h>
#include "bus_dev.h"
#include "error.h"

#define NUMBER_OF_PINS       23
#define NUMBER_OF_IRQ_PINS   NUMBER_OF_PINS
typedef enum {
    GPIO_P00   =   0,    P0  =  GPIO_P00,
    GPIO_P01   =   1,    P1  =  GPIO_P01,
    GPIO_P02   =   2,    P2  =  GPIO_P02,
    GPIO_P03   =   3,    P3  =  GPIO_P03,
    GPIO_P07   =   4,    P7  =  GPIO_P07,
    GPIO_P09   =   5,    P9  =  GPIO_P09,
    GPIO_P10   =   6,    P10  =  GPIO_P10,
    GPIO_P11   =   7,    P11  =  GPIO_P11,   Analog_IO_0 = GPIO_P11,
    GPIO_P14   =   8,    P14  =  GPIO_P14,   Analog_IO_1 = GPIO_P14,
    GPIO_P15   =   9,    P15  =  GPIO_P15,   Analog_IO_2 = GPIO_P15,
    GPIO_P16   =   10,   P16  =  GPIO_P16,   Analog_IO_3 = GPIO_P16, XTALI = GPIO_P16,
    GPIO_P17   =   11,   P17  =  GPIO_P17,   Analog_IO_4 = GPIO_P17, XTALO = GPIO_P17,
    GPIO_P18   =   12,   P18  =  GPIO_P18,   Analog_IO_5 = GPIO_P18,
    GPIO_P20   =   13,   P20  =  GPIO_P20,   Analog_IO_6 = GPIO_P20,
    GPIO_P23   =   14,   P23  =  GPIO_P23,   Analog_IO_7 = GPIO_P23,
    GPIO_P24   =   15,   P24  =  GPIO_P24,   Analog_IO_8 = GPIO_P24,
    GPIO_P25   =   16,   P25  =  GPIO_P25,   Analog_IO_9 = GPIO_P25,
    GPIO_P26   =   17,   P26  =  GPIO_P26,
    GPIO_P27   =   18,   P27  =  GPIO_P27,
    GPIO_P31   =   19,   P31  =  GPIO_P31,
    GPIO_P32   =   20,   P32  =  GPIO_P32,
    GPIO_P33   =   21,   P33  =  GPIO_P33,
    GPIO_P34   =   22,   P34  =  GPIO_P34,
    GPIO_NUM   =   23,
    GPIO_DUMMY =  0xff,
} gpio_pin_e;

typedef enum {
    FMUX_IIC0_SCL = 0,
    FMUX_IIC0_SDA = 1,
    FMUX_IIC1_SCL = 2,
    FMUX_IIC1_SDA = 3,
    FMUX_UART0_TX = 4,        FMUX_UART_TX = 4,
    FMUX_UART0_RX = 5,        FMUX_UART_RX = 5,
    FMUX_RF_RX_EN = 6,
    FMUX_RF_TX_EN = 7,
    FMUX_UART1_TX = 8,
    FMUX_UART1_RX = 9,
    FMUX_PWM0 = 10,
    FMUX_PWM1 = 11,
    FMUX_PWM2 = 12,
    FMUX_PWM3 = 13,
    FMUX_PWM4 = 14,
    FMUX_PWM5 = 15,
    FMUX_SPI_0_SCK = 16,
    FMUX_SPI_0_SSN = 17,
    FMUX_SPI_0_TX = 18,
    FMUX_SPI_0_RX = 19,
    FMUX_SPI_1_SCK = 20,
    FMUX_SPI_1_SSN = 21,
    FMUX_SPI_1_TX = 22,
    FMUX_SPI_1_RX = 23,
    FMUX_CHAX = 24,
    FMUX_CHBX = 25,
    FMUX_CHIX = 26,
    FMUX_CHAY = 27,
    FMUX_CHBY = 28,
    FMUX_CHIY = 29,
    FMUX_CHAZ = 30,
    FMUX_CHBZ = 31,
    FMUX_CHIZ = 32,
    FMUX_CLK1P28M = 33,
    FMUX_ADCC = 34,
    FMUX_ANT_SEL_0 = 35,
    FMUX_ANT_SEL_1 = 36,
    FMUX_ANT_SEL_2 = 37,

} gpio_fmux_e;

typedef enum {
    FRE_HCLK_DIV8 = 0,
    FRE_PCLK_DIV4 = 1,
    FRE_CLK_1P28M = 2,
    FRE_CLK_RC32K = 6,
    FRE_XTAL_CLK32768 = 7,
} Freq_Type_e;

typedef enum {
    GPIO_INPUT_1  = 0,
    GPIO_OUTPUT_1 = 1
} gpio_dir_t;

typedef enum {
    POL_FALLING = 0,    POL_ACT_LOW  = 0,
    POL_RISING  = 1,    POL_ACT_HIGH = 1
} gpio_polarity_e;

typedef enum {
    Bit_DISABLE = 0,
    Bit_ENABLE,
} bit_action_e;

typedef enum {
    GPIO_FLOATING  = 0x00,     //no pull
    GPIO_PULL_UP_S = 0x01,     //pull up weak
    GPIO_PULL_UP   = 0x02,     //pull up strong
    GPIO_PULL_DOWN = 0x03,
} gpio_pupd_e;

typedef struct {
    gpio_pin_e pin;
    gpio_pupd_e type;
} ioinit_cfg_t;


#define POSEDGE             POL_FALLING
#define NEGEDGE             POL_RISING
#define IO_Wakeup_Pol_e     gpio_polarity_e

#define FLOATING            GPIO_FLOATING
#define WEAK_PULL_UP        GPIO_PULL_UP
#define STRONG_PULL_UP      GPIO_PULL_UP_S
#define PULL_DOWN           GPIO_PULL_DOWN
#define GPIO_Pin_e          gpio_pin_e
#define OEN                 GPIO_OUTPUT
#define IE                  GPIO_INPUT
#define Fmux_Type_e         gpio_fmux_e
#define GPIO_Wakeup_Pol_e   gpio_polarity_e
#define BitAction_e         bit_action_e

#define IO_Pull_Type_e       gpio_pupd_e
typedef void (*gpioin_Hdl_t)(gpio_pin_e pin, gpio_polarity_e type);

void phy_gpio_write(gpio_pin_e pin, uint8_t en);
void phy_gpio_fast_write(gpio_pin_e pin, uint8_t en);
bool phy_gpio_read(gpio_pin_e pin);
void phy_gpio_fmux(gpio_pin_e pin, bit_action_e value);
void phy_gpio_fmux_set(gpio_pin_e pin, gpio_fmux_e type);

void phy_gpio_pin_init(gpio_pin_e pin, gpio_dir_t type);
void phy_gpio_ds_control(gpio_pin_e pin, bit_action_e value);
int phy_gpioin_unregister(gpio_pin_e pin);
int phy_gpio_cfg_analog_io(gpio_pin_e pin, bit_action_e value) ;
void phy_gpio_pull_set(gpio_pin_e pin, gpio_pupd_e type) ;
void phy_gpio_wakeup_set(gpio_pin_e pin, gpio_polarity_e type);

void phy_gpio_wakeup_src(uint32_t *p34_32, uint32_t *p31_P00);
void phy_gpio_pin2pin3_control(gpio_pin_e pin, uint8_t en);
void phy_gpio_retention_enable(gpio_pin_e pin, uint8_t en);

int phy_gpioin_disable(gpio_pin_e pin);
void __attribute__((used)) phy_GPIO_IRQHandler(void);
int phy_gpioin_enable(gpio_pin_e pin);
int phy_gpioin_register(gpio_pin_e pin, gpioin_Hdl_t posedgeHdl, gpioin_Hdl_t negedgeHdl);

int phy_gpio_init(void);
void phy_gpio_debug_mux(Freq_Type_e fre, bool en);

//when use gpio retention,make it output and register.
int phy_gpioretention_unregister(gpio_pin_e pin);
int phy_gpioretention_register(gpio_pin_e pin);
void phy_gpioretention_prepare_sleep_action(void);
void phy_gpioretention_prepare_wakeup_action(void);
__attribute__((section(".__sram.code.phy_gpioretention_disable"))) void phy_gpioretention_disable(void);

#endif
