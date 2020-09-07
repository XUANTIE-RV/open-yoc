/*
 * Copyright (C) 2017-2019 C-SKY Microsystems Co., All rights reserved.
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

#include <app_config.h>

#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include <aos/cli.h>

#include <yoc/sysinfo.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <devices/devicelist.h>


#include "hw_config.h"

#define TAG "app"

void test_malloc(void)
{
    char *ptr = NULL;
    unsigned int size = 64;

    ptr = (char*)aos_malloc(size);
    if (NULL == ptr) {
        printf("aos_malloc failed\n");
    } else {
        printf("aos_malloc %d bytes successfully\n", size);
        aos_free(ptr);
    }
}

void test_zalloc(void)
{
    char *ptr = NULL;
    unsigned int size = 64;

    ptr = (char*)aos_zalloc(size);
    if (NULL == ptr) {
        printf("aos_zalloc failed\n");
    } else {
        printf("aos_zalloc %d bytes successfully\n", size);
        aos_free(ptr);
    }
}

void test_calloc(void)
{
    char *ptr = NULL;
    unsigned int size = 64;
    unsigned int n = 10;

    ptr = (char*)aos_calloc(n, size);
    if (NULL == ptr) {
        printf("aos_calloc failed\n");
    } else {
        printf("aos_calloc %d * %d bytes successfully\n", size, n);
        aos_free(ptr);
    }
}


static void cmd_mm_func(char *wbuf, int wbuf_len, int argc, char **argv)
{
    if(argc < 2)
        return;

    if(strcmp(argv[1], "malloc") == 0){
        test_malloc();
    } else if(strcmp(argv[1], "zalloc") == 0){
        test_zalloc();
    } else if(strcmp(argv[1], "calloc") == 0){
        test_calloc();
    } else {
        printf("param error\n");
    }
}

void test_mm_cmd(void)
{
    static const struct cli_command cmd_info = {"mm", "mm malloc/zalloc/calloc", cmd_mm_func};

    aos_cli_register_command(&cmd_info);
}

