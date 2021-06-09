/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
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

extern void ble_sdk_mesh_test_entry(yunit_test_suite_t *suite);

void test_ble_sdk_mesh(void)
{
	yunit_test_suite_t *suite;
	suite = yunit_add_test_suite("blemeshapi", init, cleanup, setup, teardown);
	ble_sdk_mesh_test_entry(suite);
}

