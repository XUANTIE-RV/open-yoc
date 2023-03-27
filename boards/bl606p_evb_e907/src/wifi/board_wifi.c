/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <board.h>

#if defined(BOARD_WIFI_SUPPORT) && BOARD_WIFI_SUPPORT > 0
#include <stdlib.h>
#include <stdio.h>

#include <devices/wifi.h>
#include <devices/bl606p_wifi.h>
extern int wifi_mgmr_cli_init(void);

void board_wifi_init(void)
{
    wifi_bl606p_register(NULL);
    wifi_mgmr_cli_init();
}
#endif
