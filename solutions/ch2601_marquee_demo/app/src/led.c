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
#include "app_config.h"
#include "app_init.h"
#include "csi_config.h"
#include "hw_config.h"

#include "board_config.h"
#include <drv/pin.h>
#include <drv/pwm.h>

#ifdef CONFIG_PWM_MODE

static uint32_t g_ctr = 0;
static csi_pwm_t  r;
void led_pinmux_init()
{
        //7
    csi_error_t ret;
    csi_pin_set_mux(PA7, PA7_PWM_CH7);
    csi_pin_set_mux(PA25, PA25_PWM_CH2);
    csi_pin_set_mux(PA4, PA4_PWM_CH4);
    ret = csi_pwm_init(&r, 0);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
    ret = csi_pwm_out_config(&r, 7 / 2, 300, 100, PWM_POLARITY_HIGH);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
    ret = csi_pwm_out_start(&r, 7 / 2);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
        //25
    ret = csi_pwm_out_config(&r, 2 / 2, 300, 100, PWM_POLARITY_HIGH);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
    ret = csi_pwm_out_start(&r, 2 / 2);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
        //4
    ret = csi_pwm_out_config(&r, 4 / 2, 300, 100, PWM_POLARITY_HIGH);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
    ret = csi_pwm_out_start(&r, 4 / 2);
    if (ret != CSI_OK) {
            printf("===%s, %d\n", __FUNCTION__, __LINE__);
            return ;
    }
}

void led_refresh()
{
    g_ctr++;
    if (g_ctr == 3) {
            g_ctr = 0;
    }
    if (g_ctr == 0)
    {
        csi_pwm_out_start(&r, 7 / 2);
        csi_pwm_out_stop(&r, 2 / 2);
        csi_pwm_out_stop(&r, 4 / 2);

    }
    else if (g_ctr == 1)
    {        
        csi_pwm_out_start(&r, 2 / 2);
        csi_pwm_out_stop(&r, 7 / 2);
        csi_pwm_out_stop(&r, 4 / 2);

    }
    else //2
    {
        csi_pwm_out_start(&r, 4 / 2);
        csi_pwm_out_stop(&r, 7 / 2);
        csi_pwm_out_stop(&r, 2 / 2);

    }
}
void marquee_test(void)
{
    while (1);
}
#endif 
