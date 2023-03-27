/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include <ulog/ulog.h>

#include "app_main.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demos.h"

#define TAG "main"

#ifndef CONFIG_UI_TASK_STACK_SIZE
#define CONFIG_UI_TASK_STACK_SIZE (65536)
#endif

#ifndef CONFIG_LVGL_IDLE_TIME
#define CONFIG_LVGL_IDLE_TIME  20
#endif

static aos_mutex_t lvgl_mutex;

/* LVGL UI Demo */
static void lvgl_ui_task(void *arg)
{
    while (1) {
        aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
        lv_task_handler();
        aos_mutex_unlock(&lvgl_mutex);
        aos_msleep(CONFIG_LVGL_IDLE_TIME);
    }
}

static void lvgl_timer_task(void *args)
{
    while (1) {
        aos_mutex_lock(&lvgl_mutex, AOS_WAIT_FOREVER);
        lv_timer_handler();
        aos_mutex_unlock(&lvgl_mutex);
        aos_msleep(CONFIG_LVGL_IDLE_TIME);
    }
}

void lvgl_ui_task_run(void)
{
    aos_task_t lvgl_demo_task;
    aos_task_t timer_task;

    aos_mutex_new(&lvgl_mutex);

    LOGD(TAG, "lvgl demo build time: %s, %s\r\n", __DATE__, __TIME__);

    /* Init lvgl*/
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    /* Demos */
    // lv_demo_stress();
    // lv_demo_widgets();
    // lv_demo_benchmark();
    lv_demo_music();

    aos_task_new_ext(&lvgl_demo_task, "lvgl-ui-task", lvgl_ui_task, NULL, CONFIG_UI_TASK_STACK_SIZE,
                     AOS_DEFAULT_APP_PRI);

    aos_task_new_ext(&timer_task, "lvgl-timer-task", lvgl_timer_task, NULL, CONFIG_UI_TASK_STACK_SIZE,
                     AOS_DEFAULT_APP_PRI - 1);
}

int main(int argc, char *argv[])
{
    board_yoc_init();

    lvgl_ui_task_run();
}
