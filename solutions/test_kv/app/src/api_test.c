/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include "yunit.h"
extern void api_kv_test(void);

void yunit_test_api()
{
    yunit_test_init();
    api_kv_test();
}
