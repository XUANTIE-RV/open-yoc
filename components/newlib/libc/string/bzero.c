/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdlib.h>

void bzero(void *s, size_t n)
{
    memset(s, 0, n);
}
