/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"

#define TAG "app"

static void cli_reg_cmd_app(void)
{
    /* aos api test */
    extern void test_event_cmd(void);
    test_event_cmd();

    extern void test_mm_cmd(void);
    test_mm_cmd();

    extern void test_mutex_cmd(void);
    test_mutex_cmd();

    extern void test_queue_cmd(void);
    test_queue_cmd();

    extern void test_sem_cmd(void);
    test_sem_cmd();

    extern void test_aos_timer_cmd(void);
    test_aos_timer_cmd();

    extern void test_task_cmd(void);
    test_task_cmd();

    /* drv test */
    extern void test_gpio_cmd(void);
    test_gpio_cmd();

    extern void test_timer_cmd(void);
    test_timer_cmd();

    extern void test_usart_cmd(void);
    test_usart_cmd();

    extern void test_iic_cmd(void);
    test_iic_cmd();
}



/*************************************************
 * YoC入口函数
 *************************************************/
void main()
{
extern int posix_init(void);
    posix_init();

    board_base_init();
    yoc_base_init();

    LOGD(TAG, "enter app");
    cli_reg_cmd_app();
}
