/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
#include <stdlib.h>
#include <stdio.h>

#include <devices/rtl8723ds.h>

void board_wifi_init(void)
{
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };
    wifi_rtl8723ds_register(&pin);
}
#endif
