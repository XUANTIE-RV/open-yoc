/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "serf/minilibc_stdio.h"

fmt_out_fn g_current_outputs = NULL;
fmt_in_fn g_current_inputs = NULL;

extern int uart_putc(int ch);
extern int uart_getc(void);

int fputc(int ch, FILE *stream)
{
    if (g_current_outputs) {
        g_current_outputs((char *)&ch, 1);
        return 0;
    }
    return uart_putc(ch);
}

int fgetc(FILE *stream)
{
    if (g_current_inputs)
        return g_current_inputs();
    return uart_getc();
}