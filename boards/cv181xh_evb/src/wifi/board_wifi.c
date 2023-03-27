/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
#include <stdlib.h>
#include <stdio.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include <drv/gpio_pin.h>
#include "pinctrl-mars.h"

#if defined(CONFIG_BOARD_WIFI_RTL8723DS) && (CONFIG_BOARD_WIFI_RTL8723DS > 0)
#include <devices/rtl8723ds.h>
#elif defined(CONFIG_BOARD_WIFI_HI3861) && (CONFIG_BOARD_WIFI_HI3861 > 0)
#include <hi3861l_devops.h>
#endif

/* Caution: Disable the pinmux for specific evb, all related io have been pulled up directly.
   when we use kongke board, we need change the io configuration. */
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
#ifndef RTL8723DS_DEV_WIFI_EN_CHN
#define RTL8723DS_DEV_WIFI_EN_CHN 30
#endif
#ifndef RTL8723DS_DEV_WIFI_POWER_CHN
#define RTL8723DS_DEV_WIFI_POWER_CHN 30
#endif
#endif

#ifndef RTL8723DS_DEV_WIFI_EN_GRP
#define RTL8723DS_DEV_WIFI_EN_GRP 0
#endif

#ifndef RTL8723DS_DEV_WIFI_POWER_GRP
#define RTL8723DS_DEV_WIFI_POWER_GRP 0
#endif

#ifdef CONFIG_BOARD_RUIXIANG_BOARD
static void wifi_pin_init(void)
{
    csi_gpio_t wifi_en_gpio = {0};
    csi_gpio_t wifi_power_gpio = {0};
    if (csi_gpio_init(&wifi_en_gpio, RTL8723DS_DEV_WIFI_EN_GRP) != CSI_OK) {
            printf("%s gpio init err \n",__func__);
            return ;
        }
    if (csi_gpio_init(&wifi_power_gpio, RTL8723DS_DEV_WIFI_POWER_GRP) != CSI_OK) {
        csi_gpio_uninit(&wifi_en_gpio);
        printf("%s gpio init err \n",__func__);
        return ;
    }
    csi_gpio_dir(&wifi_en_gpio , (1 << RTL8723DS_DEV_WIFI_EN_CHN) , GPIO_DIRECTION_OUTPUT);
    csi_gpio_dir(&wifi_power_gpio , (1 << RTL8723DS_DEV_WIFI_POWER_CHN) , GPIO_DIRECTION_OUTPUT);

    csi_gpio_write(&wifi_power_gpio , (1 << RTL8723DS_DEV_WIFI_POWER_CHN), 1);
    aos_msleep(200);
    csi_gpio_write(&wifi_en_gpio , (1 << RTL8723DS_DEV_WIFI_EN_CHN), 0);
    aos_msleep(50);
    csi_gpio_write(&wifi_en_gpio , (1 << RTL8723DS_DEV_WIFI_EN_CHN), 1);
    aos_msleep(50);
}
#endif

void board_wifi_init(void)
{
#if defined(CONFIG_BOARD_WIFI_RTL8723DS) && (CONFIG_BOARD_WIFI_RTL8723DS > 0)
    rtl8723ds_gpio_pin pin = {
        .wl_en = -1, // Change if on kongke board
        .power = -1, // Change if on kongke board
#ifdef CONFIG_BOARD_RUIXIANG_BOARD
        .sdio_idx = 1,
#else
        .sdio_idx = 0,
#endif
    };

#ifdef CONFIG_BOARD_RUIXIANG_BOARD
    wifi_pin_init();
#endif

    wifi_rtl8723ds_register(&pin);
#elif defined(CONFIG_BOARD_WIFI_HI3861) && (CONFIG_BOARD_WIFI_HI3861 > 0)
    wifi_hi3861l_register(NULL);
#else
    printf("WARNING: NOT SUPPORT WIFI\n");
#endif
}
#endif
