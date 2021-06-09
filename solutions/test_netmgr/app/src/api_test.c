/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include "yunit.h"
extern void api_netmgr_test(void);

/**
* api 测试总入口
*/
void yunit_test_api()
{
    yunit_test_init();
    api_netmgr_test();
}
