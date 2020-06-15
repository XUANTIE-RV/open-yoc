/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

/******************************************************************************
 * @file     dw_gpio.h
 * @brief    header file for GPIO Driver
 * @version  V1.0
 * @date     7. Feb 2020
 ******************************************************************************/
#ifndef _DW_GPIO_H_
#define _DW_GPIO_H_

#include "drv/gpio.h"
#include "soc.h"


/************************************************************************************
 * Pre-processor Definitions
 ************************************************************************************/

#define SONG_GPIO_INT_DISABLE              0x00
#define SONG_GPIO_INT_HIGHLEVEL            0x01
#define SONG_GPIO_INT_LOWLEVEL             0x03
#define SONG_GPIO_INT_GPIOCLK_RISING       0x05
#define SONG_GPIO_INT_GPIOCLK_FALLING      0x07
#define SONG_GPIO_INT_GPIOCLK_BOTHEDGES    0x09

#define SONG_GPIO_REG(base, pin, num)      ((base) + ((pin) / (num)) * 4)
#define SONG_GPIO_PORT_DR(pin)             SONG_GPIO_REG(0x000, pin, 16)
#define SONG_GPIO_PORT_DDR(pin)            SONG_GPIO_REG(0x040, pin, 16)
#define SONG_GPIO_EXT_PORT(pin)            SONG_GPIO_REG(0x080, pin, 32)
#define SONG_GPIO_INTR_CTRL(pin)           SONG_GPIO_REG(0x0A0, pin, 4)
#define SONG_GPIO_DEBOUNCE(pin)            SONG_GPIO_REG(0x1A0, pin, 16)
#define SONG_GPIO_INTR_RAW(pin)            SONG_GPIO_REG(0x1E0, pin, 32)
#define SONG_GPIO_INTR_CLR(pin)            SONG_GPIO_REG(0x200, pin, 32)
#define SONG_GPIO_INTR_MASK(cpu, pin)      SONG_GPIO_REG((0x220 + cpu * 0x60), pin, 16)
#define SONG_GPIO_INTR_STATUS(cpu, pin)    SONG_GPIO_REG((0x260 + cpu * 0x60), pin, 32)

//typedef struct {
//    __IOM uint32_t SWPORT_DR;                     /* Offset: 0x000 (W/R)  PortA data register */
//    __IOM uint32_t SWPORT_DDR;                    /* Offset: 0x004 (W/R)  PortA data direction register */
//    __IOM uint32_t PORT_CTL;                      /* Offset: 0x008 (W/R)  PortA source register */

//} dw_gpio_reg_t;

//typedef struct {
//    __IOM uint32_t INTEN;                         /* Offset: 0x000 (W/R)  Interrupt enable register */
//    __IOM uint32_t INTMASK;                       /* Offset: 0x004 (W/R)  Interrupt mask register */
//    __IOM uint32_t INTTYPE_LEVEL;                 /* Offset: 0x008 (W/R)  Interrupt level register */
//    __IOM uint32_t INT_POLARITY;                  /* Offset: 0x00c (W/R)  Interrupt polarity register */
//    __IM  uint32_t INTSTATUS;                     /* Offset: 0x010 (R)    Interrupt status of Port */
//    __IM  uint32_t RAWINTSTATUS;                  /* Offset: 0x014 (W/R)  Raw interrupt status of Port */
//    __IOM uint32_t revreg1;                       /* Offset: 0x018 (W/R)  Reserve register */
//    __OM  uint32_t PORTA_EOI;                     /* Offset: 0x01c (W/R)  Port clear interrupt register */
//    __IM  uint32_t EXT_PORTA;                     /* Offset: 0x020 (W/R)  PortA external port register */
//    __IM  uint32_t EXT_PORTB;                     /* Offset: 0x024 (W/R)  PortB external port register */
//    __IOM uint32_t revreg2[2];                    /* Offset: 0x028 (W/R)  Reserve register */
//    __IOM uint32_t LS_SYNC;                       /* Offset: 0x030 (W/R)  Level-sensitive synchronization enable register */

//} dw_gpio_control_reg_t;

#endif

