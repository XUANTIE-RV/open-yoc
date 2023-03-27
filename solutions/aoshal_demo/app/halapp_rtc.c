/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "aos/hal/rtc.h"
#include "app_main.h"

#define CHECK_RETURN(ret)                           \
        do {                                        \
            if (ret != 0) {                         \
                return -1;                          \
            }                                       \
        } while(0);

#define RTC_PORT_NUM 0

/* define dev */
rtc_dev_t rtc;

int hal_rtc_demo(void)
{
    printf("hal_rtc_demo start\n");
    
    int ret = -1;

    rtc_time_t   time_buf;

    /* rtc port set */
    rtc.port = RTC_PORT_NUM;

    /* set to DEC format */
    rtc.config.format = HAL_RTC_FORMAT_DEC;

    /* init rtc with the given settings */
    ret = hal_rtc_init(&rtc);
    if (ret != 0) {
        printf("rtc init error !\n");
    }

    time_buf.sec     = 0;
    time_buf.min     = 45;
    time_buf.hr      = 14;
    time_buf.date    = 24;
    time_buf.month   = 11;
    time_buf.year    = 20;

    /* set rtc time */
    ret = hal_rtc_set_time(&rtc, &time_buf);
    if (ret != 0) {
        printf("rtc set time error !\n");
    }

    /* delay 10s */
    aos_msleep(10000);

    memset(&time_buf, 0, sizeof(rtc_time_t));

    /* get rtc current time */
    ret = hal_rtc_get_time(&rtc, &time_buf);
    if (ret != 0) {
        printf("rtc get time error !\n");
    }

    CHECK_RETURN(time_buf.sec - 10);
    CHECK_RETURN(time_buf.min - 45);
    CHECK_RETURN(time_buf.hr - 14);
    CHECK_RETURN(time_buf.date - 24);
    CHECK_RETURN(time_buf.month - 11);
    CHECK_RETURN(time_buf.year - 20);

    /* finalize rtc */
    hal_rtc_finalize(&rtc);

    printf("rtc demo successfully !\n");

    while(1) {
        /* sleep 500ms */
        aos_msleep(500);
    };
}
