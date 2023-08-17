/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <string.h>
#include <aos/kernel.h>


static void except_process_function(int err, const char *file, int line, const char *func_name, void *caller)
{
#if defined(CONFIG_DEBUG) && (CONFIG_DEBUG > 0)
    printf("Except! errno is %s, function: %s at %s:%d, caller: %p\n", strerror(err), func_name, file, line, caller);
    while(1);
#else
    printf("Except! errno is %s, caller: %p\n", strerror(err), caller);
    aos_reboot();
#endif
}

static except_process_t process = except_process_function;

void aos_set_except_callback(except_process_t except)
{
    process = except;
}

void aos_set_except_default()
{
    process = except_process_function;
}

void aos_except_process(int err, const char *file, int line, const char *func_name, void *caller)
{
    if (process)
        process(err, file, line, func_name, caller);
}
