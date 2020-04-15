/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

extern int __dtor_end__;
extern int __ctor_end__;
extern int __ctor_start__;

typedef void (*func_ptr) (void);

__attribute__((weak)) void cxx_system_init(void)
{
    func_ptr *p;
    for (p = (func_ptr *)&__ctor_end__ - 1; p >= (func_ptr *)&__ctor_start__; p--) {
        (*p) ();
    }
}