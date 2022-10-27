/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *strupr(char *str)
{
    char *orign = str;

    for (; *str != '\0'; str++)
        *str = toupper(*str);

    return orign;
}
