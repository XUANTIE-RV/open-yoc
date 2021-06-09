/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <aos/aos.h>
#include <aos/kernel.h>

#include <soc.h>
#include <aos/hal/rng.h>
#include <ulog/ulog.h>
#include <yoc/atserver.h>


#define RNG1_PORT_NUM 0
random_dev_t rng1;

void aos_hal_get_range_test(void)
{
    int ret   = -1;
    int value = 0;
    int random[100] = {0};
    int i = 0;
    int k = 0;

    rng1.port = RNG1_PORT_NUM;

    for (i=0;i<100;i++) {
        ret = hal_random_num_read(rng1, &value, sizeof(int));
        if (ret != 0) {
            printf("rng read error !\n");
            AT_BACK_ERR();
            return;
        }
        random[i] = value;
        printf("value = %d\n",value);
    }
    
    for (i = 0; i<10; i++) {
        for (k = 9; k>=0; k--) {
            if (i != k) {
                if (random[i]==random[k]) {
                    k--;
                    break;
                }
            }
        }
        if (k>=0) {
            printf("元素重复du\n");
            break;
        }
    }
    if (i==10) {
        printf("元素无重复\n");
    }

    AT_BACK_OK();
}

void aos_hal_rng_api_test(void)
{
    int ret   = -1;
    int value = 0;

    rng1.port = RNG1_PORT_NUM;

    ret = hal_random_num_read(rng1, NULL, sizeof(int));
    if (ret == 0) {
        AT_BACK_ERR();
        return;
    }
    AT_BACK_OK();
}

void test_hal_rng(char *cmd, int type, char *data)
{
    if (strcmp((const char *)data, "'GET_RANGE'\0") == 0) {
        aos_hal_get_range_test();
    }else if (strcmp((const char *)data, "'API'\0") == 0) {
        aos_hal_rng_api_test();
    }
}