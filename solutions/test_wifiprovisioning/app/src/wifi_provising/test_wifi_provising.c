/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "../yunit.h"

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

extern void api_wifi_provising_test_entry(yunit_test_suite_t *suite);

void test_wifi_provising_api(void)
{
	yunit_test_suite_t *suite;
	suite = yunit_add_test_suite("wifi_provising_api", init, cleanup, setup, teardown);
	api_wifi_provising_test_entry(suite);
}

