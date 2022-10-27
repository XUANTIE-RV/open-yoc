/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include "aos/cli.h"
#include "main.h"
#include "app_init.h"
#include "lvgl.h"
#include "lv_label.h"
#include "oled.h"

#define TAG "app"

extern uint32_t g_lcd;
extern uint32_t g_led;
extern void ft_lcd(void);
extern void led_refresh(void);

static void gui_lvgl_task(void *arg)
{
    lv_init();
    oled_init();

    while (1)
    {
        if (g_lcd == 1)
        {
            lv_task_handler();
        }

        aos_msleep(500);
        if (g_lcd == 1)
        {
            lv_tick_inc(1);
        }
        ft_led_refresh();
    }
}

int main(void)
{
    board_yoc_init();
    LOGD(TAG, "%s\n", aos_get_app_version());

    led_pinmux_init();
    ft_btn_init();
    lv_init();
    oled_init();
    cli_reg_cmd_ft();

    aos_task_new("gui", gui_lvgl_task, NULL, 10 * 1024);

    return 0;
}
