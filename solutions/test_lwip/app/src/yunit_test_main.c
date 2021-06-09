/*
 * Copyright (C) 2019-2030 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include "yunit.h"
#include <unistd.h>


extern void yunit_test_api(void);

static void yunit_test_main(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    int item_count = argc;

    sleep(2);

    aos_set_except_callback(NULL);

    yunit_test_init();
    yunit_test_api();
    int ret = 0;

    if (item_count > 1) {
        int i;

        for (i = 1; i < item_count; i++) {
            yunit_test_suite_t *test_suite = yunit_get_test_suite(argv[i]);

            if (test_suite != NULL) {
                ret = yunit_run_test_suite(test_suite);
                printf("suite %s completed with %d\n", argv[i], ret);

                continue;
            }
	    else {
                printf("suite name err\n");
		return;
	    }

            const char *suite_case = argv[i];
            char *test = strrchr(suite_case, ':');

            if (test != NULL) {
                *test++ = '\0';

                test_suite = yunit_get_test_suite(suite_case);

                if (test_suite != NULL) {
                    yunit_test_case_t *test_case = yunit_get_test_case(test_suite, test);

                    if (test_case != NULL) {
                        ret = yunit_run_test_case(test_suite, test_case);
                        printf("suite %s completed with %d\n", argv[i], ret);
                    } else {
                        printf("test case %s not found\n", test);
                    }
                } else {
                    printf("suite %s not found\n", suite_case);
                }
            }
        }
    } else {
        ret = yunit_test_run();
        printf("\nTests completed with return value %d\n", ret);
    }

    yunit_test_print_result();

    yunit_test_deinit();
}

void cli_reg_cmd_yunit_test(void)
{
    static struct cli_command cmd_info = {
        "test",
        "test suit",
        yunit_test_main
    };

    aos_cli_register_command(&cmd_info);
}
