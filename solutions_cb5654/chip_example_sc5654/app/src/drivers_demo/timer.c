/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <app_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drv/timer.h>
#include <aos/cli.h>

#define TEST_MODE_FREE_RUNING   0
#define TEST_MODE_USER_DEFINED  1
static volatile uint8_t time_free_runing_flag = 0;
static volatile uint8_t time_user_defined_flag = 0;
static volatile uint8_t test_mode;

static void timer_event_cb_fun(int32_t idx, timer_event_e event) {
    switch (test_mode) {
        case TEST_MODE_FREE_RUNING:
            time_free_runing_flag = 1;
            printf("enter cb for mode free_runing \n");
            break;

        case TEST_MODE_USER_DEFINED:
            time_user_defined_flag = 1;
            printf("enter cb for mode user_defined\n");
            break;
    }

}

static int32_t test_free_running_fun(uint8_t timer_num) {
    uint32_t timeout = 0x3ffffff;
    uint32_t value;
	timer_handle_t timer_handle;
    time_free_runing_flag = 0;

    if ((timer_handle = csi_timer_initialize(timer_num, timer_event_cb_fun)) == NULL) {
        printf("csi_timer_initialize error\n");
        return -1;
    }

    test_mode = TEST_MODE_FREE_RUNING;
    if (csi_timer_config(timer_handle, TIMER_MODE_FREE_RUNNING) < 0) {
        return -1;
    }

    if (csi_timer_start(timer_handle) < 0) {
        printf("csi_timer_start error\n");
        return -1;
    }

    while (timeout) {
        timeout--;

        csi_timer_get_current_value(timer_handle, &value);
        printf("test_free_running_fun (timeout: 0x%x): value = 0x%x.\n", timeout, value);

        if (value < 0xffffff00) {
            break;
        }
    }

    if (timeout == 0) {
        printf("time out \n");
        return -1;
    }

    if (csi_timer_stop(timer_handle) < 0) {
        return -1;
    }

    if (csi_timer_uninitialize(timer_handle) < 0) {
        printf("csi_timer_uninitialize error\n");
        return -1;
    }
    return 0;
}
static int32_t test_user_defined_fun(uint8_t timer_num)
{
    uint32_t timeout = 0x3fffffff;
	timer_handle_t timer_handle;
    time_user_defined_flag = 0;

    if ((timer_handle = csi_timer_initialize(timer_num, timer_event_cb_fun)) == NULL) {
        printf("csi_timer_initialize error\n");
        return -1;
    }

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
        printf("time out \n");
        return -1;
    }

    if(csi_timer_stop(timer_handle) < 0) {
        return -1;
    }

    if (csi_timer_uninitialize(timer_handle) < 0) {
        printf("csi_timer_uninitialize error\n");
        return -1;
    }

    return 0;
}

static void cmd_timer_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if (strcmp(argv[1], "running") == 0) {
		test_free_running_fun(atoi(argv[2]));
    } else if (strcmp(argv[1], "reload") == 0) {
		test_user_defined_fun(atoi(argv[2]));
    } else {
        printf("cmd_timer_func error\n");
    }
}

void test_timer_cmd(void)
{
    static const struct cli_command cmd_info = {"drv_timer", "drv_timer command", cmd_timer_func};

    aos_cli_register_command(&cmd_info);
}
