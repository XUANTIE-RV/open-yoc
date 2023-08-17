/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kernel.h>
#include <cxvision/cxvision.h>

extern "C" int cx_init(void)
{
    cx::Init();
    return 0;
}
