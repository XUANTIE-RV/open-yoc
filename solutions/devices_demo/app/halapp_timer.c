/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/timer.h>
#include <devices/devicelist.h>

volatile static bool intr_flag = false;
static void hal_timer_int_fun(rvm_dev_t *dev, void *arg)
{
    intr_flag = true;

    return;
}

int hal_timer_demo(void)
{
    printf("hal_timer_demo start\n");
    
    int ret = -1;

    rvm_timer_drv_register(1);

    rvm_dev_t *timer_dev = rvm_hal_timer_open("timer1");

    ret = rvm_hal_timer_attach_callback(timer_dev, hal_timer_int_fun, NULL);

    if (ret != 0) {
        printf("rvm_hal_timer_attach_callback error\n");
        ret = -1;
    }

    ret = rvm_hal_timer_start(timer_dev, 3000000);

    if (ret != 0) {
        printf("rvm_hal_timer_start error\n");
        ret = -1;
    }
#if !defined(CONFIG_CHIP_D1)
    uint32_t load_value = rvm_hal_timer_get_load_value(timer_dev);

    if (load_value == 0) {
        printf("rvm_hal_timer_start error\n");
        ret = -1;
    }

    aos_msleep(1000);

    uint32_t remaining_value = rvm_hal_timer_get_remaining_value(timer_dev);

    if (remaining_value == 0) {
        printf("rvm_hal_timer_start error\n");
        ret = -1;
    }
#endif
    aos_msleep(4000);

    if (intr_flag == 0) {
        printf("timer demo fail!\n");
    }

    ret = rvm_hal_timer_stop(timer_dev);

    if (ret != 0) {
        printf("rvm_hal_timer_stop error\n");
        ret = -1;
    }

    ret = rvm_hal_timer_detach_callback(timer_dev);

    if (ret != 0) {
        printf("rvm_hal_timer_detach_callback error\n");
        ret = -1;
    }

    rvm_hal_timer_close(timer_dev);

    printf("timer demo successfully!\n");

    return 0;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

volatile static int devfs_timer_flag;
void devfs_timer_callback(rvm_dev_t *dev, void *arg)
{
    devfs_timer_flag = 1;
}

int devfs_timer_demo(void)
{
    printf("devfs_timer_demo start\n");

    int ret = -1;

    rvm_timer_drv_register(0);

    char *timerdev = "/dev/timer1";

    int fd = open(timerdev, O_RDWR);
    printf("open rtc0 fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", timerdev, fd);
        return -1;
    }

    rvm_timer_dev_msg_t msg_timer;
    msg_timer.callback = devfs_timer_callback;
    msg_timer.arg = NULL;
    ret = ioctl(fd, TIMER_IOC_ATTACH_CALLBACK, &msg_timer);
    if (ret < 0) {
        printf("TIMER_IOC_ATTACH_CALLBACK fail !\n");
        goto failure;
    }

    uint32_t timeout_us = 3000000;
    ret = ioctl(fd, TIMER_IOC_START, &timeout_us);
    if (ret < 0) {
        printf("TIMER_IOC_START fail !\n");
        goto failure;
    }
#if !defined(CONFIG_CHIP_D1)
    uint32_t load_value;
    ret = ioctl(fd, TIMER_IOC_GET_LOAD_VALUE, &load_value);
    if (load_value <= 0) {
        printf("TIMER_IOC_GET_LOAD_VALUE fail !\n");
        goto failure;
    }

    aos_msleep(1000);

    uint32_t remaining_value;
    ret = ioctl(fd, TIMER_IOC_GET_REMAINING_VALUE, &remaining_value);
    if (remaining_value <= 0) {
        printf("TIMER_IOC_GET_REMAINING_VALUE fail !\n");
        goto failure;
    }
#endif
    aos_msleep(4000);

    if (devfs_timer_flag == 0) {
        printf("devfs timer demo fail!\n");
    }

    ret = ioctl(fd, TIMER_IOC_STOP);
    if (ret < 0) {
        printf("TIMER_IOC_STOP fail !\n");
        goto failure;
    }

    ret = ioctl(fd, TIMER_IOC_DETACH_CALLBACK);
    if (ret < 0) {
        printf("TIMER_IOC_DETACH_CALLBACK fail !\n");
        goto failure;
    }

    close(fd);
    printf("devfs timer demo successfully !\n");
    return 0;
failure:
    close(fd);
    return -1;
}

#endif