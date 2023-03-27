/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <aos/kernel.h>
#include <drv/porting.h>
#include <devices/input.h>
#include <devices/display.h>

#include "ui_port.h"
#include "aiot_jsapi.h"

#ifndef CONFIG_UI_TASK_STACK_SIZE
#define CONFIG_UI_TASK_STACK_SIZE 95536
#endif

#define TAG "HAASUI"

static void ui_task(void *arg)
{
    extern int falcon_entry(int argc, char *argv[]);
    LOGD(TAG, "haasui build time: %s, %s\r\n", __DATE__, __TIME__);

    /* 注册JSAPI */
    register_hello_jsapi();

    LOGD(TAG, "haasui entry here!\r\n");
    falcon_entry(0, NULL);
}

/** haasui entry task */
void ui_task_run(void)
{
    aos_task_t task;
    aos_task_new_ext(&task, "ui-task", ui_task, NULL, CONFIG_UI_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI - 5);
}