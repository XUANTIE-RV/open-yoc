/*
 * silan_rtc.h
 *
 * Modify Date: 2016-4-28 18:10
 * MaintainedBy: yerenfeng <yerenfeng@silan.com.cn>
 */
 
#ifndef __SILAN_RTC_H__
#define __SILAN_RTC_H__

#include "silan_types.h"

/*
 * set rtc register
 * offset:register offset 
 */
void silan_rtc_reg_set(uint32_t offset, uint8_t value);
/*
 *read rtc register
 */
uint32_t silan_rtc_reg_get(uint32_t offset);
void silan_soc_ldo_config(uint8_t level);
void silan_rtc_init(void);


#endif //__SILAN_RTC_H__


