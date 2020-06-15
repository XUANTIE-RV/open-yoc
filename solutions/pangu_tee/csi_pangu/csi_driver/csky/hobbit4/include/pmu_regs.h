/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pmu_regs.h
 * @brief    header file for the pmu
 * @version  V1.0
 * @date     07. Aug 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#ifndef _PMU_REGS_
#define _PMU_REGS_

#define     __IM     volatile const       /*! Defines 'read only' structure member permissions */
#define     __OM     volatile             /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile             /*! Defines 'read / write' structure member permissions */

typedef struct {
    __IOM uint32_t PMU_CLKSRCSEL;
    __IOM uint32_t PMU_SYSCLKDIVEN;
    __IOM uint32_t PMU_CPU12SWRST;
    __IOM uint32_t PMU_PLLCTRL;
    __IOM uint32_t PMU_PLLCTRL_FRAC;
    __IOM uint32_t PMU_LPCR;
    __IOM uint32_t PMU_WKUPMASK;
    __IOM  uint32_t PMU_RSTSTA;
    __IOM uint32_t PMU_RCCTRL;
    __IM  uint32_t PMU_LPSTA;
    __IM  uint32_t PMU_LVMD;
    __IOM uint32_t PMU_REQMDTRIG;
    __IM  uint32_t PMU_PERIERRSTA;
    __IOM uint32_t PMU_PLLINTRCTRL;
    __IOM uint32_t PMU_FRACPLLFRAC;
    uint32_t RESERVED0;
    __IOM uint32_t PMU_BOOTADDR_804_0;
    __IOM uint32_t PMU_BOOTADDR_805;
    __IOM uint32_t PMU_BOOTADDR_BYPASS;
    __IOM uint32_t PMU_AONTIMCTRL;
    uint32_t RESERVED1;
    __IOM uint32_t PMU_AONTIMRST;
    uint32_t RESERVED2;
    __IOM uint32_t PMU_AONRTCRST;
    uint32_t RESERVED3;
    __IOM uint32_t PMU_AONGPIORST;
    __IOM uint32_t PMU_LPBOOTADDR_804_0;
    __IOM uint32_t PMU_LPBOOTADDR_805;
    __IOM uint32_t PMU_QSPI_CNT;
    __IOM uint32_t PMU_WAIT_12M;
    __IOM uint32_t PMU_RTCCLKCTRL;
    uint32_t RESERVED4;
    __IOM uint32_t PMU_BOOTADDR_804_1;
    __IOM uint32_t PMU_LP_BOOTADDR_804_1;
    uint32_t RESERVED5[94];
    __IOM uint32_t DLC_PCTRL;
    __IOM uint32_t DLC_PRDATA;
    __IOM uint32_t DLC_SR;
    __IOM uint32_t DLC_IMR;
    __IOM uint32_t DLC_IFR;
    __IOM uint32_t DLC_IOIFR;
    __IOM uint32_t DLC_IDIFR;
    __IOM uint32_t DLC_IMCIFR;
} pmu_reg_t;

#endif
