/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "drv/timer.h"
#include "string.h"
#include "soc.h"
#include "app_main.h"
#include "shell.h"
#include "app_printf.h"

#define TEST_MODE_FREE_RUNING   0
#define TEST_MODE_USER_DEFINED  1
static volatile uint8_t time_free_runing_flag = 0;
static volatile uint8_t time_user_defined_flag = 0;
static volatile uint8_t test_mode;

static void timer_event_cb_fun(int32_t idx, timer_event_e event)
{
    switch (test_mode) {
        case TEST_MODE_FREE_RUNING:
            time_free_runing_flag = 1;
            LOGD("enter cb for mode free_runing \n");
            break;

        case TEST_MODE_USER_DEFINED:
            time_user_defined_flag = 1;
            LOGD("enter cb for mode user_defined\n");
            break;
    }

}

static int32_t test_free_running_fun(timer_handle_t timer_handle)
{
    uint32_t timeout = 0x3ffffff;
    uint32_t value;
    time_free_runing_flag = 0;

    test_mode = TEST_MODE_FREE_RUNING;
    if (csi_timer_config(timer_handle, TIMER_MODE_FREE_RUNNING) < 0) {
        return -1;
    }

    if (csi_timer_start(timer_handle) < 0) {
        LOGE("csi_timer_start error\n");
        return -1;
    }

    while (timeout) {
        timeout--;
        csi_timer_get_current_value(timer_handle, &value);
        if (value < 0xffffff00) {
            break;
        }
    }

    if (timeout == 0) {
        LOGD("time out \n");
        return -1;
    }

    if (csi_timer_stop(timer_handle) < 0) {
        return -1;
    }

    return 0;
}
static int32_t test_user_defined_fun(timer_handle_t timer_handle)
{
    uint32_t timeout = 0x3fffffff;
    time_user_defined_flag = 0;

    test_mode = TEST_MODE_USER_DEFINED;

    if (csi_timer_config(timer_handle, TIMER_MODE_RELOAD) < 0) {
        return -1;
    }

    if (csi_timer_set_timeout(timer_handle, 10000000) < 0) {
        return -1;
    }

    if (csi_timer_start(timer_handle) < 0) {
        return -1;
    }

    while (timeout) {
        timeout--;
        if (time_user_defined_flag == 1) {
            break;
        }
    }

    if (time_user_defined_flag == 0) {
        LOGD("time out \n");
        return -1;
    }

    if (csi_timer_stop(timer_handle) < 0) {
        return -1;
    }

    return 0;
}

static void cmd_app_func(int argc, char *argv[])
{
    timer_handle_t timer_handle;
    if (argc < 2) {
        return;
    }
    timer_handle = csi_timer_initialize(0, timer_event_cb_fun);

    if (strcmp(argv[1], "free") == 0) {
        if (test_free_running_fun(timer_handle) < 0) {
            LOGI("test_free_running_fun error\n");
        } else {
            LOGI("timer free_running test passed\n");
        }
    } else if (strcmp(argv[1], "defined") == 0) {
        if (test_user_defined_fun(timer_handle) < 0) {
            LOGI("test_reload_fun error\n");
        } else {
            LOGI("time out after 10s passed\n");
        }
    } else {
        LOGE("timer cmd error\n");
    }

    if (csi_timer_uninitialize(timer_handle) < 0) {
        LOGE("csi_timer_uninitialize error\n");
    }
}

void shell_reg_cmd_timer(void)
{
    shell_cmd_t cmd_info = {"timer", "timer free/defined", cmd_app_func};

    shell_register_command(&cmd_info);
}
