/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/rtc.h>
#include <devices/devicelist.h>

#define CHECK_RETURN(ret)                           \
        do {                                        \
            if (ret != 0) {                         \
                return -1;                          \
            }                                       \
        } while(0);

#define RTC_PORT_NUM 0

int hal_rtc_demo(void)
{
    printf("hal_rtc_demo start\n");
    
    int ret = -1;

    struct tm   time_buf;

    rvm_rtc_drv_register(0);

    rvm_dev_t *rtc_dev = NULL;
    char *filename = "rtc0";

    rtc_dev = rvm_hal_rtc_open(filename);

    time_buf.tm_sec     = 0;
    time_buf.tm_min     = 45;
    time_buf.tm_hour      = 14;
    time_buf.tm_mday    = 24;
    time_buf.tm_mon   = 11;
    time_buf.tm_year    = 123;

    /* set rtc time */
    ret = rvm_hal_rtc_set_time(rtc_dev, &time_buf);
    if (ret != 0) {
        printf("rvm_hal_rtc_set_time fail !\n");
        return -1;
    }

    /* delay 10s */
    aos_msleep(10000);

    memset(&time_buf, 0, sizeof(struct tm));

    /* get rtc current time */
    ret = rvm_hal_rtc_get_time(rtc_dev, &time_buf);
    if (ret != 0) {
        printf("rvm_hal_rtc_get_time fail !\n");
        return -1;
    }

    CHECK_RETURN(time_buf.tm_sec - 10);
    CHECK_RETURN(time_buf.tm_min - 45);
    CHECK_RETURN(time_buf.tm_hour - 14);
    CHECK_RETURN(time_buf.tm_mday - 24);
    CHECK_RETURN(time_buf.tm_mon - 11);
    CHECK_RETURN(time_buf.tm_year - 123);

    /* close rtc */
    rvm_hal_rtc_close(rtc_dev);

    printf("rtc demo successfully !\n");

    return 0;
}
