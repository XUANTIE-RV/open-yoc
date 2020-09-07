/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
/******************************************************************************
 * @file     lib_string.h
 * @brief    CSI Source File for string
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>

char *strtok_r(char *str, const char *delim, char **saveptr)
{
    if (!str && !(str = *saveptr))
        return NULL;

    str += strspn(str, delim);
    if (!*str)
        return *saveptr = 0;
    *saveptr = str + strcspn(str, delim);

    if (**saveptr)
        *(*saveptr)++ = 0;
    else
        *saveptr = 0;

    return str;
}