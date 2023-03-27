/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_BT_SUPPORT) && BOARD_BT_SUPPORT > 0
#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <drv/pin.h>
#include <drv/gpio.h>
#include "pinctrl-mars.h"

#if defined(CONFIG_BOARD_BT_RTL8723DS) && (CONFIG_BOARD_BT_RTL8723DS > 0)
#include <devices/rtl8723ds_bt.h>
#ifndef CVI_BT_UART_ID
#define CVI_BT_UART_ID 3
#endif

#ifdef CONFIG_BOARD_RUIXIANG_BOARD
#ifndef CVI_BT_DIS_PIN_ID
#define CVI_BT_DIS_PIN_ID 21
#endif

#ifndef BT_DIS_PIN_GRP
#define BT_DIS_PIN_GRP 1
#endif

#else
//Change if on kongke board
#ifndef CVI_BT_DIS_PIN_ID
#define CVI_BT_DIS_PIN_ID -1
#endif

#ifndef BT_DIS_PIN_GRP
#define BT_DIS_PIN_GRP 0
#endif
#endif
#endif

#ifdef CONFIG_BOARD_RUIXIANG_BOARD
#include <drv/pin.h>
#include <pinctrl-mars.h>
#include "cvi_type.h"
static void bt_pin_init(void)
{
    csi_gpio_t bt_dsi_gpio = {0};

    if (csi_gpio_init(&bt_dsi_gpio, BT_DIS_PIN_GRP) != CSI_OK) {
        printf("%s gpio init err \n",__func__);
        return;
    }
    csi_gpio_mode(&bt_dsi_gpio , (1 << CVI_BT_DIS_PIN_ID) , GPIO_MODE_PULLUP);
    csi_gpio_dir(&bt_dsi_gpio , (1 << CVI_BT_DIS_PIN_ID) , GPIO_DIRECTION_OUTPUT);
    csi_gpio_write(&bt_dsi_gpio , (1 << CVI_BT_DIS_PIN_ID), 0);
    aos_msleep(200);
    csi_gpio_write(&bt_dsi_gpio , (1 << CVI_BT_DIS_PIN_ID), 1);
    aos_msleep(500);
}
#endif

void board_bt_init(void)
{
#if defined(CONFIG_BOARD_BT_RTL8723DS) && (CONFIG_BOARD_BT_RTL8723DS > 0)
    rtl8723ds_bt_config config = {
        .uart_id    = CVI_BT_UART_ID,
        .bt_dis_pin = -1,//CVI_BT_DIS_PIN_ID,
    };

#ifdef CONFIG_BOARD_RUIXIANG_BOARD
    bt_pin_init();
#endif

    bt_rtl8723ds_register(&config);

    extern int hci_h5_driver_init();
    hci_h5_driver_init();
#else
    printf("WARNING: NOT SUPPORT BT\n");
#endif
}
#endif
