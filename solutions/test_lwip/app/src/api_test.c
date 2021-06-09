/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include "yunit.h"

extern void test_lwipapi(void);

void yunit_test_api()
{
    yunit_test_init();
    test_lwipapi();
}
