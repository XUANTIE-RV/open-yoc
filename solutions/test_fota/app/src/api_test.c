/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include "yunit.h"
#include <unistd.h>

extern void test_fotaapi(void);

void yunit_test_api()
{
    test_fotaapi();
}
