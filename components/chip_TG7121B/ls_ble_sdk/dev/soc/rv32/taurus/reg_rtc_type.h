#ifndef REG_RTC_TYPE_H_
#define REG_RTC_TYPE_H_
#include <stdint.h>

typedef struct
{
    volatile uint32_t RTC_CTRL;
    volatile uint32_t RTC_CALIB;
    volatile uint32_t RTC_SET_TGT0;
    volatile uint32_t RTC_SET_TGT1;
    volatile uint32_t RTC_CUR0;
    volatile uint32_t RTC_CUR1;
    volatile uint32_t RTC_INTR;
}reg_rtc_t;

enum RTC_REG_RTC_CTRL_FIELD
{
    RTC_RTC_EN_MASK = (int)0x1,
    RTC_RTC_EN_POS = 0,
    RTC_CALIB_EN_MASK = (int)0x2,
    RTC_CALIB_EN_POS = 1,
    RTC_RTC_ALARM_EN_MASK = (int)0x4,
    RTC_RTC_ALARM_EN_POS = 2,
    RTC_RTC_SET_TGGL_MASK = (int)0x8,
    RTC_RTC_SET_TGGL_POS = 3,
    RTC_RTC_INTR_CLR_MASK = (int)0x10,
    RTC_RTC_INTR_CLR_POS = 4,
};

enum RTC_REG_RTC_CALIB_FIELD
{
    RTC_CYC_1HZ_M1_MASK = (int)0xffff,
    RTC_CYC_1HZ_M1_POS = 0,
    RTC_CALIB_CYC_MASK = (int)0x3f0000,
    RTC_CALIB_CYC_POS = 16,
};

enum RTC_REG_RTC_SET_TGT0_FIELD
{
    RTC_RTC_SET_SEC_MASK = (int)0x3f,
    RTC_RTC_SET_SEC_POS = 0,
    RTC_RTC_SET_MIN_MASK = (int)0x3f00,
    RTC_RTC_SET_MIN_POS = 8,
    RTC_RTC_SET_HOUR_MASK = (int)0x10000,
    RTC_RTC_SET_HOUR_POS = 16,
    RTC_RTC_SET_DAY_MASK = (int)0x1f000000,
    RTC_RTC_SET_DAY_POS = 24,
};

enum RTC_REG_RTC_SET_TGT1_FIELD
{
    RTC_RTC_SET_WEEK_MASK = (int)0x7,
    RTC_RTC_SET_WEEK_POS = 0,
    RTC_RTC_SET_MON_MASK = (int)0xf0,
    RTC_RTC_SET_MON_POS = 4,
    RTC_RTC_SET_YEAR_MASK = (int)0xff00,
    RTC_RTC_SET_YEAR_POS = 8,
};

enum RTC_REG_RTC_CUR0_FIELD
{
    RTC_RTC_CUR_SEC_MASK = (int)0x3f,
    RTC_RTC_CUR_SEC_POS = 0,
    RTC_RTC_CUR_MIN_MASK = (int)0x3f00,
    RTC_RTC_CUR_MIN_POS = 8,
    RTC_RTC_CUR_HOUR_MASK = (int)0x1f0000,
    RTC_RTC_CUR_HOUR_POS = 16,
    RTC_RTC_CUR_DAY_MASK = (int)0x1f000000,
    RTC_RTC_CUR_DAY_POS = 24,
};

enum RTC_REG_RTC_CUR1_FIELD
{
    RTC_RTC_CUR_WEEK_MASK = (int)0x7,
    RTC_RTC_CUR_WEEK_POS = 0,
    RTC_RTC_CUR_MON_MASK = (int)0xf0,
    RTC_RTC_CUR_MON_POS = 4,
    RTC_RTC_CUR_YEAR_MASK = (int)0xff00,
    RTC_RTC_CUR_YEAR_POS = 8,
};

enum RTC_REG_RTC_INTR_FIELD
{
    RTC_RTC_INTR_MASK = (int)0x1,
    RTC_RTC_INTR_POS = 0,
};

#endif


