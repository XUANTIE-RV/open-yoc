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

#include "app_config.h"
#include "app_init.h"
#include "csi_config.h"
#include "hw_config.h"
#include "lvgl.h"
#include "lv_label.h"
#include "oled.h"

#include "board_config.h"
#include "drv/gpio_pin.h"
#include <drv/pin.h>
#include <drv/pwm.h>

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
static void demo_task(void *arg);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

volatile uint32_t g_debug = 0;
volatile uint32_t g_debug_v = 0;

static void demo_task(void *arg);

#include "csi_core.h"
/**
 * main
 */
int main(void)
{
    board_yoc_init();

    printf("===%s, %d\n", __FUNCTION__, __LINE__);
    printf("===%s, %d\n", __FUNCTION__, __LINE__);
    aos_task_new("demo", demo_task, NULL, 10 * 1024);
    return 0;
}

static void lable_test(void)
{
    lv_obj_t *p = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(p, LV_LABEL_LONG_BREAK);
    lv_label_set_align(p, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_pos(p, 0, 4);
    lv_obj_set_size(p, 128, 60);
    lv_label_set_text(p, "MARQUEE\nDEMO");
}

#ifdef CONFIG_GPIO_MODE
static uint32_t g_ctr = 0;
static csi_gpio_pin_t r;
static csi_gpio_pin_t g;
static csi_gpio_pin_t b;
void led_pinmux_init()
{
    csi_pin_set_mux(PA7, PIN_FUNC_GPIO);
    csi_pin_set_mux(PA25, PIN_FUNC_GPIO);
    csi_pin_set_mux(PA4, PIN_FUNC_GPIO);
    csi_gpio_pin_init(&r, PA7);
    csi_gpio_pin_dir(&r, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_init(&g, PA25);
    csi_gpio_pin_dir(&g, GPIO_DIRECTION_OUTPUT);
    csi_gpio_pin_init(&b, PA4);
    csi_gpio_pin_dir(&b, GPIO_DIRECTION_OUTPUT);
    g_ctr = 0;
}

void led_refresh()
{
    g_ctr++;
    if (g_ctr % 3 == 0)
    {

        csi_gpio_pin_write(&r, GPIO_PIN_HIGH);
        csi_gpio_pin_write(&g, GPIO_PIN_LOW);
        csi_gpio_pin_write(&b, GPIO_PIN_LOW);
    }
    else if (g_ctr % 2 == 0)
    {
        csi_gpio_pin_write(&r, GPIO_PIN_LOW);
        csi_gpio_pin_write(&g, GPIO_PIN_HIGH);
        csi_gpio_pin_write(&b, GPIO_PIN_LOW);
    }
    else
    {
        csi_gpio_pin_write(&r, GPIO_PIN_LOW);
        csi_gpio_pin_write(&g, GPIO_PIN_LOW);
        csi_gpio_pin_write(&b, GPIO_PIN_HIGH);
    }
}

void marquee_test(void)
{
    while (1)
    {

        udelay(1000 * 1000);

        led_refresh();
    }
}
#endif

static void demo_task(void *arg)
{
    lv_init();
    oled_init();
    lable_test();
    led_pinmux_init();

    while (1)
    {
        lv_task_handler();
        udelay(1000 * 1000);
        lv_tick_inc(1);
        led_refresh();
    }
}
