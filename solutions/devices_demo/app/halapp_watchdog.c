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
#define WDT_TIMEOUT            15000

/* task handle */
aos_task_t task_feedwdg;

/* wdt handle */
rvm_dev_t *wdt_dev = NULL;

/* task entry */
static void task_feedwdg_entry(void *arg)
{
    /**
     * The task fed the watchdog every 1000ms. The feeding interval must be less than the
     * watchdog timeout, otherwise it will trigger by mistake.
     */
    while (1) {
        printf("feed the watchdog!\r\n");

        rvm_hal_wdt_feed(wdt_dev);

        aos_msleep(1000);
    }
}

int hal_watchdog_demo(void)
{
    int ret;
    char *filename = "wdt0";

    rvm_wdt_drv_register(0);

    wdt_dev = rvm_hal_wdt_open(filename);
    if (wdt_dev == NULL) {
        return -1;
    }

    ret = rvm_hal_wdt_set_timeout(wdt_dev, WDT_TIMEOUT);
    if (ret != 0) {
        printf("rvm_hal_wdt_set_timeout error!\r\n");
    }

    ret = rvm_hal_wdt_start(wdt_dev);
    if (ret != 0) {
        printf("rvm_hal_wdt_start error!\r\n");
    }

    /* Create the task to feed the watchdog */
    ret = aos_task_new_ext(&task_feedwdg, (const char *)TASK_FEEDWDG_NAME, task_feedwdg_entry, NULL,
                           TASK_FEEDWDG_STACKSIZE, TASK_FEEDWDG_PRI);
    if (ret != 0) {
        printf("create watchdog task error\r\n");

        ret = rvm_hal_wdt_stop(wdt_dev);
        if (ret != 0) {
            printf("delete watchdog error!\r\n");
        }

        return -1;
    }

    /* warning: This is just to show the effect of not feeding the dog.
     * When the watchdog feeding task is deleted, the watchdog timeout will
     * trigger the system reboot. */

    aos_msleep(10000);
    printf("delete the watchdog fedding task\r\n");
    aos_task_delete(&task_feedwdg);
    printf("There is no place to feed the watchdog now\r\n");

    return 0;
}
