/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "app_init.h"
#include <aos/yloop.h>
#define TAG "app"

extern void board_yoc_init(void);

void main()
{
    board_yoc_init();
    event_service_init(NULL);
    aos_loop_init();
}

