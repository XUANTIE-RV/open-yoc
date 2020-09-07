/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <app_config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <aos/aos.h>
#include "aos/cli.h"

#include "app_init.h"
#include "app_main.h"

#define TAG "app"

/*************************************************
 * APP的各种测试命令
 *************************************************/
static void cli_reg_cmd_app(void)
{
    // extern void cli_reg_cmd_aui(void);
    // cli_reg_cmd_aui();

    extern void cli_reg_cmd_apps(void);
    cli_reg_cmd_apps();

    //extern void test_gpio_register_cmd(void);
    //test_gpio_register_cmd();

    // extern void cli_yv_test_init(void);
    // cli_yv_test_init();

    //extern void test_register_system_cmd(void);
    //test_register_system_cmd();

    // extern void test_iperf_register_cmd(void);
    // test_iperf_register_cmd();
}

void main()
{
extern int posix_init(void);
extern void cxx_system_init(void);

    // must call this func when use c++
    posix_init();
    // must call this func when use c++
    cxx_system_init();
    
    board_yoc_init();

    /* 命令行测试命令 */
    cli_reg_cmd_app();
}

