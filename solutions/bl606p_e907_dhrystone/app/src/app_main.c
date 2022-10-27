/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "app_config.h"
#include "app_main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <soc.h>
#include <drv/tick.h>

void pre_main(int argc, char *argv[0])
{
    board_yoc_init();
    printf("start dhrystone\r\n");
    benchmark_dhry_main();
}
