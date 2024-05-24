/*
 * Copyright (C) 2018-2024 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static int g_tcm_cnt = 10;
static char *g_tcm_str = "execute in tcm region";

void tcm_test()
{
    while (g_tcm_cnt--) {
        printf("[%d]: %s\r\n", g_tcm_cnt, g_tcm_str);
    }
}
