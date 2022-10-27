/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static char g_strerr[16];
char *strerror(int errnum)
{
    if (errnum > ESTRPIPE || errnum < EPERM) {
        return NULL;
    }

    switch (errnum) {
        case EIO:
            return "EIO";
        case EINVAL:
            return "EINVAL";
        case ENOMEM:
            return "ENOMEM";
        case EBUSY:
            return "EBUSY";
        default:
            sprintf(g_strerr, "%d", errnum);
    }

    return g_strerr;
}
