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

extern void api_kv_test_entry(yunit_test_suite_t *suite);

void api_kv_test(void)
{
	yunit_test_suite_t *suite;
	suite = yunit_add_test_suite("kvapi", init, cleanup, setup, teardown);
	api_kv_test_entry(suite);
}

