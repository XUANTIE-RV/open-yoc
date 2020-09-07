/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef INTC_H
#define INTC_H

#define  HOBBIT_VIC_BASE 0xE000E100

#define  VIC_ISER        0x0000
#define  VIC_IWER        0x0040
#define  VIC_ICER        0x0080
#define  VIC_IWDR        0x00C0
#define  VIC_ISPR        0x0100
#define  VIC_ICPR        0x0180
#define  VIC_IABR        0x0200
#define  VIC_IPR0        0x0300
#define  VIC_IPR1        0x0304
#define  VIC_IPR2        0x0308
#define  VIC_IPR3        0x030C
#define  VIC_IPR4        0x0310
#define  VIC_IPR5        0x0314
#define  VIC_IPR6        0x0318
#define  VIC_IPR7        0x031C
#define  VIC_ISR         0x0B00
#define  VIC_IPTR        0x0B04

void hal_intc_init(void);

#endif

