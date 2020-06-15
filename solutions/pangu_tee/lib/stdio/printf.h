/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>

int getch(void);
int putch(int c);

int printf(const char *format, ...);

#endif  /* PRINTF_H */
