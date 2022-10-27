/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef CRT_H_
#define CRT_H_

#include <inttypes.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *operator new(size_t size);
void *operator new[](size_t size);

void operator delete(void * ptr);
void operator delete[] (void *ptr);

extern "C" void __cxa_pure_virtual(void);
extern "C" void  cxx_system_init(void);

#ifdef __cplusplus
}
#endif

#endif
