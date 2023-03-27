/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifdef __ICCARM__
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "aos/kernel.h"

#include "sys/types.h"
#include "sys/time.h"
#include "umm.h"

int errno;

extern long long  aos_now_ms(void);

__ATTRIBUTES void *malloc(unsigned int size)
{
    return umm_alloc(size);
}

__ATTRIBUTES void *realloc(void *old, unsigned int newlen)
{
    return umm_realloc(old, newlen);
}

__ATTRIBUTES void *calloc(size_t len, size_t elsize)
{
    void *mem;

    mem = umm_alloc(elsize * len);

    if (mem) {
        memset(mem, 0, elsize * len);
    }

    return mem;
}

__ATTRIBUTES void free(void *mem)
{
    umm_free(mem);
}

__ATTRIBUTES time_t time(time_t *tod)
{
    uint64_t t = aos_now_ms();
    return (time_t)(t / 1000);
}

int *__errno _PARAMS ((void))
{
    return 0;
}

void __assert_func(const char *a, int b, const char *c, const char *d)
{
    while (1);
}

/*TO DO*/
#pragma weak __write
size_t __write(int handle, const unsigned char *buffer, size_t size)
{
    if (buffer == 0) {
        /*
         * This means that we should flush internal buffers.  Since we don't we just return.
         * (Remember, "handle" == -1 means that all handles should be flushed.)
         */
        return 0;
    }

    /* This function only writes to "standard out" and "standard err" for all other file handles it returns failure. */
    if ((handle != 1) && (handle != 2)) {
        return ((size_t) - 1);
    }

    extern int uart_write(const void *buf, size_t size);
    uart_write((const void*)buffer, size);

    return size;
}

void bzero()
{

}

void __lseek()
{

}

void __close()
{

}

int remove(char const *p)
{
    return 0;
}

int gettimeofday(struct timeval *tp, void *ignore)
{
    return 0;
}

void getopt()
{

}

void optarg()
{

}

#endif

