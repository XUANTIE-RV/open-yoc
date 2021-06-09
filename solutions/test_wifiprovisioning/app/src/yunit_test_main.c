/*
 * Copyright (C) 2017 C-SKY Microsystems Co., All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdlib.h>
#include <stdio.h>
#include <aos/kernel.h>
#include <aos/cli.h>
#include "yunit.h"
#include <unistd.h>
#include <ulog/ulog.h>

#define TAG "YUNIT"


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
                LOGI(TAG, "suite %s completed with %d\n", argv[i], ret);

                continue;
            }
	    else {
                LOGE(TAG, "suite name err\n");
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
                        LOGI(TAG, "suite %s completed with %d\n", argv[i], ret);
                    } else {
                        LOGE(TAG, "test case %s not found\n", test);
                    }
                } else {
                    LOGE(TAG, "suite %s not found\n", suite_case);
                }
            }
        }
    } else {
        ret = yunit_test_run();
        LOGE(TAG, "\nTests completed with return value %d\n", ret);
    }

    yunit_test_print_result();

    yunit_test_deinit();
}

void test_yunit_test_register_cmd(void)
{
    static struct cli_command cmd_info = {
        "test",
        "test suit",
        yunit_test_main
    };

    aos_cli_register_command(&cmd_info);
}
