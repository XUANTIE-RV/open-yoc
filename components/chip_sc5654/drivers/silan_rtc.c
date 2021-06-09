/*
 * silan_rtc.c
 *
 * Modify Date: 2016-4-28 18:10
 * MaintainedBy: yerenfeng <yerenfeng@silan.com.cn>
 */

#include "ap1508.h"
#include "silan_rtc_regs.h"
#include "silan_rtc.h"
#include "silan_printf.h"
#include "silan_syscfg.h"

void silan_rtcmem_ldo_onoff(uint8_t onoff)
{
    uint8_t tmp;
    tmp = __sREG32(RTC_BASE,RTC_CS0);
    if(onoff)
        tmp |= 0x80;   //ldo on
    else
        tmp &= 0x7f;   //ldo off

    __sREG32(RTC_BASE,RTC_CS0) = tmp;
}

void silan_soc_ldo_config(uint8_t level)
{
    uint8_t tmp;
    silan_vlsp_cclk_config(CLK_ON);
    tmp = __sREG32(RTC_BASE,RTC_WEEK_ALARM) ;
    tmp &= ~(0xf0);
    tmp |= (level<<4);
    __sREG32(RTC_BASE,RTC_WEEK_ALARM) = tmp;
}

inline void silan_rtc_reg_set(uint32_t offset, uint8_t value)
{
    __sREG32(RTC_BASE, offset) = value;
}

inline uint32_t silan_rtc_reg_get(uint32_t offset)
{
    return (__sREG32(RTC_BASE, offset));
}

inline void silan_rtcmem_write(uint32_t offset, uint8_t value)
{
    __sREG32(SILAN_RTCMEM_BASE, (offset<<2)) = value;
}

inline uint32_t silan_rtcmem_read(uint32_t offset)
{
    return __sREG32(SILAN_RTCMEM_BASE, (offset<<2));
}

void silan_rtc_init(void)
{
    silan_vlsp_cclk_config(CLK_ON);
    silan_rtcmem_ldo_onoff(1);
    //pvt
    silan_rtc_reg_get(RTC_CLKOUT_CON);
}
