/*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
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

#include "pwm_test.h"



int test_pwm_interface(char *args)
{


    //initialize
    pwm_handle_t pwm = csi_pwm_initialize(100);
    TEST_CASE_ASSERT(pwm == NULL,"pwm != NULL act_val=%d",pwm);

    pwm = csi_pwm_initialize(0);
    TEST_CASE_ASSERT(pwm != NULL,"pwm == NULL act_val=%d",pwm);

    int32_t ret;
    //uninitialize
    csi_pwm_uninitialize(NULL);

    ret = csi_pwm_power_control(pwm, 4);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_PWM_BASE | DRV_ERROR_UNSUPPORTED),"ret != (CSI_DRV_ERRNO_PWM_BASE | DRV_ERROR_UNSUPPORTED) act_val=%d",ret);

    //config
    ret = csi_pwm_config(NULL, 0, 300000, 100000);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_PWM_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_PWM_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    ret = csi_pwm_config(pwm, 0, 300000, 300001);
    TEST_CASE_ASSERT(ret == (CSI_DRV_ERRNO_PWM_BASE | DRV_ERROR_PARAMETER),"ret != (CSI_DRV_ERRNO_PWM_BASE | DRV_ERROR_PARAMETER) act_val=%d",ret);

    csi_pwm_uninitialize(pwm);
    return 0;
}