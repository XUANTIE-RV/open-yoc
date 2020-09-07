/*
 * silan_gpio_regs.h
 *
 * Modify Date: 2016-4-25 16:00
 */
#ifndef __SILAN_GPIO_REGS_H__
#define __SILAN_GPIO_REGS_H__

#include "silan_types.h"
#include "ap1508.h"

#define GPIO_PIN(n)         (1 << n)
#define GPIO_High_Level     1
#define GPIO_High_Edge      2
#define GPIO_Low_Level      3
#define GPIO_Low_Edge       4
#define GPIO_Edge           5

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

#define GPIO_FIL_DIV(x)		log2(x)

#endif //__SILAN_GPIO_REGS_H__

