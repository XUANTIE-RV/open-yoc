/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>

void transfer_data(char *addr, uint32_t size)
{
    uint32_t i;

    for (i=0; i<size; i++) {
        *(addr+i) = i%256;
    }
}

void dataset(uint8_t *addr, uint32_t size, uint8_t pattern){
    uint32_t i;
    for (i=0; i<size; i++) {
        *(addr+i) = pattern;
    }
}