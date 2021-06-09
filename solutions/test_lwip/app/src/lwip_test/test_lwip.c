/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include "yunit.h"

static int init(void)
{
    return 0;
}
static int cleanup(void)
{
    return 0;
}
static void setup(void)
{
}
static void teardown(void)
{
}

extern void lwipapi_test_entry(yunit_test_suite_t *suite);

void test_lwipapi(void){
    yunit_test_suite_t *suite;
    suite = yunit_add_test_suite("lwipapi", init, cleanup, setup, teardown);
    lwipapi_test_entry(suite);
}
