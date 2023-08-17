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

#define CHECK_RETURN(ret)                                      \
    do                                                         \
    {                                                          \
        if (ret != 0)                                          \
        {                                                      \
            printf("check ret failed in %d line\n", __LINE__); \
            return -1;                                         \
        }                                                      \
    } while (0);

#define RTC_PORT_NUM 0

volatile static int hal_rtc_flag;
void hal_rtc_callback(rvm_dev_t *dev, void *arg)
{
    hal_rtc_flag = 1;
}

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
    time_buf.tm_hour    = 14;
    time_buf.tm_mday    = 24;
    time_buf.tm_mon     = 11;
    time_buf.tm_year    = 123;

    /* set rtc time */
    ret = rvm_hal_rtc_set_time(rtc_dev, &time_buf);
    if (ret != 0) {
        printf("rvm_hal_rtc_set_time fail !\n");
        goto failure;
    }

    time_buf.tm_sec     = 5;
    time_buf.tm_min     = 45;
    time_buf.tm_hour    = 14;
    time_buf.tm_mday    = 24;
    time_buf.tm_mon     = 11;
    time_buf.tm_year    = 123;

    /* set rtc time */
    ret = rvm_hal_rtc_set_alarm(rtc_dev, &time_buf, hal_rtc_callback, NULL);
    if (ret != 0) {
        printf("rvm_hal_rtc_set_time fail !\n");
        goto failure;
    }

    /* delay 3s */
    aos_msleep(3000);
    uint32_t reamin_time = rvm_hal_rtc_get_alarm_remaining_time(rtc_dev);
    if (!((reamin_time > 0)&&(reamin_time < 4))) { // 这个地方涉及底层对计数器的值的处理，目前处理不恰当导致的错误
        printf("rvm_hal_rtc_get_alarm_remaining_time test fail !\n");
        goto failure;
    }

    /* delay 8s */
    aos_msleep(8000);

    if (hal_rtc_flag != 1) {
        printf("rvm_hal_rtc_set_alarm test fail !\n");
        goto failure;
    }

    memset(&time_buf, 0, sizeof(struct tm));

    /* get rtc current time */
    ret = rvm_hal_rtc_get_time(rtc_dev, &time_buf);
    if (ret != 0) {
        printf("rvm_hal_rtc_get_time fail !\n");
        goto failure;
    }

    /* close rtc */
    rvm_hal_rtc_close(rtc_dev);
    if (!(time_buf.tm_sec >= 10 && time_buf.tm_sec <= 12)) {
        CHECK_RETURN(time_buf.tm_sec - 11);
    }
    CHECK_RETURN(time_buf.tm_min - 45);
    CHECK_RETURN(time_buf.tm_hour - 14);
    CHECK_RETURN(time_buf.tm_mday - 24);
    CHECK_RETURN(time_buf.tm_mon - 11);
    CHECK_RETURN(time_buf.tm_year - 123);

    printf("rtc demo successfully !\n");
    return 0;
failure:
    rvm_hal_rtc_close(rtc_dev);
    return -1;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

volatile static int devfs_rtc_flag;
void devfs_rtc_callback(rvm_dev_t *dev, void *arg)
{
    devfs_rtc_flag = 1;
}

int devfs_rtc_demo(void)
{
    printf("devfs_rtc_demo start\n");

    int ret = -1;

    struct tm   time_buf;

    rvm_rtc_drv_register(0);

    char *rtcdev = "/dev/rtc0";

    int fd = open(rtcdev, O_RDWR);
    printf("open rtc0 fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", rtcdev, fd);
        return -1;
    }

    time_buf.tm_sec     = 0;
    time_buf.tm_min     = 45;
    time_buf.tm_hour    = 14;
    time_buf.tm_mday    = 24;
    time_buf.tm_mon     = 11;
    time_buf.tm_year    = 123;

    /* set rtc time */
    ret = ioctl(fd, RTC_IOC_SET_TIME, &time_buf);
    if (ret < 0) {
        printf("RTC_IOC_SET_TIME fail !\n");
        goto failure;
    }

    /* set rtc alarm */
    rvm_rtc_dev_msg_t msg;
    time_buf.tm_sec     = 5;
    time_buf.tm_min     = 45;
    time_buf.tm_hour    = 14;
    time_buf.tm_mday    = 24;
    time_buf.tm_mon     = 11;
    time_buf.tm_year    = 123;
    msg.time            = &time_buf;
    msg.callback        = devfs_rtc_callback;
    msg.arg             = NULL;

    ret = ioctl(fd, RTC_IOC_SET_ALARM, &msg);
    if (ret < 0) {
        printf("RTC_IOC_SET_ALARM fail !\n");
        goto failure;
    }

    /* delay 3s */
    aos_msleep(3000);

    uint32_t remaining_time;
    ret = ioctl(fd, RTC_IOC_GET_ALARM_REMAINING_TIME, &remaining_time);
    if (ret < 0) {
        printf("RTC_IOC_GET_ALARM_REMAINING_TIME fail !\n");
        goto failure;
    }

    if (!((remaining_time > 0)&&(remaining_time < 4)))  {
        printf("RTC_IOC_GET_ALARM_REMAINING_TIME test fail !\n");
        goto failure;
    }

    /* delay 8s */
    aos_msleep(8000);

    if (devfs_rtc_flag != 1) {
        printf("RTC_IOC_SET_ALARM test fail !\n");
        goto failure;
    }

    devfs_rtc_flag = 0;

    memset(&time_buf, 0, sizeof(struct tm));

    /* get rtc current time */
    ret = ioctl(fd, RTC_IOC_GET_TIME, &time_buf);
    if (ret < 0) {
        printf("RTC_IOC_GET_TIME fail !\n");
        goto failure;
    }

    /* close rtc */
    close(fd);
    if (!(time_buf.tm_sec >= 10 && time_buf.tm_sec <= 12)) {
        CHECK_RETURN(time_buf.tm_sec - 11);
    }
    CHECK_RETURN(time_buf.tm_min - 45);
    CHECK_RETURN(time_buf.tm_hour - 14);
    CHECK_RETURN(time_buf.tm_mday - 24);
    CHECK_RETURN(time_buf.tm_mon - 11);
    CHECK_RETURN(time_buf.tm_year - 123);

    printf("devfs rtc demo successfully !\n");
    return 0;
failure:
    close(fd);
    return -1;
}

int devfs_rtc_cancel_alarm_demo(void)
{
    printf("devfs_rtc_cancel_alarm_demo start\n");

    int ret = -1;

    struct tm   time_buf;

    rvm_rtc_drv_register(0);

    char *rtcdev = "/dev/rtc0";

    int fd = open(rtcdev, O_RDWR);
    printf("open rtc0 fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", rtcdev, fd);
        return -1;
    }

    time_buf.tm_sec     = 0;
    time_buf.tm_min     = 45;
    time_buf.tm_hour    = 14;
    time_buf.tm_mday    = 24;
    time_buf.tm_mon     = 11;
    time_buf.tm_year    = 123;

    /* set rtc time */
    ret = ioctl(fd, RTC_IOC_SET_TIME, &time_buf);
    if (ret < 0) {
        printf("RTC_IOC_SET_TIME fail !\n");
        goto failure;
    }

    /* set rtc alarm */
    rvm_rtc_dev_msg_t msg;
    time_buf.tm_sec     = 5;
    time_buf.tm_min     = 45;
    time_buf.tm_hour    = 14;
    time_buf.tm_mday    = 24;
    time_buf.tm_mon     = 11;
    time_buf.tm_year    = 123;
    msg.time            = &time_buf;
    msg.callback        = devfs_rtc_callback;
    msg.arg             = NULL;

    ret = ioctl(fd, RTC_IOC_SET_ALARM, &msg);
    if (ret < 0) {
        printf("RTC_IOC_SET_ALARM fail !\n");
        goto failure;
    }

    /* delay 3s */
    aos_msleep(3000);

    ret = ioctl(fd, RTC_IOC_CANCEL_ALARM, &msg);
    if (ret < 0) {
        printf("RTC_IOC_CANCEL_ALARM fail !\n");
        goto failure;
    }

    /* delay 3s */
    aos_msleep(3000);

    if (devfs_rtc_flag == 1) {
        printf("RTC_IOC_CANCEL_ALARM test fail !\n");
        goto failure;
    }

    /* close rtc */
    close(fd);
    printf("devfs rtc cancel alarm successfully !\n");
    return 0;
failure:
    close(fd);
    return -1;
}

#endif
