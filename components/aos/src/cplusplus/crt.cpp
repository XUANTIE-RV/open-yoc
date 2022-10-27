/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <aos/kernel.h>

#include "crt.h"

void *operator new(size_t size)
{
    return aos_malloc(size);
}

void *operator new[](size_t size)
{
    return aos_malloc(size);
}

void operator delete(void *ptr)
{
    aos_free(ptr);
}

void operator delete[] (void *ptr)
{
    return aos_free(ptr);
}

void __cxa_pure_virtual(void)
{
    printf("Illegal to call a pure virtual function.\n");
}
