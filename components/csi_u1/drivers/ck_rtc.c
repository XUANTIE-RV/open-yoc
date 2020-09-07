/*
 * Copyright (C) 2020 FishSemi Inc.
 */

/******************************************************************************
 * @file     ck_rtc.c
 * @brief    CSI Source File for RTC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <stdbool.h>
#include <string.h>
#include "ck_rtc.h"
#include "csi_core.h"
#include "drv/rtc.h"
#include "drv/irq.h"
#include "soc.h"

/* ALARMID CK/AP use RTC index 1 */
#define ALARMID     1

#define ERR_RTC(errno) (CSI_DRV_ERRNO_RTC_BASE | errno)
#define RTC_BUSY_TIMEOUT        0x10000000
#define RTC_NULL_PARAM_CHK(para)    HANDLE_PARAM_CHK(para, ERR_RTC(DRV_ERROR_PARAMETER))

struct song_rtc_alarm_s
{
    volatile uint32_t INT_STATUS;
    volatile uint32_t INT_MASK;
    volatile uint32_t INT_EN;
    volatile uint32_t CNT_LO;
    volatile uint32_t CNT_HI;
    volatile uint32_t INT_UPDATE;
    volatile uint32_t RESERVED[10];
};

struct song_rtc_s
{
    volatile uint32_t SET_CNT1;
    volatile uint32_t SET_CNT2;
    volatile uint32_t SET_UPDATE;
    volatile uint32_t RESERVED0;
    volatile uint32_t SET_CYC;
    volatile uint32_t SET_CLK32K_ADJ;
    volatile uint32_t CALI_UPDATE;
    volatile uint32_t VERSION;
    volatile uint32_t RTC_CTL;
    volatile uint32_t USER_DEFINED;
    volatile uint32_t RESERVED1[6];
    struct song_rtc_alarm_s ALARM[];
};

typedef struct {
#ifdef CONFIG_LPM
    uint8_t rtc_power_status;
    uint32_t rtc_regs_saved[3];
#endif
    uint32_t base;
    uint32_t irq;
    rtc_event_cb_t cb_event;
} ck_rtc_priv_t;

extern int32_t target_get_rtc(int32_t idx, uint32_t *base, int32_t *irq, void **handler);

static ck_rtc_priv_t rtc_instance[CONFIG_RTC_NUM];
static uint8_t leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint8_t noleap_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint16_t g_noleap_daysbeforemonth[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
//static const uint16_t g_leap_daysbeforemonth[13] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};

static const rtc_capabilities_t rtc_capabilities = {
    .interrupt_mode = 1,    /* supports Interrupt mode */
    .wrap_mode = 0          /* supports wrap mode */
};

static inline int clock_isleapyear(int year)
{
    return (year % 400) ? ((year % 100) ? ((year % 4) ? 0 : 1) : 0) : 1;
}

static int ck_check_tm_ok(struct tm *rtctime)
{
    int32_t leap = 1;

    if (rtctime->tm_year < 70 || rtctime->tm_year >= 200) {
        goto error_time;
    }

    leap = clock_isleapyear(rtctime->tm_year + 1900);

    if (rtctime->tm_sec < 0 || rtctime->tm_sec >= 60) {
        goto error_time;
    }

    if (rtctime->tm_min < 0 || rtctime->tm_min >= 60) {
        goto error_time;
    }

    if (rtctime->tm_hour < 0 || rtctime->tm_hour >= 24) {
        goto error_time;
    }

    if (rtctime->tm_mon < 0 || rtctime->tm_mon >= 12) {
        goto error_time;
    }

    if (leap) {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > leap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    } else {
        if (rtctime->tm_mday < 1 || rtctime->tm_mday > noleap_year[rtctime->tm_mon]) {
            goto error_time;
        }
    }

    return 0;
error_time:
    return ERR_RTC(RTC_ERROR_TIME);

}

static int clock_daysbeforemonth(int month, bool leapyear)
{
    int retval = g_noleap_daysbeforemonth[month];

    if (month >= 2 && leapyear) {
        retval++;
    }

    return retval;
}

static time_t clock_calendar2utc(int year, int month, int day)
{
    time_t days;

    /* Years since epoch in units of days (ignoring leap years). */

    days = (year - 1970) * 365;

    /* Add in the extra days for the leap years prior to the current year. */

    days += (year - 1969) >> 2;

    /* Add in the days up to the beginning of this month. */

    days += (time_t)clock_daysbeforemonth(month, clock_isleapyear(year));

    /* Add in the days since the beginning of this month (days are 1-based). */

    days += day - 1;

    /* Then convert the seconds and add in hours, minutes, and seconds */

    return days;
}

time_t mktime(struct tm *tp)
{
    time_t ret;
    time_t jdn;

    /* Get the EPOCH-relative julian date from the calendar year,
     * month, and date
     */

    ret = ck_check_tm_ok(tp);

    if (ret < 0) {
        return -1;
    }

    jdn = clock_calendar2utc(tp->tm_year + 1900, tp->tm_mon, tp->tm_mday);

    /* Return the seconds into the julian day. */

    ret = ((jdn * 24 + tp->tm_hour) * 60 + tp->tm_min) * 60 + tp->tm_sec;

    return ret;
}

static void clock_utc2calendar(time_t days, int *year, int *month,
        int *day)
{

    /* There is one leap year every four years, so we can get close with the
     * following:
     */

    int value   = days  / (4 * 365 + 1); /* Number of 4-years periods since the epoch */
    days   -= value * (4 * 365 + 1); /* Remaining days */
    value <<= 2;                     /* Years since the epoch */

    /* Then we will brute force the next 0-3 years */
    bool leapyear;
    int  tmp;

    for (; ;) {
        /* Is this year a leap year (we'll need this later too) */

        leapyear = clock_isleapyear(value + 1970);

        /* Get the number of days in the year */

        tmp = (leapyear ? 366 : 365);

        /* Do we have that many days? */

        if (days >= tmp) {
            /* Yes.. bump up the year */

            value++;
            days -= tmp;
        } else {
            /* Nope... then go handle months */

            break;
        }
    }

    /* At this point, value has the year and days has number days into this year */

    *year = 1970 + value;

    /* Handle the month (zero based) */
    int  min = 0;
    int  max = 11;

    do {
        /* Get the midpoint */

        value = (min + max) >> 1;

        /* Get the number of days that occurred before the beginning of the month
         * following the midpoint.
         */

        tmp = clock_daysbeforemonth(value + 1, leapyear);

        /* Does the number of days before this month that equal or exceed the
         * number of days we have remaining?
         */

        if (tmp > days) {
            /* Yes.. then the month we want is somewhere from 'min' and to the
             * midpoint, 'value'.  Could it be the midpoint?
             */

            tmp = clock_daysbeforemonth(value, leapyear);

            if (tmp > days) {
                /* No... The one we want is somewhere between min and value-1 */

                max = value - 1;
            } else {
                /* Yes.. 'value' contains the month that we want */

                break;
            }
        } else {
            /* No... The one we want is somwhere between value+1 and max */

            min = value + 1;
        }

        /* If we break out of the loop because min == max, then we want value
         * to be equal to min == max.
         */

        value = min;
    } while (min < max);

    /* The selected month number is in value. Subtract the number of days in the
     * selected month
     */

    days -= clock_daysbeforemonth(value, leapyear);

    /* At this point, value has the month into this year (zero based) and days has
     * number of days into this month (zero based)
     */

    *month = value + 1; /* 1-based */
    *day   = days + 1;  /* 1-based */
}

struct tm *gmtime_r(const time_t *timer, struct tm *result)
{
    time_t epoch;
    time_t jdn;
    int    year;
    int    month;
    int    day;
    int    hour;
    int    min;
    int    sec;

    /* Get the seconds since the EPOCH */

    epoch = *timer;

    /* Convert to days, hours, minutes, and seconds since the EPOCH */

    jdn    = epoch / SEC_PER_DAY;
    epoch -= SEC_PER_DAY * jdn;

    hour   = epoch / SEC_PER_HOUR;
    epoch -= SEC_PER_HOUR * hour;

    min    = epoch / SEC_PER_MIN;
    epoch -= SEC_PER_MIN * min;

    sec    = epoch;

    /* Convert the days since the EPOCH to calendar day */

    clock_utc2calendar(jdn, &year, &month, &day);


    /* Then return the struct tm contents */

    result->tm_year  = (int)year - 1900; /* Relative to 1900 */
    result->tm_mon   = (int)month - 1;   /* zero-based */
    result->tm_mday  = (int)day;         /* one-based */
    result->tm_hour  = (int)hour;
    result->tm_min   = (int)min;
    result->tm_sec   = (int)sec;
    return result;
}

void ck_rtc_irqhandler(int32_t idx)
{
    ck_rtc_priv_t *rtc_priv = &rtc_instance[idx];
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;
    struct song_rtc_alarm_s *alarm = &base->ALARM[ALARMID];

    alarm->INT_EN     = 0; /* Disable interrupt first */
    alarm->INT_STATUS = 1; /* Clear the request */
    alarm->INT_UPDATE = 1; /* Trigger the update */

    if (rtc_priv->cb_event) {
        rtc_priv->cb_event(idx, RTC_EVENT_TIMER_INTRERRUPT);
    }
}

#ifdef CONFIG_LPM
static void manage_clock(rtc_handle_t handle, uint8_t enable)
{
    if (handle == &rtc_instance[0]) {
        drv_clock_manager_config(CLOCK_MANAGER_RTC, enable);
    } else if (handle == &rtc_instance[1]) {
        drv_clock_manager_config(CLOCK_MANAGER_RTC1, enable);
    }
}

static void do_prepare_sleep_action(rtc_handle_t handle)
{
    ck_rtc_priv_t *rtc_priv = handle;
    uint32_t *rbase = (uint32_t *)(rtc_priv->base);
    registers_save(rtc_priv->rtc_regs_saved, rbase + 1, 3);
}

static void do_wakeup_sleep_action(rtc_handle_t handle)
{
    ck_rtc_priv_t *rtc_priv = handle;
    uint32_t *rbase = (uint32_t *)(rtc_priv->base);
    registers_restore(rbase + 1, rtc_priv->rtc_regs_saved, 3);
}
#endif

static uint32_t song_rtc_nsec2cnt(uint32_t nsec)
{
    uint32_t usec;

    usec = nsec / NSEC_PER_USEC;
    return 1024 * usec / USEC_PER_SEC;
}

static uint32_t song_rtc_cnt2nsec(uint32_t cnt)
{
    uint32_t usec;

    usec = USEC_PER_SEC * cnt / 1024;
    return NSEC_PER_USEC * usec;
}

/**
  \brief       Initialize RTC Interface. 1. Initializes the resources needed for the RTC interface 2.registers event callback function
  \param[in]   idx  rtc index
  \param[in]   cb_event  Pointer to \ref rtc_event_cb_t
  \return      pointer to rtc instance
 */
rtc_handle_t csi_rtc_initialize(int32_t idx, rtc_event_cb_t cb_event)
{
    if (idx < 0 || idx >= CONFIG_RTC_NUM) {
        return NULL;
    }

    int32_t real_idx;
    uint32_t base = 0u;
    int32_t irq;
    void *handler;

    real_idx = target_get_rtc(idx, &base, &irq, &handler);

    if (real_idx != idx) {
        return NULL;
    }

    ck_rtc_priv_t *rtc_priv;

    rtc_priv = &rtc_instance[idx];
    rtc_priv->base = base;
    rtc_priv->irq  = irq;
    rtc_priv->cb_event = cb_event;

#ifdef CONFIG_LPM
    csi_rtc_power_control(rtc_priv, DRV_POWER_FULL);
#endif

    csi_vic_clear_pending_irq(rtc_priv->irq);
    drv_irq_register(rtc_priv->irq, handler);
    drv_irq_enable(rtc_priv->irq);

    return (rtc_handle_t)rtc_priv;
}

/**
  \brief       De-initialize RTC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
 */
int32_t csi_rtc_uninitialize(rtc_handle_t handle)
{
    RTC_NULL_PARAM_CHK(handle);

    ck_rtc_priv_t *rtc_priv = handle;

    rtc_priv->cb_event = NULL;
    drv_irq_disable(rtc_priv->irq);
    drv_irq_unregister(rtc_priv->irq);

#ifdef CONFIG_LPM
    csi_rtc_power_control(rtc_priv, DRV_POWER_OFF);
#endif
    return 0;
}

int32_t csi_rtc_power_control(rtc_handle_t handle, csi_power_stat_e state)
{
    RTC_NULL_PARAM_CHK(handle);
#ifdef CONFIG_LPM
    power_cb_t callback = {
        .wakeup = do_wakeup_sleep_action,
        .sleep = do_prepare_sleep_action,
        .manage_clock = manage_clock
    };
    return drv_soc_power_control(handle, state, &callback);
#else
    return ERR_RTC(DRV_ERROR_UNSUPPORTED);
#endif
}

/**
  \brief       Get driver capabilities.
  \param[in]   idx  rtc index
  \return      \ref rtc_capabilities_t
 */
rtc_capabilities_t csi_rtc_get_capabilities(int32_t idx)
{
    if (idx < 0 || idx >= CONFIG_RTC_NUM) {
        rtc_capabilities_t ret;
        memset(&ret, 0, sizeof(rtc_capabilities_t));
        return ret;
    }

    return rtc_capabilities;
}

/**
  \brief       Set RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime \ref struct tm
  \return      \ref execution_status
 */

int32_t csi_rtc_set_time(rtc_handle_t handle, const struct tm *rtctime)
{
    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    base->SET_CNT2     = mktime((struct tm *)rtctime);
    base->SET_CNT1     = 0;
    base->USER_DEFINED = 1; /* Mark the change */
    base->SET_UPDATE   = 1; /* Trigger the update */

    return 0;
}

/**
  \brief       Get RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime \ref struct tm
  \return      \ref execution_status
 */
int32_t csi_rtc_get_time(rtc_handle_t handle, struct tm *rtctime)
{
    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;
    uint32_t cnt2;

    do {
        cnt2 = base->SET_CNT2;
    }
    while (cnt2 != base->SET_CNT2);

    gmtime_r((const time_t *)&cnt2, rtctime);

    return 0;
}

/**
  \brief       Start RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
 */
int32_t csi_rtc_start(rtc_handle_t handle)
{
    return 0;
}

/**
  \brief       Stop RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
 */
int32_t csi_rtc_stop(rtc_handle_t handle)
{
    return 0;
}

/**
  \brief       Get RTC status.
  \param[in]   handle rtc handle to operate.
  \return      RTC status \ref rtc_status_t
 */
rtc_status_t csi_rtc_get_status(rtc_handle_t handle)
{
    rtc_status_t rtc_status = {
        .active = 1,
    };

    return rtc_status;
}

/**
  \brief       config RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime time to wake up
  \return      error code
 */
int32_t csi_rtc_set_alarm(rtc_handle_t handle, const struct tm *rtctime)
{
    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;
    struct song_rtc_alarm_s *alarm = &base->ALARM[ALARMID];

    alarm->CNT_HI     = mktime((struct tm *)rtctime);
    alarm->CNT_LO     = 0;
    alarm->INT_EN     = 1; /* Then enable interrupt */
    alarm->INT_UPDATE = 1; /* Trigger the update */

    return 0;
}

/**
  \brief       disable or enable RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   flag  1 - enable rtc alarm 0 - disable rtc alarm
  \return      error code
 */
int32_t csi_rtc_enable_alarm(rtc_handle_t handle, uint8_t en)
{
    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;
    struct song_rtc_alarm_s *alarm = &base->ALARM[ALARMID];

    if (!en) {
        alarm->INT_EN     = 0; /* Disable interrupt first */
        alarm->INT_STATUS = 1; /* Clear the request */
        alarm->INT_UPDATE = 1; /* Trigger the update */
    }

    return 0;
}

