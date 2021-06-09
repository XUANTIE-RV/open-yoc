/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/debug.h>

void __assert_fail (const char *file, unsigned int line,
                        const char *func, const char *failedexpr)
{
    fprintf(stderr,
             "assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
             failedexpr, file, line,
             func ? ", function: " : "", func ? func : "");

    except_process(0);
}
