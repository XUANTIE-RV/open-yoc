/*
 * silan_rtc_regs.h
 *
 * Modify Date: 2016-4-28 18:10
 * MaintainedBy: yerenfeng <yerenfeng@silan.com.cn>
 */

#ifndef __SILAN_RTC_REGS_H__
#define __SILAN_RTC_REGS_H__

#include "silan_types.h"
#include "ap1508.h"

#define RTC_TIMER_LOAD_DEFUALT_VALUE    0xff

#define RTC_BASE                SILAN_RTC_BASE

#define SILAN_RTCMEM_BASE       (SILAN_RTC_BASE + 0x0400)

#define RTC_TIMER               0x0100
#define RTC_TMCON               0x0104
#define RTC_CLKOUT_CON          0x0108
#define RTC_WEEK_ALARM          0x010C
#define RTC_DAY_ALARM           0x0110
#define RTC_HOUR_ALARM          0x0114
#define RTC_MIN_ALARM           0x0118
#define RTC_YEARL               0x011C
#define RTC_MON                 0x0120
#define RTC_WEEK                0x0124
#define RTC_DAY                 0x0128
#define RTC_HOUR                0x012C
#define RTC_MIN                 0x0130
#define RTC_SEC                 0x0134
#define RTC_YEARH               0x0138
#define RTC_CS0                 0x013C
#define RTC_CS1                 0x0140
#define RTC_LOADH               0x0144
#define RTC_LOADL               0x0148

// TMCON
#define RTC_TMCON_HYS           (1 << 7)
#define RTC_TMCON_DLY           (1 << 6)
#define RTC_TMCON_OSC_CTRL      0x38
#define RTC_TMCON_TE            (1 << 2)
#define RTC_TMCON_TD            0x03

// CLKOUT_CON
#define RTC_CLKOUT_CON_PD_WR    (1 << 7)
#define RTC_CLKOUT_CON_PVT      0x78
#define RTC_CLKOUT_CON_FE       (1 << 2)
#define RTC_CLKOUT_CON_PD       0xFD

// WEEK_ALARM
#define RTC_WEEK_ALARM_WAE      (1 << 3)

// DAY_ALARM
#define RTC_DAY_ALARM_DAE       (1 << 6)

// HOUR_ALARM
#define RTC_HOUR_ALARM_HAE      (1 << 6)

// MIN_ALARM
#define RTC_WEEK_ALARM_MAE      (1 << 7)

// MON
#define RTC_MON_LEAP            (1 << 7)

// CS0
#define RTC_CS0_LDOEN           (1 << 7)
#define RTC_CS0_TEST            (1 << 6)
#define RTC_CS0_STOP            (1 << 5)
#define RTC_CS0_TI_TP           (1 << 4)
#define RTC_CS0_AF              (1 << 3)
#define RTC_CS0_TF              (1 << 2)
#define RTC_CS0_AIE             (1 << 1)
#define RTC_CS0_TIE             (1 << 0)

// CS1
#define RTC_CS1_BGP_TRIM33      0xFC
#define RTC_CS1_SCTRL           0x03

#endif //__SILAN_RTC_REGS_H__

