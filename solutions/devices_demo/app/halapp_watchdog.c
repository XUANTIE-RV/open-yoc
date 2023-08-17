/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include <aos/aos.h>
#include "app_main.h"

#include <devices/device.h>
#include <devices/driver.h>
#include <devices/wdt.h>
#include <devices/devicelist.h>

/* task parameters */
#define TASK_FEEDWDG_NAME      "feedwdg"
#define TASK_FEEDWDG_STACKSIZE 4096
#define TASK_FEEDWDG_PRI       32

/* timeout */
#define WDT_TIMEOUT            8000

/* task handle */
static aos_task_t task_feedwdg;

static volatile int task_quit;

/* wdt handle */
static rvm_dev_t *wdt_dev = NULL;

/* task entry */
static void task_feedwdg_entry(void *arg)
{
    /**
     * The task feed the watchdog every 1000ms. The feeding interval must be less than the
     * watchdog timeout, otherwise it will trigger by mistake.
     */
    while (!task_quit) {
        printf("feed the watchdog!\r\n");
        rvm_hal_wdt_feed(wdt_dev);
        aos_msleep(1000);
#if !defined(CONFIG_CHIP_D1) && !defined(CONFIG_CHIP_CH2601) && !defined(CVI_SOC_CV181XC)
        uint32_t remaining_time = 0;
        remaining_time = rvm_hal_wdt_get_remaining_time(wdt_dev);
        printf("remaining_time: %d\n", remaining_time);
#endif
    }
}

int hal_watchdog_demo(void)
{
    int ret;
    char *filename = "wdt0";

    rvm_wdt_drv_register(0);

    wdt_dev = rvm_hal_wdt_open(filename);
    if (wdt_dev == NULL) {
        printf("rvm_hal_wdt_open %s error!\r\n", filename);
        return -1;
    }

    ret = rvm_hal_wdt_set_timeout(wdt_dev, WDT_TIMEOUT);
    if (ret != 0) {
        printf("rvm_hal_wdt_set_timeout error!\r\n");
        goto failure;
    }

    ret = rvm_hal_wdt_start(wdt_dev);
    if (ret != 0) {
        printf("rvm_hal_wdt_start error!\r\n");
        goto failure;
    }

    /* Create the task to feed the watchdog */
    task_quit = 0;
    ret = aos_task_new_ext(&task_feedwdg, (const char *)TASK_FEEDWDG_NAME, task_feedwdg_entry, NULL,
                           TASK_FEEDWDG_STACKSIZE, TASK_FEEDWDG_PRI);
    if (ret != 0) {
        printf("create watchdog task error\r\n");

        ret = rvm_hal_wdt_stop(wdt_dev);
        if (ret != 0) {
            printf("stop watchdog error!\r\n");
        }
        goto failure;
    }

    /* warning: This is just to show the effect of not feeding the dog.
     * When the watchdog feeding task is deleted, the watchdog timeout will
     * trigger the system reboot. */

    aos_msleep(5000);
    printf("delete the watchdog fedding task\r\n");
    task_quit = 1;
    printf("There is no place to feed the watchdog now\r\n");
    ret = rvm_hal_wdt_stop(wdt_dev);
    if (ret != 0) {
        printf("stop watchdog error!\r\n");
        goto failure;
    }
    rvm_hal_wdt_close(wdt_dev);
    printf("watchdog test successfully\r\n");
    return 0;

failure:
    rvm_hal_wdt_close(wdt_dev);
    return -1;
}


#if defined(AOS_COMP_DEVFS) && AOS_COMP_DEVFS
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TASK_DEVFS_FEEDWDG_NAME      "devfsfeedwdg"

static int fd;
volatile static int devfs_wdt_flag;
void devfs_wdt_callback(rvm_dev_t *dev, void *arg)
{
    devfs_wdt_flag = 1;
}

/* task entry */
static void task_devfs_feedwdg_entry(void *arg)
{
    /**
     * The task fed the watchdog every 1000ms. The feeding interval must be less than the
     * watchdog timeout, otherwise it will trigger by mistake.
     */
    while (!task_quit) {
        printf("devfs feed the watchdog!\r\n");

        int ret = ioctl(fd, WDT_IOC_FEED);
        if (ret < 0) {
            printf("WDT_IOC_FEED fail !\n");
            return;
        }

        aos_msleep(1000);
#if !defined(CONFIG_CHIP_D1) && !defined(CONFIG_CHIP_CH2601) && !defined(CVI_SOC_CV181XC)
        uint32_t remaining_time = 0;
        ret = ioctl(fd, WDT_IOC_GET_REMAINING_TIME, &remaining_time);
        if (ret < 0) {
            printf("WDT_IOC_GET_REMAINING_TIME fail !\n");
        }
        printf("WDT_IOC_GET_REMAINING_TIME: %d\n", remaining_time);
#endif
    }
}

int devfs_wdt_demo(void)
{
    printf("devfs_wdt_demo start\n");

    int ret = -1;

    rvm_wdt_drv_register(0);

    char *wdtdev = "/dev/wdt0";

    fd = open(wdtdev, O_RDWR);
    printf("open wdt0 fd:%d\n", fd);
    if (fd < 0) {
        printf("open %s failed. fd:%d\n", wdtdev, fd);
        return -1;
    }

    uint32_t ms = WDT_TIMEOUT;
    ret = ioctl(fd, WDT_IOC_SET_TIMEOUT, &ms);
    if (ret < 0) {
        printf("WDT_IOC_SET_TIMEOUT fail !\n");
        goto failure;
    }

    ret = ioctl(fd, WDT_IOC_START, &ms);
    if (ret < 0) {
        printf("WDT_IOC_START fail !\n");
        goto failure;
    }

    /* Create the task to feed the watchdog */
    task_quit = 0;
    ret = aos_task_new_ext(&task_feedwdg, (const char *)TASK_DEVFS_FEEDWDG_NAME, task_devfs_feedwdg_entry, NULL,
                           TASK_FEEDWDG_STACKSIZE, TASK_FEEDWDG_PRI);
    if (ret != 0) {
        printf("create watchdog task error\r\n");
        ret = ioctl(fd, WDT_IOC_STOP);
        if (ret < 0) {
            printf("WDT_IOC_STOP fail !\n");
            goto failure;
        }
    }

    /* warning: This is just to show the effect of not feeding the dog.
     * When the watchdog feeding task is deleted, the watchdog timeout will
     * trigger the system reboot. */

    aos_msleep(5000);
    printf("delete the watchdog fedding task\r\n");
    task_quit = 1;
#if !defined(CONFIG_CHIP_D1) && !defined(CONFIG_CHIP_CH2601) && !defined(CVI_SOC_CV181XC)
    rvm_wdt_dev_msg_t msg_wdt;
    msg_wdt.callback = devfs_wdt_callback;
    msg_wdt.arg = NULL;
    ret = ioctl(fd, WDT_IOC_ATTACH_CALLBACK, &msg_wdt);
    if (ret < 0) {
        printf("WDT_IOC_ATTACH_CALLBACK fail !\n");
        goto failure;
    }
    devfs_wdt_flag = 0;
    while(!devfs_wdt_flag) {
        ;
    }
    ret = ioctl(fd, WDT_IOC_FEED);
    if (ret < 0) {
        printf("WDT_IOC_FEED fail !\n");
        goto failure;
    }
    if (devfs_wdt_flag == 1) {
        printf("WDT_IOC_ATTACH_CALLBACK test successfully !\n");
    } else {
        printf("WDT_IOC_ATTACH_CALLBACK test fail !\n");
        goto failure;
    }

    ret = ioctl(fd, WDT_IOC_DETACH_CALLBACK);
    if (ret < 0) {
        printf("WDT_IOC_DETACH_CALLBACK fail !\n");
        goto failure;
    }
#endif
    ret = ioctl(fd, WDT_IOC_STOP);
    if (ret < 0) {
        printf("WDT_IOC_STOP fail !\n");
        goto failure;
    }
    close(fd);
    printf("devfs watchdog test successfully\r\n");
    return 0;
failure:
    close(fd);
    return -1;
}

#endif