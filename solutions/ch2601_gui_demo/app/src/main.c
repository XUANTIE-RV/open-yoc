/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <aos/aos.h>
#include "aos/cli.h"

#include "app_init.h"
#include "lvgl.h"
#include "lv_label.h"
#include "oled.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "app"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
// GUI
static void gui_lvgl_task(void *arg);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

volatile uint32_t g_debug   = 0;
volatile uint32_t g_debug_v = 0;

static void gui_lvgl_task(void *arg);

#include "csi_core.h"
/**
 * main
 */
int main(void)
{
    board_yoc_init();

    aos_task_new("gui", gui_lvgl_task, NULL, 10 * 1024);
    return 0;
}

static void gui_label_create(void)
{
    lv_obj_t *p = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(p, LV_LABEL_LONG_BREAK);
    lv_label_set_align(p, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_pos(p, 0, 4);
    lv_obj_set_size(p, 128, 60);
    lv_label_set_text(p, "THEAD RISC-V\nGUI\nDEMO");
}

static void gui_lvgl_task(void *arg)
{
    lv_init();
    /*Initialize for LittlevGL*/
    oled_init();

    /*Select display 1*/
    // demo_create();
    gui_label_create();

    while (1) {
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_task_handler();

        aos_msleep(5);
        lv_tick_inc(1);
    }
}
