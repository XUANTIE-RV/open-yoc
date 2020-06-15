/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "app_main.h"

#define TAG "sd_detect"


/* 如果系统有sd卡检测pin口，检测sd卡是否存在。
 * 检测到sd存在，自动注册fatfs，否则不注册fatfs；
 * 如果系统没有sd卡检测机制，则自动注册fatfs，存在
 * sd卡则注册成功，否则注册失败 */
int8_t app_sd_detect_check(void)
{
#if defined(CONFIG_SD_DETECT_EN) && CONFIG_SD_DETECT_EN
    int sd_detect = 0;

    if (SD_DETECT_PIN > 0) {
        if (app_gpio_read(SD_DETECT_PIN, &sd_detect) != 0) {
            LOGI(TAG, "sd detect pin config error");
            return -1;
        }
        drv_pinmux_config(SD_DETECT_PIN, SD_DETECT_FUNC);
    }
    return (sd_detect == 1) ? 0 : 1;
#else
    return 1;
#endif
}
