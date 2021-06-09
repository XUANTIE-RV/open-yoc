/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "misc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void dump_data(uint8_t *data, int32_t len)
{
    int32_t i;

    for (i = 0; i < len; i++) {
        if (i % 16 == 0) {
            printf("\n");
        }

        printf("%02x ", data[i]);
    }

    printf("\n");
}