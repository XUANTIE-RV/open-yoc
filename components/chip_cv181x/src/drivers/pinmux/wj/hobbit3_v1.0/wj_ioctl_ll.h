/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     wj_ioctl_ll.h
 * @brief    head file for hobbit3 pinmux
 * @version  V1.0
 * @date     9. July 2020
 ******************************************************************************/
#ifndef _WJ_IOCTL_LL_H_
#define _WJ_IOCTL_LL_H_

#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t IO_GPIO_SEL0;            //0x00  PA31~PA0 GPIO0 function enable
    __IOM uint32_t Reserved0;                //0x04
    __IOM uint32_t IO_AF_SEL0_L;            //0x08  PA15~PA0 alternative function select
    __IOM uint32_t IO_AF_SEL0_H;             //0x0C  PA31~PA16 alternative function select
    __IOM uint32_t Reserved1[(0x18 - 0x10) / 4]; //0x10~0x14
    __IOM uint32_t IO_WKUP0;                //0x18  PA31~PA0 wake up function enable
    __IOM uint32_t Reserved2[(0x20 - 0x1c) / 4]; //0x1C
    __IOM uint32_t IO_PU0;                    //0x20  PA31~PA0 pull up enable
    __IOM uint32_t Reserved3[(0x28 - 0x24) / 4]; //0x24
    __IOM uint32_t IO_PD0;                    //0x28  PA31~PA0 pull down enable
    __IOM uint32_t Reserved4[(0x30 - 0x2c) / 4]; //0x2C
    __IOM uint32_t IO_CS0;                    //0x30  PA31~PA0 input buffer select
    __IOM uint32_t Reserved5[(0x38 - 0x34) / 4]; //0x34
    __IOM uint32_t IO_DS0_L;                //0x38  PA15~PA0 drive strength select
    __IOM uint32_t IO_DS0_H;                //0x3C  PA31~PA16 drive strength select
    __IOM uint32_t Reserved6[(0x60 - 0x40) / 4]; //0x40~0x5C
    __IOM uint32_t IO_SR0;                    //0x60  PA31~PA0 slew rate select
    __IOM uint32_t Reserved7[(0x68 - 0x64) / 4]; //0x64
    __IOM uint32_t IO_PRSET0;                //0x68  PA31~PA0 pull up/down resistance select
    __IOM uint32_t Reserved8[(0x70 - 0x6c) / 4]; //0x6C
    __IOM uint32_t IO_ANAEN0;                //0x70  PA31~PA0 analog function enable
    __IOM uint32_t Reserved9[(0x78 - 0x74) / 4]; //0x74
    __IOM uint32_t IO_WKUP_POLAR0;            //0x78  PA31~PA0 wake up polarity select
    __IOM uint32_t Reserved10[(0xa0 - 0x7c) / 4]; //0x7c~0x9C
    __IOM uint32_t IO_OSC_TRM;                //0xa0  12M OSC pad operation current and capacitance trimming
    __IOM uint32_t IO_QSPIDRV;
    __IOM uint32_t IO_IIS456;
} wj_ioctl_regs_t;

#ifdef __cplusplus
}
#endif

#endif
