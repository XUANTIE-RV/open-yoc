/*
 * Copyright (C) 2020 FishSemi Inc.
 */

/******************************************************************************
 * @file     ck_rtc.c
 * @brief    CSI Source File for RTC Driver
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include <stdbool.h>
#include <string.h>
#include "csi_core.h"
#include "drv/rtc.h"
#include "drv/irq.h"
#include "soc.h"

#define ERR_RTC(errno) (CSI_DRV_ERRNO_RTC_BASE | errno)

struct song_rtc_s {
    __IOM uint32_t RTC_CTRL1;
    __IOM uint32_t RTC_CTRL2;
    __IOM uint32_t INTERRUPT_SRC;
};

typedef struct {
    uint32_t base;
    uint32_t irq;
    rtc_event_cb_t cb_event;

    struct tm tblock;
} ck_rtc_priv_t;

extern int32_t target_get_rtc(int32_t idx, uint32_t *base, uint32_t *irq, void **handler);

static ck_rtc_priv_t rtc_instance[CONFIG_RTC_NUM];

static const rtc_capabilities_t rtc_capabilities = {
    .interrupt_mode = 1,    /* supports Interrupt mode */
    .wrap_mode = 0          /* supports wrap mode */
};

static uint8_t leap_year[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint8_t noleap_year[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static inline int clock_isleapyear(int year)
{
    return (year % 400) ? ((year % 100) ? ((year % 4) ? 0 : 1) : 0) : 1;
}

static int ck_check_tm_ok(struct tm *rtctime)
{
    int32_t leap = 1;

    if (rtctime->tm_year < 100 || rtctime->tm_year > 227) /* only 800 */ {
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

void rtc_irqhandler(int32_t idx)
{
    ck_rtc_priv_t *rtc_priv = &rtc_instance[idx];
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    if (base->INTERRUPT_SRC & (1UL << 4))
    {
        base->INTERRUPT_SRC |= (1UL << 4); /* clear rtc interrupt */

        if (rtc_priv->cb_event) {
            rtc_priv->cb_event(idx, RTC_EVENT_TIMER_INTRERRUPT);
        }
    }
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
    uint32_t irq;
    void *handler;

    real_idx = target_get_rtc(idx, &base, &irq, &handler);

    if (real_idx != idx) {
        return NULL;
    }

    ck_rtc_priv_t *rtc_priv;

    rtc_priv = &rtc_instance[idx];
    rtc_priv->base = base;
    rtc_priv->irq = irq;
    rtc_priv->cb_event = cb_event;

    drv_irq_register(rtc_priv->irq, handler);
    drv_irq_enable(rtc_priv->irq);

    return rtc_priv;
}

/**
  \brief       De-initialize RTC Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
 */
int32_t csi_rtc_uninitialize(rtc_handle_t handle)
{
    if (!handle)
        return ERR_RTC(DRV_ERROR_PARAMETER);

    ck_rtc_priv_t *rtc_priv = handle;

    rtc_priv->cb_event = NULL;
    drv_irq_disable(rtc_priv->irq);
    drv_irq_unregister(rtc_priv->irq);

    return 0;
}

int32_t csi_rtc_power_control(rtc_handle_t handle, csi_power_stat_e state)
{
    return ERR_RTC(DRV_ERROR_UNSUPPORTED);
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
    if (!handle || !rtctime)
        return ERR_RTC(DRV_ERROR_PARAMETER);

    int32_t ret = ck_check_tm_ok((struct tm *)rtctime);

    if (ret < 0) {
        return ret;
    }

    uint8_t run = 0;
    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    if (base->RTC_CTRL2 & (1 << 16))
    {
        run = 1;
        base->RTC_CTRL2 &= ~(1 << 16);
    }

    /*
        s:0~59
        m:0~59
        h:0~23
        d:1~31
        m:1~12
        y:0~127 (base:2000)
    */
    base->RTC_CTRL1 = rtctime->tm_sec | (rtctime->tm_min  << 8) | (rtctime->tm_hour << 16) | (rtctime->tm_mday << 24);
    base->RTC_CTRL2 = (rtctime->tm_mon + 1) | ((rtctime->tm_year + 1900 - 2000) << 8);

    if (run)
        base->RTC_CTRL2 |= (1 << 16);

    memcpy(&rtc_priv->tblock, rtctime, sizeof(struct tm));

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
    if (!handle || !rtctime)
        return ERR_RTC(DRV_ERROR_PARAMETER);

    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    uint32_t ctrl1 = base->RTC_CTRL1;
	uint32_t ctrl2 = base->RTC_CTRL2;

    rtctime->tm_year = ((int)((int)ctrl2 & 0x00007f00) >> 8) + 2000 - 1900;
	rtctime->tm_mon  = (ctrl2 & 0x0000000f) - 1;
	rtctime->tm_mday = (ctrl1 & 0x1f000000) >> 24;
	rtctime->tm_hour = (ctrl1 & 0x001f0000) >> 16;
	rtctime->tm_min  = (ctrl1 & 0x00003f00) >>  8;
	rtctime->tm_sec  =  ctrl1 & 0x0000003f;

    return 0;
}

/**
  \brief       Start RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
 */
int32_t csi_rtc_start(rtc_handle_t handle)
{
    if (!handle)
        return ERR_RTC(DRV_ERROR_PARAMETER);

    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    csi_rtc_set_time(handle, &rtc_priv->tblock);

    base->RTC_CTRL2 |= (1 << 16);

    return 0;
}

/**
  \brief       Stop RTC timer.
  \param[in]   handle rtc handle to operate.
  \return      \ref execution_status
 */
int32_t csi_rtc_stop(rtc_handle_t handle)
{
    if (!handle)
        return ERR_RTC(DRV_ERROR_PARAMETER);

    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    base->RTC_CTRL2 &= ~(1 << 16);

    return 0;
}

/**
  \brief       Get RTC status.
  \param[in]   handle rtc handle to operate.
  \return      RTC status \ref rtc_status_t
 */
rtc_status_t csi_rtc_get_status(rtc_handle_t handle)
{
    rtc_status_t status = {0};

    if (!handle)
        return status;

    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    status.active = !!(base->RTC_CTRL2 & (1 << 16));

    return status;
}

/**
  \brief       config RTC timer.
  \param[in]   handle rtc handle to operate.
  \param[in]   rtctime time to wake up
  \return      error code
 */
int32_t csi_rtc_set_alarm(rtc_handle_t handle, const struct tm *rtctime)
{
    if (!handle || !rtctime)
        return ERR_RTC(DRV_ERROR_PARAMETER);

    int32_t ret = ck_check_tm_ok((struct tm *)rtctime);

    if (ret < 0) {
        return ret;
    }

    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    memcpy(&rtc_priv->tblock, rtctime, sizeof(struct tm));

    if (base->RTC_CTRL1 & (1UL << 31))
    {
        base->RTC_CTRL1 &= ~(1UL << 31);

        base->RTC_CTRL2 = (rtc_priv->tblock.tm_mon + 1) | ((rtc_priv->tblock.tm_year + 1900 - 2000) << 8) | (1UL << 16);
        base->RTC_CTRL1 =  rtc_priv->tblock.tm_sec | (rtc_priv->tblock.tm_min << 8) | (rtc_priv->tblock.tm_hour << 16) | (rtc_priv->tblock.tm_mday << 24) | (1UL << 31);
    }

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
    if (!handle)
        return ERR_RTC(DRV_ERROR_PARAMETER);

    ck_rtc_priv_t *rtc_priv = handle;
    struct song_rtc_s *base = (struct song_rtc_s *)rtc_priv->base;

    if (en)
    {
        base->RTC_CTRL2 = (rtc_priv->tblock.tm_mon + 1) | ((rtc_priv->tblock.tm_year + 1900 - 2000) << 8) | (1UL << 16);
        base->RTC_CTRL1 = rtc_priv->tblock.tm_sec | (rtc_priv->tblock.tm_min << 8) | (rtc_priv->tblock.tm_hour << 16) | (rtc_priv->tblock.tm_mday << 24) | (1UL << 31);
    }
    else
    {
        base->RTC_CTRL1 &= ~(1UL << 31);
    }

    return 0;
}

