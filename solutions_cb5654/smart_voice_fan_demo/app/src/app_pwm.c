/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <drv/pwm.h>
#include <soc.h>
#include <pin_name.h>
#include "board_config.h"
#include <pinmux.h>

#include "app_main.h"
#include "shell.h"
#include "app_printf.h"


static void pwm_pin_init(void)
{
    drv_pinmux_config(TEST_PWM_CH1, TEST_PWM_CH1_FUNC);
}

void pwm_test_func(void)
{
    pwm_handle_t pwm_handle;

    LOGD("pwm_test_func enter\n");

    pwm_pin_init();
    pwm_handle = csi_pwm_initialize(TEST_PWM_IDX);

    csi_pwm_config(pwm_handle, TEST_PWM_CH, 3000, 1500);
    mdelay(20);
    csi_pwm_start(pwm_handle, TEST_PWM_CH);

    csi_pwm_config(pwm_handle, TEST_PWM_CH, 200, 150);
    mdelay(20);
    csi_pwm_stop(pwm_handle, TEST_PWM_CH);

    csi_pwm_uninitialize(pwm_handle);
    LOGD("pwm_test_func end\n");
}


static void cmd_app_func(int argc, char *argv[])
{
    if (argc > 1) {
        return;
    }
    pwm_test_func();
}

void shell_reg_cmd_pwm(void)
{
    shell_cmd_t cmd_info = {"pwm", "pwm test", cmd_app_func};

    shell_register_command(&cmd_info);
}
