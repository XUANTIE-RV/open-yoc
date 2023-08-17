/*
 * Copyright (C) 2018-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "ulog/ulog.h"
#include <fcntl.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#define TAG "pf_test"

void do_pf_test() __attribute__((noinline));
void do_pf_test()
{
    LOGI(TAG, "cause bengine dload when calling this function!!!");
}


