/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/kernel.h>
#include <aos/aos.h>


#include <aos/hal/gpio.h>
#include <aos/hal/pwm.h>
#include <drv/pwm.h>
#include <ulog/ulog.h>
#include <yoc/atserver.h>
#include <soc.h>

#define TAG "TEST_HAL_PWM"

#define PWM_PORT_PIN_NAME               PA29
#define PWM_PORT_PIN_FUNC               2
#define PWM_PORT_PIN_CH                 3

#define AT_BACK_READY()                       atserver_send("\r\nREADY\r\n")

/* define dev */
pwm_dev_t pwm1;

static void aos_hal_pwm(float duty_cycle,uint32_t freq)
{
    int32_t ret = -1;
    int count = 0;

    /* pwm port set */
    pwm1.port = PWM_PORT_PIN_CH;
    /* pwm attr config */
    pwm1.config.duty_cycle = duty_cycle; 
    pwm1.config.freq       = freq;

    /* init pwm1 with the given settings */
    ret = hal_pwm_init(&pwm1);
    if (ret != 0){
        printf("pwm init error !\n");
        AT_BACK_ERR();
        return;
    }

    while (count < 5000) {
        ret = hal_pwm_start(&pwm1);
        if (ret != 0){
            printf("pwm output error !\n");
            AT_BACK_ERR();
            return;
        }   
        LOGI(TAG, "pwm1 start output!\n");
        // aos_msleep(1000);
        count++;
    };

    ret = hal_pwm_stop(&pwm1);
    if (ret != 0){
        printf("pwm stop error !\n");
        AT_BACK_ERR();
        return;
    }
    LOGI(TAG, "pwm1 stop output!\n");

    ret = hal_pwm_finalize(&pwm1);
    if (ret != 0){
        printf("pwm finalize error !\n");
        AT_BACK_ERR();
        return;
    }

    AT_BACK_OK();
}

static void aos_hal_pwm_para_chg(float duty_cycle,uint32_t freq)
{
    int32_t ret = -1;
    pwm_config_t pwm_cfg1;
    int count = 0;

    /* pwm port set */
    pwm1.port = PWM_PORT_PIN_CH;
    /* pwm attr config */
    pwm1.config.duty_cycle = 0.5f; 
    pwm1.config.freq       = 1000;

    ret = hal_pwm_init(&pwm1);
    if (ret != 0){
        printf("pwm init error !\n");
        AT_BACK_ERR();
        return;
    }

    ret = hal_pwm_start(&pwm1);
    if (ret != 0){
        printf("pwm output error !\n");
        AT_BACK_ERR();
        return;
    }  

    while (count < 20) {
        LOGI(TAG, "pwm1 is output now!\n");
        if (count == 5) {
            memset(&pwm_cfg1, 0, sizeof(pwm_config_t));
            pwm_cfg1.duty_cycle      = duty_cycle;
            pwm_cfg1.freq            = freq;

            ret = hal_pwm_para_chg(&pwm1, pwm_cfg1);
            if (ret != 0){
                printf("pwm para_chg error !\n");
                AT_BACK_ERR();
                return;
            }
        }
        count++;
        if (count == 20) {
            hal_pwm_stop(&pwm1);
            hal_pwm_finalize(&pwm1);
        }
    };

    AT_BACK_OK();
}

static void aos_hal_pwm_error(){
    int32_t ret1 = -1;
    int32_t ret2 = -1;
    int32_t ret3 = -1;
    int32_t ret4 = -1;

    ret1 = hal_pwm_init(NULL);

    ret2 = hal_pwm_start(NULL);

    ret3 = hal_pwm_stop(NULL);
    
    ret4 = hal_pwm_finalize(NULL);

    if (ret1 != 0 && ret2 != 0 && ret3 != 0 && ret4 != 0) {
            AT_BACK_OK();
    }
    else{
            AT_BACK_ERR();
    }
}

void test_hal_pwm(char *cmd, int type, char *data)
{
    csi_pin_set_mux(PWM_PORT_PIN_NAME, PWM_PORT_PIN_FUNC);

    if (strcmp((const char *)data, "'0.5_5'\0") == 0) {
        AT_BACK_READY();
        aos_hal_pwm(0.5f,10);
    }else if (strcmp((const char *)data, "'0_2000'\0") == 0) {
        aos_hal_pwm(0.0f,2000);
    }else if (strcmp((const char *)data, "'0.9_500'\0") == 0) {
        aos_hal_pwm(0.9f,500);
    }else if (strcmp((const char *)data, "'para_chg_250'\0") == 0) {
        aos_hal_pwm_para_chg(0.5f,250);
    }else if (strcmp((const char *)data, "'para_chg_50000'\0") == 0) {
        aos_hal_pwm_para_chg(0.5f,50000);
    }else if (strcmp((const char *)data, "'para_chg_1000'\0") == 0) {
        aos_hal_pwm_para_chg(0.3f,1000);
    }else if (strcmp((const char *)data, "'error'\0") == 0) {
        aos_hal_pwm_error();
    }
}
