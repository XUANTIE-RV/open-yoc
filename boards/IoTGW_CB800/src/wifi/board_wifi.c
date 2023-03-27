/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>

#include <board.h>
#include <w800_devops.h>

void board_wifi_init(void)
{
    wifi_w800_register(NULL);
}
