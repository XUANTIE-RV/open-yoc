/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <pwm_test.h>


test_func_map_t pwm_test_funcs_map[] = {
    {"PWM_INTERFACE", test_pwm_interface},
    {"PWM_OUTPUT", test_pwm_output},
    {"PWM_OUTPUT_CHECK", test_pwm_outputCheck},
    {"PWM_CAPTURE", test_pwm_capture},
    {"PWM_CAPTURE_INPUT", test_pwm_captureInput},
};


int test_pwm_main(char *args)
{
    uint8_t i;

    for (i = 0; i < sizeof(pwm_test_funcs_map) / sizeof(test_func_map_t); i++) {
        if (!strcmp((void *)_mc_name, pwm_test_funcs_map[i].test_func_name)) {
            (*(pwm_test_funcs_map[i].test_func_p))(args);
            return 0;
        }
    }

    TEST_CASE_TIPS("PWM module don't support this command.");
    return -1;
}


