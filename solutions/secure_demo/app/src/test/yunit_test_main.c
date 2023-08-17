/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include "yunit.h"


void yunit_test_main(void)
{
    int ret = 0;

    ret = yunit_test_run();
    printf("\nTests completed with return value %d\n", ret);

    yunit_test_print_result();

    yunit_test_deinit();
}

void add_yunit_test(const char *name, yunit_test_case_proc proc) {
    static int ini = 0;
    static yunit_test_suite_t *suite;
    if(ini == 0) {
        yunit_test_init();
	    suite = yunit_add_test_suite(name, NULL, NULL, NULL, NULL);
        ini = 1;
    }

    yunit_add_test_case(suite, name, proc);
}