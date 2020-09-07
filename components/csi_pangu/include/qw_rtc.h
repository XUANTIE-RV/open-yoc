/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     qw_rtc.h
 * @brief    header file for rtc driver
 * @version  V1.0
 * @date     26. April 2019
 ******************************************************************************/
#ifndef _QW_RTC_H_
#define _QW_RTC_H_

#include <soc.h>

#define SEC_PER_MIN  ((time_t)60)
#define SEC_PER_HOUR ((time_t)60 * SEC_PER_MIN)
#define SEC_PER_DAY  ((time_t)24 * SEC_PER_HOUR)

enum rtc_int_t {
    RTC_INT_ALM = 0,
    RTC_INT_YEAR,
    RTC_INT_MON,
    RTC_INT_WEEK,
    RTC_INT_DAY,
    RTC_INT_HOUR,
    RTC_INT_MIN,
    RTC_INT_SEC
};

typedef struct {
    __IM  uint32_t RTC_CDR;    /* Offset: 0x00 (R/ )  rtc_cnt, read value */
    __IOM uint32_t RTC_CLR;    /* Offset: 0x04 (R/W)  rtc_cnt, set value */
    __IOM uint32_t RTC_CR;     /* Offset: 0x08 (R/W)  rtc_cnt control register */
    __IM  uint32_t RTC_STAT;   /* Offset: 0x0c (R/ )  mask interrupt register */
    __IM  uint32_t RTC_RSTAT;  /* Offset: 0x10 (R/ )  raw mask interrupt register */
    __IM  uint32_t RTC_VID;    /* Offset: 0x14 (R/ )  version register */
    __IM  uint32_t RTC_TR_L;   /* Offset: 0x18 (R/ )  time register, low 16 bits */
    __IM  uint32_t RTC_TR_H;   /* Offset: 0x1c (R/ )  time register, high 16 bits */
    __IM  uint32_t RTC_DR_L;   /* Offset: 0x20 (R/ )  date register, low 16 bits */
    __IM  uint32_t RTC_DR_H;   /* Offset: 0x24 (R/ )  date register, high 16 bits */
    __IOM uint32_t RTC_TLR_L;  /* Offset: 0x28 (R/W)  time load register, low 16 bits */
    __IOM uint32_t RTC_TLR_H;  /* Offset: 0x2c (R/W)  time load register, high 16 bits */
    __IOM uint32_t RTC_DLR_L;  /* Offset: 0x30 (R/W)  date load register, low 16 bits */
    __IOM uint32_t RTC_DLR_H;  /* Offset: 0x34 (R/W)  date load register, high 16 bits */
    __IOM uint32_t RTC_TMR_L;  /* Offset: 0x38 (R/W)  time interrupt match reg, low 16 bits */
    __IOM uint32_t RTC_TMR_H;  /* Offset: 0x3c (R/W)  time interrupt match reg, high 16 bits */
    __IOM uint32_t RTC_DTCR_L; /* Offset: 0x40 (R/W)  data/time intc, low 16 bits */
    __OM  uint32_t RTC_DTCR_H; /* Offset: 0x44 (R/ )  data/time intc, high 16 bits */
    __IOM uint32_t RTC_CAL_CR; /* Offset: 0x48 (R/W)  cal control register */
    __IOM uint32_t RTC_CAL_LD; /* Offset: 0x4c (R/W)  calibration value set register */
    __IM  uint32_t RTC_CAL_RD; /* Offset: 0x50 (R/ )  calibration value get register */
} qw_rtc_reg_t;
#endif /* _QW_RTC_H_ */

